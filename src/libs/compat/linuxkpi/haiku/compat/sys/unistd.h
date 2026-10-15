/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_UNISTD_H_
#define _LKPI_HAIKU_SYS_UNISTD_H_

/*
 * The rfork()/kthread_add() flags from FreeBSD's <sys/unistd.h> that
 * LinuxKPI uses, with FreeBSD's values (lkpi_thread.c's fallback
 * definition of RFSTOPPED must stay in sync).
 */

#define RFPROC		(1 << 4)	/* change child (else changes curproc) */
#define RFMEM		(1 << 5)	/* share address space */
#define RFNOWAIT	(1 << 6)	/* give child to init */
#define RFSTOPPED	(1 << 17)	/* leave child in a stopped state */
#define RFHIGHPID	(1 << 18)	/* use a pid higher than 10 (idleproc) */


#endif /* _LKPI_HAIKU_SYS_UNISTD_H_ */
