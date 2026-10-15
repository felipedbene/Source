/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_GTASKQUEUE_H_
#define _LKPI_HAIKU_SYS_GTASKQUEUE_H_

/*
 * The subset of FreeBSD's <sys/gtaskqueue.h> that linux_tasklet.c uses,
 * on freebsd_network's taskqueue. There is one "softirq" queue instead
 * of FreeBSD's per-CPU groups: taskqgroup_attach_cpu()'s CPU binding is
 * ignored, which costs tasklet locality, not correctness
 * (lkpi_gtaskqueue.c).
 */

#include <sys/taskqueue.h>


typedef void gtask_fn_t(void* context);

struct grouptask {
	struct task			gt_task;
	gtask_fn_t*			gt_func;
	void*				gt_context;
	struct taskqueue*	gt_taskqueue;
};

struct taskqgroup;
extern struct taskqgroup* qgroup_softirq;

void lkpi_grouptask_init(struct grouptask* gtask, int priority,
	gtask_fn_t* func, void* context);
void taskqgroup_attach_cpu(struct taskqgroup* qgroup,
	struct grouptask* gtask, void* uniq, int cpu, void* dev, void* irq,
	const char* name);
void taskqgroup_detach(struct taskqgroup* qgroup, struct grouptask* gtask);
void taskqgroup_drain_all(struct taskqgroup* qgroup);

#define GROUPTASK_INIT(gtask, priority, func, context) \
	lkpi_grouptask_init((gtask), (priority), (func), (context))
#define GROUPTASK_ENQUEUE(gtask) \
	taskqueue_enqueue((gtask)->gt_taskqueue, &(gtask)->gt_task)


#endif /* _LKPI_HAIKU_SYS_GTASKQUEUE_H_ */
