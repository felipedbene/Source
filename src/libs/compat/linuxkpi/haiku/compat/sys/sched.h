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
 * kernel <thread.h>).
 */

struct thread;

void lkpi_sched_pin(void);
void lkpi_sched_unpin(void);
void lkpi_sched_relinquish(struct thread* td);

#define sched_pin()				lkpi_sched_pin()
#define sched_unpin()			lkpi_sched_unpin()
#define sched_relinquish(td)	lkpi_sched_relinquish(td)


#endif /* _LKPI_HAIKU_SYS_SCHED_H_ */
