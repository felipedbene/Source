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


int
pause_sbt(const char* wmesg __unused, sbintime_t sbt, sbintime_t pr __unused,
	int flags)
{
	uint32 snoozeFlags = B_RELATIVE_TIMEOUT;
	status_t status;

	if ((flags & C_CATCH) != 0)
		snoozeFlags |= B_CAN_INTERRUPT;

	do {
		status = snooze_etc(sbttous(sbt), B_SYSTEM_TIMEBASE, snoozeFlags);
		/* an uninterruptible pause must run its full time */
	} while (status == B_INTERRUPTED && (flags & C_CATCH) == 0);

	if (status == B_INTERRUPTED)
		return EINTR;
	return EWOULDBLOCK;
}
