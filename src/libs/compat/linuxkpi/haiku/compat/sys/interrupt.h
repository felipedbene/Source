/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_INTERRUPT_H_
#define _LKPI_HAIKU_SYS_INTERRUPT_H_

/*
 * The subset of FreeBSD's <sys/interrupt.h> LinuxKPI uses: the software
 * interrupt numbers, and PI_SWI() giving the Haiku priority of a software
 * interrupt thread. PI_SWI(SWI_NET) equals freebsd_network's PI_NET
 * (sys/priority.h); lower-numbered SWIs are more urgent, so they get a
 * higher Haiku priority.
 */

#include <sys/priority.h>


#define SWI_TTY		0
#define SWI_NET		1
#define SWI_CAMBIO	2
#define SWI_BUSDMA	3
#define SWI_CLOCK	4
#define SWI_TQ_FAST	5
#define SWI_TQ		6
#define SWI_TQ_GIANT	6

#define PI_SWI(x)	(B_REAL_TIME_DISPLAY_PRIORITY - 8 - (x))

/* IRQ affinity is a hint; the implementation (lkpi_machdep.c) reports
 * ENOSYS, like Linux without affinity support. */
int intr_setaffinity(int irq, int mode, void* mask);


#endif /* _LKPI_HAIKU_SYS_INTERRUPT_H_ */
