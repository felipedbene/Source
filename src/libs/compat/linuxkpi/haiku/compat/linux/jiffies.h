/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_LINUX_JIFFIES_H_
#define _LKPI_HAIKU_LINUX_JIFFIES_H_

/*
 * The one shadow over a LinuxKPI header: on FreeBSD, jiffies is a real
 * global (an assembly alias of the 64-bit ticksl); freebsd_network's
 * ticksl is a function-call macro, which no variable can alias without
 * going stale. Parse the real header (its extern declaration stays
 * unreferenced), then turn every later use of jiffies into a live ticksl
 * read. Not the int32 ticks: it would sign-extend at its roll.
 */

#include_next <linux/jiffies.h>

#undef jiffies
#define jiffies		((unsigned long)ticksl)


#endif /* _LKPI_HAIKU_LINUX_JIFFIES_H_ */
