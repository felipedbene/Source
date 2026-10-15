/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * FreeBSD sleepqueue(9) for LinuxKPI (linux_schedule.c, linux_compat.c).
 *
 * Model: wait channels hash onto chains; sleepq_lock() is the chain's
 * mutex. A waiter is enqueued by sleepq_add() and blocks on its own
 * semaphore in sleepq_wait*(). Wakeups cannot be lost: sleepq_signal()
 * dequeues the waiter and releases its semaphore while holding the chain
 * lock, so a release that happens right after the sleeper drops the lock
 * (but before it blocks) is simply consumed by acquire_sem.
 *
 * Divergences from FreeBSD, matching what LinuxKPI's call sites need:
 * - The chain lock is a mutex, not a spin mutex: every LinuxKPI caller
 *   runs in thread context on this port (interrupt handlers are threaded)
 *   and sleepq_add() may need to allocate.
 * - Every sleepq_add() is followed by exactly one sleepq_*wait*() (true of
 *   linux_schedule.c and linux_compat.c); add-then-release-without-wait is
 *   not supported.
 * - The *_sig variants depend on acquire_sem_etc(B_CAN_INTERRUPT), which
 *   only fires if a signal reaches the kernel thread; LinuxKPI's own
 *   linux_send_sig() path is the only expected source.
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mutex.h>
#include <sys/queue.h>
#include <sys/sleepqueue.h>
#include <sys/systm.h>

#include <KernelExport.h>


MALLOC_DEFINE(M_LKPI_SLEEPQ, "lkpisleepq", "LinuxKPI sleepqueue waiters");

#define LKPI_SLEEPQ_CHAINS	64	/* must be a power of two */

struct lkpi_sleepq_waiter {
	TAILQ_ENTRY(lkpi_sleepq_waiter) link;
	SLIST_ENTRY(lkpi_sleepq_waiter) free_link;
	sem_id			sem;
	thread_id		thread;
	const void*		wchan;			/* NULL once dequeued */
	int				timeout;		/* ticks (ms, hz = 1000); 0 = none */
};

struct lkpi_sleepq_chain {
	struct mtx		lock;
	TAILQ_HEAD(, lkpi_sleepq_waiter) waiters;
};

static struct lkpi_sleepq_chain sChains[LKPI_SLEEPQ_CHAINS];

/* Waiter structs (each owning a semaphore) are recycled through a free
 * list, so steady-state sleeping allocates nothing. */
static SLIST_HEAD(, lkpi_sleepq_waiter) sFreeWaiters
	= SLIST_HEAD_INITIALIZER(sFreeWaiters);
static spinlock sFreeWaitersLock = B_SPINLOCK_INITIALIZER;


static struct lkpi_sleepq_chain*
sleepq_chain(const void* wchan)
{
	uintptr_t v = (uintptr_t)wchan;
	return &sChains[(v >> 4 ^ v >> 12) & (LKPI_SLEEPQ_CHAINS - 1)];
}


static struct lkpi_sleepq_waiter*
sleepq_waiter_get(void)
{
	struct lkpi_sleepq_waiter* w;
	cpu_status state;

	state = disable_interrupts();
	acquire_spinlock(&sFreeWaitersLock);
	w = SLIST_FIRST(&sFreeWaiters);
	if (w != NULL)
		SLIST_REMOVE_HEAD(&sFreeWaiters, free_link);
	release_spinlock(&sFreeWaitersLock);
	restore_interrupts(state);

	if (w == NULL) {
		w = malloc(sizeof(*w), M_LKPI_SLEEPQ, M_WAITOK);
		w->sem = create_sem(0, "lkpi sleepq");
		if (w->sem < 0)
			panic("lkpi sleepq: create_sem: %" B_PRId32, w->sem);
	}

	w->thread = find_thread(NULL);
	w->wchan = NULL;
	w->timeout = 0;
	return w;
}


static void
sleepq_waiter_put(struct lkpi_sleepq_waiter* w)
{
	cpu_status state;

	state = disable_interrupts();
	acquire_spinlock(&sFreeWaitersLock);
	SLIST_INSERT_HEAD(&sFreeWaiters, w, free_link);
	release_spinlock(&sFreeWaitersLock);
	restore_interrupts(state);
}


static struct lkpi_sleepq_waiter*
sleepq_self_locked(struct lkpi_sleepq_chain* chain, const void* wchan)
{
	struct lkpi_sleepq_waiter* w;
	thread_id self = find_thread(NULL);

	TAILQ_FOREACH(w, &chain->waiters, link) {
		if (w->wchan == wchan && w->thread == self)
			return w;
	}
	return NULL;
}


void
sleepq_lock(const void* wchan)
{
	mtx_lock(&sleepq_chain(wchan)->lock);
}


void
sleepq_release(const void* wchan)
{
	mtx_unlock(&sleepq_chain(wchan)->lock);
}


void
sleepq_add(const void* wchan, struct lock_object* lock __unused,
	const char* wmesg __unused, int flags __unused, int queue __unused)
{
	struct lkpi_sleepq_chain* chain = sleepq_chain(wchan);
	struct lkpi_sleepq_waiter* w;

	w = sleepq_waiter_get();
	w->wchan = wchan;
	TAILQ_INSERT_TAIL(&chain->waiters, w, link);
}


