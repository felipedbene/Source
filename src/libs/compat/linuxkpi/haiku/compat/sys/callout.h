/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_CALLOUT_H_
#define _LKPI_HAIKU_SYS_CALLOUT_H_

/*
 * Extends freebsd_network's <sys/callout.h> with the sbintime_t variants
 * LinuxKPI's hrtimers use, converted onto the tick API (ticks are
 * milliseconds here; sub-millisecond timers round up to 1 ms - hrtimers
 * are only "high resolution" to that extent on this port).
 */

#include_next <sys/callout.h>

#include <limits.h>
#include <sys/time.h>


static inline int
lkpi_sbt_to_ticks(sbintime_t sbt)
{
	__INT64_TYPE__ ms = sbttous(sbt) / 1000;
	if (ms < 1)
		ms = 1;
	if (ms > INT_MAX)
		ms = INT_MAX;
	return (int)ms;
}

#define callout_reset_sbt(c, sbt, pr, fn, arg, flags) \
	callout_reset((c), lkpi_sbt_to_ticks(sbt), (fn), (arg))
#define callout_schedule_sbt(c, sbt, pr, flags) \
	callout_schedule((c), lkpi_sbt_to_ticks(sbt))

/* freebsd_network's callout_active() means "handler running right now"
 * and clears by itself when the handler returns. */
#define callout_deactivate(c)	((void)(c))


#endif /* _LKPI_HAIKU_SYS_CALLOUT_H_ */
