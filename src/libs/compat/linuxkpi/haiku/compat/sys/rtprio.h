/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_RTPRIO_H_
#define _LKPI_HAIKU_SYS_RTPRIO_H_

/*
 * The subset of FreeBSD's <sys/rtprio.h> that LinuxKPI's <linux/sched.h>
 * uses (sched_set_fifo(), sched_set_fifo_low()). Types and constants have
 * FreeBSD's values; rtp_to_pri() translates to a Haiku priority and applies
 * it (lkpi_thread.c).
 */

#include <sys/types.h>


/* priority types; a realtime type may have RTP_PRIO_FIFO_BIT or'ed in */
#define RTP_PRIO_REALTIME		2
#define RTP_PRIO_NORMAL			3
#define RTP_PRIO_IDLE			4

#define RTP_PRIO_FIFO_BIT		8
#define RTP_PRIO_FIFO			(RTP_PRIO_FIFO_BIT | RTP_PRIO_REALTIME)
#define RTP_PRIO_BASE(P)		((P) & ~RTP_PRIO_FIFO_BIT)
#define RTP_PRIO_IS_REALTIME(P)	(RTP_PRIO_BASE(P) == RTP_PRIO_REALTIME)

/* priority range inside a type; 0 is the highest priority */
#define RTP_PRIO_MIN			0
#define RTP_PRIO_MAX			31

struct rtprio {
	u_short	type;
	u_short	prio;
};

struct thread;

int rtp_to_pri(struct rtprio* rtp, struct thread* td);


#endif /* _LKPI_HAIKU_SYS_RTPRIO_H_ */
