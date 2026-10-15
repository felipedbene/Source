/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Shadow struct thread for LinuxKPI.
 *
 * LinuxKPI needs curthread to be a stable, per-thread struct thread: it stores
 * "current" (td_lkpi_task) in it and records it as sx lock owner. Haiku has no
 * such structure we may use, so we keep one shadow per Haiku thread that enters
 * LinuxKPI, in a hash table keyed by thread_id. Haiku thread IDs increase
 * monotonically, so a stale entry can never be mistaken for a new thread.
 *
 * Haiku offers drivers no thread-death notification. A reaper thread
 * periodically drops shadows of threads that no longer exist and fires the
 * thread_dtor event, which is how linux_current.c frees task_structs (as it
 * does on FreeBSD). Reaping never happens from curthread itself, as the
 * dtor handlers may need locks the caller already holds.
 */

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/kthread.h>
#include <sys/eventhandler.h>
#include <sys/rtprio.h>
#include <sys/signalvar.h>
#include <sys/sx.h>
#include <sys/systm.h>

#include <KernelExport.h>
#include <string.h>

#include <interrupts.h>

/* From the kernel's private <smp.h>, which can only be included from C++. */
int32 smp_get_num_cpus(void);
int32 smp_get_current_cpu(void);


#define LKPI_THREAD_HASH_SIZE		128		/* must be a power of two */
#define LKPI_THREAD_REAP_INTERVAL	2000000	/* usecs */

#ifndef RFSTOPPED
#define RFSTOPPED	(1 << 17)
#endif


MALLOC_DEFINE(M_LKPI_THREAD, "lkpithread", "LinuxKPI shadow threads");

SLIST_HEAD(lkpi_thread_list, thread);

static struct lkpi_thread_list sThreadHash[LKPI_THREAD_HASH_SIZE];
static spinlock sThreadHashLock = B_SPINLOCK_INITIALIZER;

static struct proc sKernelProc = { .p_pid = B_SYSTEM_TEAM };
static struct ucred sRootCred = { .cr_uid = 0 };

/* Handed out when no shadow exists yet and we cannot allocate one because
 * interrupts are disabled. Nothing can preempt the caller on that CPU until
 * interrupts are enabled again, so sharing one per CPU is safe. Allocated in
 * lkpi_thread_sysinit(), one per CPU. */
static struct thread* sFallbackThreads;

static thread_id sReaperThread = -1;
static volatile bool sReaperQuit;


static inline struct lkpi_thread_list*
lkpi_thread_bucket(thread_id id)
{
	return &sThreadHash[(uint32)id & (LKPI_THREAD_HASH_SIZE - 1)];
}


static struct thread*
lkpi_thread_lookup_locked(thread_id id)
{
	struct thread* td;

	SLIST_FOREACH(td, lkpi_thread_bucket(id), td_hash_link) {
		if (td->td_tid == id)
			return td;
	}
	return NULL;
}


static struct thread*
lkpi_thread_lookup(thread_id id)
{
	struct thread* td;
	cpu_status state = disable_interrupts();
	acquire_spinlock(&sThreadHashLock);
	td = lkpi_thread_lookup_locked(id);
	release_spinlock(&sThreadHashLock);
	restore_interrupts(state);
	return td;
}


/*
 * PROC_LOCK on the one kernel proc is the shadow-table spinlock, so
 * FOREACH_THREAD_IN_PROC can walk the table under it. The saved interrupt
 * state is protected by the spinlock itself: it is written after
 * acquiring and read back before releasing.
 */
static cpu_status sProcLockCpuState;


void
lkpi_proc_lock(struct proc* p __unused)
{
	cpu_status state = disable_interrupts();
	acquire_spinlock(&sThreadHashLock);
	sProcLockCpuState = state;
}


void
lkpi_proc_unlock(struct proc* p __unused)
{
	cpu_status state = sProcLockCpuState;
	release_spinlock(&sThreadHashLock);
	restore_interrupts(state);
}


