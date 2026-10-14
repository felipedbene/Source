/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/* Extends freebsd_network's <sys/cdefs.h> with what LinuxKPI and pctrie need. */

#include_next <sys/cdefs.h>

#ifndef _LKPI_HAIKU_SYS_CDEFS_H_
#define _LKPI_HAIKU_SYS_CDEFS_H_


#ifndef __always_inline
#	define __always_inline	__inline __attribute__((__always_inline__))
#endif

#ifndef __unreachable
#	define __unreachable()	__builtin_unreachable()
#endif


#endif /* _LKPI_HAIKU_SYS_CDEFS_H_ */
