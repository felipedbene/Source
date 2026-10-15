/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_TIME_H_
#define _LKPI_HAIKU_SYS_TIME_H_

/*
 * Extends freebsd_network's <sys/time.h> with FreeBSD's timespec
 * arithmetic macros, which LinuxKPI's <linux/time.h> uses.
 */

#include_next <sys/time.h>


#ifndef timespecclear
#define timespecclear(tsp)		((tsp)->tv_sec = (tsp)->tv_nsec = 0)
#define timespecisset(tsp)		((tsp)->tv_sec || (tsp)->tv_nsec)
#define timespeccmp(tsp, usp, cmp)					\
	(((tsp)->tv_sec == (usp)->tv_sec) ?				\
	    ((tsp)->tv_nsec cmp (usp)->tv_nsec) :		\
	    ((tsp)->tv_sec cmp (usp)->tv_sec))

#define timespecadd(tsp, usp, vsp)					\
	do {											\
		(vsp)->tv_sec = (tsp)->tv_sec + (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec + (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec >= 1000000000L) {		\
			(vsp)->tv_sec++;						\
			(vsp)->tv_nsec -= 1000000000L;			\
		}											\
	} while (0)
#define timespecsub(tsp, usp, vsp)					\
	do {											\
		(vsp)->tv_sec = (tsp)->tv_sec - (usp)->tv_sec;		\
		(vsp)->tv_nsec = (tsp)->tv_nsec - (usp)->tv_nsec;	\
		if ((vsp)->tv_nsec < 0) {					\
			(vsp)->tv_sec--;						\
			(vsp)->tv_nsec += 1000000000L;			\
		}											\
	} while (0)
#endif

/* FreeBSD timecounter calls LinuxKPI's <linux/ktime.h> uses; the get*
 * variants may be less precise there, here they are the same
 * (lkpi_time.c). */
void nanouptime(struct timespec* ts);
void nanotime(struct timespec* ts);
#define getnanouptime(ts)	nanouptime(ts)
#define getnanotime(ts)		nanotime(ts)


#endif /* _LKPI_HAIKU_SYS_TIME_H_ */
