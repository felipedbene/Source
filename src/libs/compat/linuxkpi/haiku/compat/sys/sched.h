/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SCHED_H_
#define _LKPI_HAIKU_SYS_SCHED_H_

/*
 * The subset of FreeBSD's <sys/sched.h> that LinuxKPI uses.
 * Implementations are in lkpi_sched.cpp (pinning needs the C++-only
 * kernel <thread.h>) and lkpi_thread.c.
 *
 * sched_bind() cannot migrate a thread to a chosen CPU on Haiku; it
 * yields instead, and sched_is_bound() is accordingly never true.
 * linux_rcu.c only binds to speed grace periods up -
 * ck_epoch_synchronize_wait() does the actual waiting.
 */

struct thread;

void lkpi_sched_prio(struct thread* td, u_char prio);
void lkpi_sched_add(struct thread* td, int flags);
void lkpi_sched_pin(void);
void lkpi_sched_unpin(void);
void lkpi_sched_relinquish(struct thread* td);
void lkpi_sched_bind(struct thread* td, int cpu);
void lkpi_sched_unbind(struct thread* td);
void lkpi_mi_switch(int flags);

#define sched_prio(td, prio)	lkpi_sched_prio(td, prio)
#define sched_add(td, flags)	lkpi_sched_add(td, flags)
#define sched_pin()				lkpi_sched_pin()
#define sched_unpin()			lkpi_sched_unpin()
#define sched_relinquish(td)	lkpi_sched_relinquish(td)
#define sched_bind(td, cpu)		lkpi_sched_bind(td, cpu)
#define sched_unbind(td)		lkpi_sched_unbind(td)
#define sched_is_bound(td)		0

/* mi_switch() is entered with thread_lock(curthread) held and returns
 * with it dropped, like FreeBSD's. */
#define SW_VOL			0x0001
#define SWT_RELINQUISH	0x0002
#define mi_switch(flags)	lkpi_mi_switch(flags)


#endif /* _LKPI_HAIKU_SYS_SCHED_H_ */