void
sleepq_set_timeout(const void* wchan, int timo)
{
	struct lkpi_sleepq_chain* chain = sleepq_chain(wchan);
	struct lkpi_sleepq_waiter* w;

	w = sleepq_self_locked(chain, wchan);
	if (w == NULL)
		panic("sleepq_set_timeout: thread not on sleepqueue");
	w->timeout = timo;
}


/*
 * Common tail of the wait variants: resolve how the semaphore wait ended.
 * A waiter still on the chain timed out (or was interrupted) for real; a
 * dequeued waiter was signaled, and the matching release_sem already
 * happened before the signaler dropped the chain lock, so it can be
 * consumed without blocking.
 */
static int
sleepq_wait_done(struct lkpi_sleepq_chain* chain, struct lkpi_sleepq_waiter* w,
	status_t status)
{
	int error = 0;

	if (status != B_OK) {
		mtx_lock(&chain->lock);
		if (w->wchan != NULL) {
			TAILQ_REMOVE(&chain->waiters, w, link);
			w->wchan = NULL;
			error = status == B_TIMED_OUT ? EWOULDBLOCK : EINTR;
		} else {
			acquire_sem_etc(w->sem, 1, B_RELATIVE_TIMEOUT, 0);
		}
		mtx_unlock(&chain->lock);
	}

	sleepq_waiter_put(w);
	return error;
}


static int
sleepq_wait_common(const void* wchan, bool catchSignals)
{
	struct lkpi_sleepq_chain* chain = sleepq_chain(wchan);
	struct lkpi_sleepq_waiter* w;
	status_t status;
	bigtime_t deadline = 0;
	uint32 semFlags = 0;

	w = sleepq_self_locked(chain, wchan);
	if (w == NULL)
		panic("sleepq_wait: thread not on sleepqueue");

	if (w->timeout != 0) {
		deadline = system_time() + (bigtime_t)w->timeout * 1000;
		semFlags |= B_ABSOLUTE_TIMEOUT;
	}
	if (catchSignals)
		semFlags |= B_CAN_INTERRUPT;

	mtx_unlock(&chain->lock);

	do {
		status = acquire_sem_etc(w->sem, 1, semFlags, deadline);
		/* an uninterruptible sleep must not end early */
	} while (status == B_INTERRUPTED && !catchSignals);

	return sleepq_wait_done(chain, w, status);
}


void
sleepq_wait(const void* wchan, int pri __unused)
{
	sleepq_wait_common(wchan, false);
}


int
sleepq_wait_sig(const void* wchan, int pri __unused)
{
	return sleepq_wait_common(wchan, true);
}


int
sleepq_timedwait(const void* wchan, int pri __unused)
{
	return sleepq_wait_common(wchan, false);
}


int
sleepq_timedwait_sig(const void* wchan, int pri __unused)
{
	return sleepq_wait_common(wchan, true);
}


static int
sleepq_wakeup_locked(struct lkpi_sleepq_chain* chain, const void* wchan,
	bool all)
{
	struct lkpi_sleepq_waiter* w;
	struct lkpi_sleepq_waiter* next;
	int woken = 0;

	TAILQ_FOREACH_SAFE(w, &chain->waiters, link, next) {
		if (w->wchan != wchan)
			continue;
		TAILQ_REMOVE(&chain->waiters, w, link);
		w->wchan = NULL;
		/* under the chain lock; see sleepq_wait_done() */
		release_sem_etc(w->sem, 1, B_DO_NOT_RESCHEDULE);
		woken++;
		if (!all)
			break;
	}
	return woken;
}


int
sleepq_signal(const void* wchan, int flags __unused, int pri __unused,
	int queue __unused)
{
	return sleepq_wakeup_locked(sleepq_chain(wchan), wchan, false);
}


int
sleepq_broadcast(const void* wchan, int flags __unused, int pri __unused,
	int queue __unused)
{
	return sleepq_wakeup_locked(sleepq_chain(wchan), wchan, true);
}


static void
lkpi_sleepq_sysinit(void* arg __unused)
{
	int i;

	for (i = 0; i < LKPI_SLEEPQ_CHAINS; i++) {
		mtx_init(&sChains[i].lock, "lkpi sleepq chain", NULL, MTX_DEF);
		TAILQ_INIT(&sChains[i].waiters);
	}
}
SYSINIT(lkpi_sleepq, SI_SUB_LOCK, SI_ORDER_FIRST, lkpi_sleepq_sysinit, NULL);


static void
lkpi_sleepq_sysuninit(void* arg __unused)
{
	struct lkpi_sleepq_waiter* w;
	int i;

	/* No waiters can be left: the driver is quiesced before unload. */
	while ((w = SLIST_FIRST(&sFreeWaiters)) != NULL) {
		SLIST_REMOVE_HEAD(&sFreeWaiters, free_link);
		delete_sem(w->sem);
		free(w, M_LKPI_SLEEPQ);
	}

	for (i = 0; i < LKPI_SLEEPQ_CHAINS; i++)
		mtx_destroy(&sChains[i].lock);
}
SYSUNINIT(lkpi_sleepq, SI_SUB_LOCK, SI_ORDER_FIRST, lkpi_sleepq_sysuninit,
	NULL);
