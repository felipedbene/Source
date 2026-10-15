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

/*
 * FreeBSD's 32.32 fixed-point sbintime_t, as far as LinuxKPI's sleep and
 * callout paths use it.
 */
typedef __INT64_TYPE__ sbintime_t;

#define SBT_1S	((sbintime_t)1 << 32)
#define SBT_1MS	(SBT_1S / 1000)
#define SBT_1US	(SBT_1S / 1000000)
#define SBT_1NS	(SBT_1S / 1000000000)
#define SBT_MAX	0x7fffffffffffffffLL

static inline sbintime_t
mstosbt(__INT64_TYPE__ ms)
{
	return ms * SBT_1MS;
}

static inline sbintime_t
ustosbt(__INT64_TYPE__ us)
{
	return us * SBT_1US;
}

static inline sbintime_t
nstosbt(__INT64_TYPE__ ns)
{
	return (sbintime_t)(((__int128)ns * SBT_1S) / 1000000000);
}

static inline __INT64_TYPE__
sbttous(sbintime_t sbt)
{
	return (__INT64_TYPE__)(((__int128)sbt * 1000000) >> 32);
}

static inline __INT64_TYPE__
sbttons(sbintime_t sbt)
{
	return (__INT64_TYPE__)(((__int128)sbt * 1000000000) >> 32);
}

/* FreeBSD <sys/callout.h> precision/behavior flags (interpreted by this
 * port's pause_sbt and callout shims; values match FreeBSD's). */
#ifndef C_HARDCLOCK
#	define C_DIRECT_EXEC	0x0001
#	define C_HARDCLOCK		0x0100
#	define C_ABSOLUTE		0x0200
#	define C_PRECALC		0x0400
#	define C_CATCH			0x0800
#endif

/* Sleeps for sbt; EWOULDBLOCK when the time elapsed, EINTR when a signal
 * ended it early (C_CATCH only). Implemented in lkpi_time.c. */
int pause_sbt(const char* wmesg, sbintime_t sbt, sbintime_t pr, int flags);

/* ticks are milliseconds in this compat layer (hz = 1000) */
#define pause(wmesg, timo)	pause_sbt((wmesg), mstosbt(timo), 0, C_HARDCLOCK)


#endif /* _LKPI_HAIKU_SYS_TIME_H_ */
