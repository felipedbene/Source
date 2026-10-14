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


#define SRQ_BORING	0

#define SWI_NET		0
#define PI_SWI(x)	(PI_SOFT)

__BEGIN_DECLS
int lkpi_kthread_add(void (*func)(void*), void* arg, void* p,
	struct thread** newtdp, int flags, int pages, const char* fmt, ...);
void lkpi_sched_prio(struct thread* td, u_char prio);
void lkpi_sched_add(struct thread* td, int flags);
void lkpi_kthread_exit(void);
__END_DECLS

#define kthread_add		lkpi_kthread_add
#define sched_prio		lkpi_sched_prio
#define sched_add		lkpi_sched_add
#define kthread_exit	lkpi_kthread_exit

#define thread_lock(td)
#define thread_unlock(td)


#endif /* _LKPI_HAIKU_SYS_KTHREAD_H_ */