struct thread*
lkpi_proc_thread_first(struct proc* p __unused)
{
	for (int i = 0; i < LKPI_THREAD_HASH_SIZE; i++) {
		struct thread* td = SLIST_FIRST(&sThreadHash[i]);
		if (td != NULL)
			return td;
	}
	return NULL;
}


struct thread*
lkpi_proc_thread_next(struct thread* td)
{
	struct thread* next = SLIST_NEXT(td, td_hash_link);
	if (next != NULL)
		return next;

	for (uint32 i = ((uint32)td->td_tid & (LKPI_THREAD_HASH_SIZE - 1)) + 1;
			i < LKPI_THREAD_HASH_SIZE; i++) {
		next = SLIST_FIRST(&sThreadHash[i]);
		if (next != NULL)
			return next;
	}
	return NULL;
}


struct thread*
tdfind(lwpid_t tid, pid_t pid __unused)
{
	struct thread* td;

	lkpi_proc_lock(&sKernelProc);
	td = lkpi_thread_lookup_locked(tid);
	if (td == NULL)
		lkpi_proc_unlock(&sKernelProc);
	return td;
}


struct proc*
pfind(pid_t pid)
{
	if (pid != sKernelProc.p_pid)
		return NULL;
	lkpi_proc_lock(&sKernelProc);
	return &sKernelProc;
}


struct proc*
lkpi_allproc_first(void)
{
	return &sKernelProc;
}


/* All shadow threads belong to the one kernel proc. Initialized in
 * lkpi_thread_sysinit(). */
struct sx allproc_lock;


/*
 * FreeBSD's thread_lock(td) is a per-thread spin lock; here one global
 * spinlock covers them all. Only linux_rcu.c's priority boosting takes
 * it, briefly, so contention is not a concern.
 */
static spinlock sThreadLock = B_SPINLOCK_INITIALIZER;
static cpu_status sThreadLockCpuState;


void
lkpi_thread_lock(struct thread* td __unused)
{
	cpu_status state = disable_interrupts();
	acquire_spinlock(&sThreadLock);
	sThreadLockCpuState = state;
}


void
lkpi_thread_unlock(struct thread* td __unused)
{
	cpu_status state = sThreadLockCpuState;
	release_spinlock(&sThreadLock);
	restore_interrupts(state);
}


void
tdsignal(struct thread* td, int signo)
{
	/* Runs under PROC_LOCK (a spinlock), so this only records the signal
	 * for the signal_pending() emulation. It does not interrupt a wait
	 * already in progress: nothing on the wifi path sends signals to a
	 * sleeping kthread (kthread parking uses completions). */
	SIGADDSET(td->td_siglist, signo);
}


void
thread_reap_barrier(void)
{
	/* Reaping stays single-threaded (see lkpi_thread_reap); waiting out a
	 * full reaper cycle guarantees any pass that had started before this
	 * call - and the thread_dtor handlers it fires - has finished. */
	if (sReaperThread >= 0)
		snooze(2 * LKPI_THREAD_REAP_INTERVAL);
}


static void
lkpi_thread_init_shadow(struct thread* td, thread_id id, const char* name)
{
	thread_info info;

	memset(td, 0, sizeof(*td));
	td->td_tid = id;
	td->td_proc = &sKernelProc;
	td->td_ucred = &sRootCred;

	if (name == NULL && get_thread_info(id, &info) == B_OK)
		name = info.name;
	if (name != NULL)
		strlcpy(td->td_name, name, sizeof(td->td_name));
}


/* Inserts td unless a shadow for its thread already exists. Returns the shadow
 * that is in the table; td is freed if it lost the race. */
static struct thread*
lkpi_thread_insert(struct thread* td)
{
	struct thread* existing;
	cpu_status state = disable_interrupts();
	acquire_spinlock(&sThreadHashLock);
	existing = lkpi_thread_lookup_locked(td->td_tid);
	if (existing == NULL)
		SLIST_INSERT_HEAD(lkpi_thread_bucket(td->td_tid), td, td_hash_link);
	release_spinlock(&sThreadHashLock);
	restore_interrupts(state);

	if (existing != NULL) {
		free(td, M_LKPI_THREAD);
		return existing;
	}
	return td;
}


