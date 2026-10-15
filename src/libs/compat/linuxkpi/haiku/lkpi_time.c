/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * FreeBSD timecounter calls LinuxKPI's <linux/ktime.h> uses.
 */

#include <sys/param.h>
#include <sys/time.h>

#include <OS.h>


void
nanouptime(struct timespec* ts)
{
	bigtime_t nsecs = system_time_nsecs();
	ts->tv_sec = nsecs / 1000000000LL;
	ts->tv_nsec = nsecs % 1000000000LL;
}


void
nanotime(struct timespec* ts)
{
	bigtime_t usecs = real_time_clock_usecs();
	ts->tv_sec = usecs / 1000000LL;
	ts->tv_nsec = (usecs % 1000000LL) * 1000;
}
