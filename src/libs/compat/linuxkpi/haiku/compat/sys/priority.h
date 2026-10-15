/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_PRIORITY_H_
#define _LKPI_HAIKU_SYS_PRIORITY_H_

/*
 * Extends freebsd_network's <sys/priority.h> (Haiku-valued PI_*) with
 * names LinuxKPI passes to calls that ignore them here.
 */

#include_next <sys/priority.h>

#ifndef PRI_USER
#	define PRI_USER		B_NORMAL_PRIORITY	/* only fed to kern_yield() */
#endif

#endif /* _LKPI_HAIKU_SYS_PRIORITY_H_ */