struct thread*
lkpi_curthread(void)
{
	thread_id id = find_thread(NULL);
	struct thread* td = lkpi_thread_lookup(id);
	if (td != NULL)
		return td;

	if (!are_interrupts_enabled()) {
		if (sFallbackThreads == NULL)
			panic("lkpi_curthread: called with interrupts disabled before init");
		td = &sFallbackThreads[smp_get_current_cpu()];
		td->td_tid = id;
		td->td_proc = &sKernelProc;
		td->td_ucred = &sRootCred;
		return td;
	}

	td = malloc(sizeof(*td), M_LKPI_THREAD, M_WAITOK);
	if (td == NULL)
		panic("lkpi_curthread: out of memory");
	lkpi_thread_init_shadow(td, id, NULL);
	return lkpi_thread_insert(td);
}


bool
lkpi_thread_can_sleep(void)
{
	return are_interrupts_enabled();
}


static void
lkpi_thread_free(struct thread* td)
{
	EVENTHANDLER_INVOKE(thread_dtor, td);
	free(td, M_LKPI_THREAD);
}


/* Removes and frees the shadows of all threads for which isDead() holds.
 * Only the reaper (and uninit, after the reaper is gone) remove entries, and
 * inserts only ever prepend, so a bucket can be walked without the lock. */
static void
lkpi_thread_reap(bool all)
{
	thread_info info;
	int i;

	for (i = 0; i < LKPI_THREAD_HASH_SIZE; i++) {
		struct thread* td;
		struct thread* next;
		cpu_status state;

		state = disable_interrupts();
		acquire_spinlock(&sThreadHashLock);
		td = SLIST_FIRST(&sThreadHash[i]);
		release_spinlock(&sThreadHashLock);
		restore_interrupts(state);

		for (; td != NULL; td = next) {
			next = SLIST_NEXT(td, td_hash_link);
			if (!all && get_thread_info(td->td_tid, &info) == B_OK)
				continue;

			state = disable_interrupts();
			acquire_spinlock(&sThreadHashLock);
			SLIST_REMOVE(&sThreadHash[i], td, thread, td_hash_link);
			release_spinlock(&sThreadHashLock);
			restore_interrupts(state);

			lkpi_thread_free(td);
		}
	}
}


static status_t
lkpi_thread_reaper(void* data __unused)
{
	while (!sReaperQuit) {
		snooze(LKPI_THREAD_REAP_INTERVAL);
		lkpi_thread_reap(false);
	}
	return B_OK;
}


static void
lkpi_thread_sysinit(void* arg __unused)
{
	sFallbackThreads = malloc(sizeof(struct thread) * smp_get_num_cpus(),
		M_LKPI_THREAD, M_WAITOK | M_ZERO);
	if (sFallbackThreads == NULL)
		panic("lkpi_thread_sysinit: out of memory");

	sx_init(&allproc_lock, "allproc");

	sReaperQuit = false;
	sReaperThread = spawn_kernel_thread(lkpi_thread_reaper, "lkpi thread reaper",
		B_LOW_PRIORITY, NULL);
	if (sReaperThread >= 0)
		resume_thread(sReaperThread);
}
SYSINIT(lkpi_thread, SI_SUB_EVENTHANDLER, SI_ORDER_FIRST, lkpi_thread_sysinit,
	NULL);


static void
lkpi_thread_sysuninit(void* arg __unused)
{
	status_t result;

	if (sReaperThread >= 0) {
		sReaperQuit = true;
		wait_for_thread(sReaperThread, &result);
		sReaperThread = -1;
	}
	lkpi_thread_reap(true);

	sx_destroy(&allproc_lock);

	free(sFallbackThreads, M_LKPI_THREAD);
	sFallbackThreads = NULL;
}
SYSUNINIT(lkpi_thread, SI_SUB_EVENTHANDLER, SI_ORDER_FIRST,
	lkpi_thread_sysuninit, NULL);


/* #pragma mark - kthreads */


struct lkpi_kthread_args {
	void	(*func)(void*);
	void*	arg;
};


