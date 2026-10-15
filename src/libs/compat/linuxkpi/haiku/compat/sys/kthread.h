/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_KTHREAD_H_
#define _LKPI_HAIKU_SYS_KTHREAD_H_

/*
 * Shadows freebsd_network's <sys/kthread.h>. freebsd_network hands out a
 * thread_id cast to struct thread*, which LinuxKPI would dereference. The
 * LinuxKPI variants return real shadow threads instead; they are renamed so
 * both implementations can be linked into the same driver.
 */

#include <sys/pcpu.h>
#include <sys/priority.h>
#include <sys/sched.h>		/* sched_prio, sched_add, thread model */


#define SRQ_BORING	0

__BEGIN_DECLS
int lkpi_kthread_add(void (*func)(void*), void* arg, void* p,
	struct thread** newtdp, int flags, int pages, const char* fmt, ...);
void lkpi_kthread_exit(void);
__END_DECLS

#define kthread_add		lkpi_kthread_add
#define kthread_exit	lkpi_kthread_exit


#endif /* _LKPI_HAIKU_SYS_KTHREAD_H_ */
