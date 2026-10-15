/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * FreeBSD's grouped taskqueues for linux_tasklet.c, on freebsd_network's
 * taskqueue (see haiku/compat/sys/gtaskqueue.h).
 */

#include <sys/param.h>
#include <sys/gtaskqueue.h>
#include <sys/interrupt.h>
#include <sys/kernel.h>
#include <sys/malloc.h>


static struct taskqueue* sSoftirqQueue;

struct taskqgroup {
	int	unused;
};

static struct taskqgroup sSoftirqGroup;
struct taskqgroup* qgroup_softirq = &sSoftirqGroup;


static void
lkpi_gtask_run(void* context, int pending __unused)
{
	struct grouptask* gtask = context;
	gtask->gt_func(gtask->gt_context);
}


void
lkpi_grouptask_init(struct grouptask* gtask, int priority, gtask_fn_t* func,
	void* context)
{
	TASK_INIT(&gtask->gt_task, priority, lkpi_gtask_run, gtask);
	gtask->gt_func = func;
	gtask->gt_context = context;
	gtask->gt_taskqueue = NULL;
}


void
taskqgroup_attach_cpu(struct taskqgroup* qgroup __unused,
	struct grouptask* gtask, void* uniq __unused, int cpu __unused,
	void* dev __unused, void* irq __unused, const char* name __unused)
{
	gtask->gt_taskqueue = sSoftirqQueue;
}


void
taskqgroup_detach(struct taskqgroup* qgroup __unused, struct grouptask* gtask)
{
	taskqueue_drain(sSoftirqQueue, &gtask->gt_task);
	gtask->gt_taskqueue = NULL;
}


void
taskqgroup_drain_all(struct taskqgroup* qgroup __unused)
{
	taskqueue_drain_all(sSoftirqQueue);
}


/*
 * Newer FreeBSD taskqueue API linux_work.c uses. The poll only feeds
 * flush_work()'s return value - taskqueue_drain() does the actual
 * waiting - so pending-count accuracy suffices ("running right now"
 * is not visible outside freebsd_network's taskqueue).
 */
int
taskqueue_poll_is_busy(struct taskqueue* queue __unused, struct task* task)
{
	return task->ta_pending != 0;
}


int
taskqueue_enqueue_flags(struct taskqueue* queue, struct task* task,
	int flags)
{
	if ((flags & TASKQUEUE_FAIL_IF_PENDING) != 0 && task->ta_pending != 0)
		return EEXIST;
	/* no cancel state is visible; TASKQUEUE_FAIL_IF_CANCELING passes */
	return taskqueue_enqueue(queue, task);
}


static void
lkpi_gtaskqueue_sysinit(void* arg __unused)
{
	sSoftirqQueue = taskqueue_create_fast("lkpi softirq", M_WAITOK,
		taskqueue_thread_enqueue, &sSoftirqQueue);
	if (sSoftirqQueue == NULL)
		panic("lkpi_gtaskqueue_sysinit: out of memory");
	taskqueue_start_threads(&sSoftirqQueue, 1, PI_SWI(SWI_NET),
		"lkpi softirq");
}
SYSINIT(lkpi_gtaskqueue, SI_SUB_TASKQ, SI_ORDER_SECOND,
	lkpi_gtaskqueue_sysinit, NULL);


static void
lkpi_gtaskqueue_sysuninit(void* arg __unused)
{
	taskqueue_free(sSoftirqQueue);
	sSoftirqQueue = NULL;
}
SYSUNINIT(lkpi_gtaskqueue, SI_SUB_TASKQ, SI_ORDER_SECOND,
	lkpi_gtaskqueue_sysuninit, NULL);