static status_t
lkpi_kthread_trampoline(void* data)
{
	struct lkpi_kthread_args args = *(struct lkpi_kthread_args*)data;
	free(data, M_LKPI_THREAD);

	args.func(args.arg);
	return B_OK;
}


int
lkpi_kthread_add(void (*func)(void*), void* arg, void* p __unused,
	struct thread** newtdp, int flags, int pages __unused, const char* fmt, ...)
{
	struct lkpi_kthread_args* args;
	struct thread* td;
	char name[B_OS_NAME_LENGTH];
	thread_id id;
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(name, sizeof(name), fmt, ap);
	va_end(ap);

	args = malloc(sizeof(*args), M_LKPI_THREAD, M_WAITOK);
	td = malloc(sizeof(*td), M_LKPI_THREAD, M_WAITOK);
	if (args == NULL || td == NULL) {
		free(args, M_LKPI_THREAD);
		free(td, M_LKPI_THREAD);
		return ENOMEM;
	}
	args->func = func;
	args->arg = arg;

	/* Kernel threads are spawned suspended, so the shadow is in the table
	 * before the thread can run and look itself up. */
	id = spawn_kernel_thread(lkpi_kthread_trampoline, name, B_NORMAL_PRIORITY,
		args);
	if (id < 0) {
		free(args, M_LKPI_THREAD);
		free(td, M_LKPI_THREAD);
		return ENOMEM;
	}

	lkpi_thread_init_shadow(td, id, name);
	td->td_pflags |= TDP_KTHREAD;
	td->td_priority = B_NORMAL_PRIORITY;
	td = lkpi_thread_insert(td);

	if (newtdp != NULL)
		*newtdp = td;
	if ((flags & RFSTOPPED) == 0)
		resume_thread(id);
	return 0;
}


int
linux_alloc_current_noop(struct thread* td __unused, int flags __unused)
{
	return ENOTSUP;
}

/* linux_current.c points this at linux_alloc_current() while it is loaded. */
int (*lkpi_alloc_current)(struct thread*, int) = linux_alloc_current_noop;


void
lkpi_sched_prio(struct thread* td, u_char prio)
{
	/* freebsd_network's PI_* priorities are already Haiku priorities, with
	 * one exception: a literal 0 is FreeBSD's "highest" (linux_rcu.c
	 * boosts with it), while 0 is idle on Haiku - no caller ever means
	 * that, so translate it to a boost. */
	if (prio == 0)
		prio = B_URGENT_DISPLAY_PRIORITY;

	td->td_priority = prio;
	set_thread_priority(td->td_tid, prio);
}


/* From the kernel's private <thread.h>, which is C++-only. */
void thread_yield(void);


void
lkpi_mi_switch(int flags __unused)
{
	/* Entered with thread_lock() held; FreeBSD's mi_switch() drops it
	 * before switching, and linux_rcu.c relies on exactly that. */
	lkpi_thread_unlock(NULL);
	thread_yield();
}


void
lkpi_sched_add(struct thread* td, int flags __unused)
{
	resume_thread(td->td_tid);
}


int
rtp_to_pri(struct rtprio* rtp, struct thread* td)
{
	int32 priority;

	if (rtp->prio > RTP_PRIO_MAX)
		return EINVAL;

	switch (RTP_PRIO_BASE(rtp->type)) {
		case RTP_PRIO_REALTIME:
			/* RTP_PRIO_MIN (0) is FreeBSD's highest realtime priority,
			 * RTP_PRIO_MAX its lowest; map onto Haiku's realtime band. */
			priority = B_REAL_TIME_PRIORITY - (rtp->prio
					* (B_REAL_TIME_PRIORITY - B_REAL_TIME_DISPLAY_PRIORITY))
				/ RTP_PRIO_MAX;
			break;
		case RTP_PRIO_NORMAL:
			priority = B_NORMAL_PRIORITY;
			break;
		case RTP_PRIO_IDLE:
			priority = B_LOW_PRIORITY;
			break;
		default:
			return EINVAL;
	}

	lkpi_sched_prio(td, priority);
	return 0;
}


void
lkpi_kthread_exit(void)
{
	/* The shadow is freed by the reaper once the thread is gone. */
	exit_thread(B_OK);
}
