/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SIGNALVAR_H_
#define _LKPI_HAIKU_SYS_SIGNALVAR_H_

/*
 * The subset of FreeBSD's <sys/signalvar.h> that linux_schedule.c's
 * signal_pending emulation uses, on Haiku's flat uint64 sigset_t.
 */

#include <signal.h>


#define SIGSETOR(set1, set2)	((set1) |= (set2))
#define SIGSETNAND(set1, set2)	((set1) &= ~(set2))
#define SIGISEMPTY(set)			((set) == 0)
#define SIGISMEMBER(set, signo) \
	(((set) & (((sigset_t)1) << ((signo) - 1))) != 0)
#define SIGADDSET(set, signo) \
	((set) |= (((sigset_t)1) << ((signo) - 1)))

struct thread;

/* Records the signal on the shadow thread (lkpi_thread.c). */
void tdsignal(struct thread* td, int signo);


#endif /* _LKPI_HAIKU_SYS_SIGNALVAR_H_ */
