/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_TASKQUEUE_H_
#define _LKPI_HAIKU_SYS_TASKQUEUE_H_

/*
 * Extends freebsd_network's <sys/taskqueue.h> with the newer FreeBSD API
 * LinuxKPI's <linux/kthread.h> worker inlines use. Declared so they
 * compile; implemented if a driver turns out to use kthread workers
 * (rtw88 does not).
 */

#include_next <sys/taskqueue.h>


#define TASKQUEUE_FAIL_IF_PENDING	(1 << 0)
#define TASKQUEUE_FAIL_IF_CANCELING	(1 << 1)

#define TASK_INITIALIZER(priority, func, context) \
	{ .ta_pending = 0, .ta_priority = (priority), .ta_flags = 0, \
	  .ta_func = (func), .ta_context = (context) }

int taskqueue_enqueue_flags(struct taskqueue* queue, struct task* task,
	int flags);
int taskqueue_poll_is_busy(struct taskqueue* queue, struct task* task);


#endif /* _LKPI_HAIKU_SYS_TASKQUEUE_H_ */
