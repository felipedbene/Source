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

#ifndef __result_use_check
#	define __result_use_check	__attribute__((__warn_unused_result__))
#endif

/* FreeBSD's base cdefs has this Linux-style alias; LinuxKPI's
 * <linux/cleanup.h> uses it directly. */
#ifndef __maybe_unused
#	define __maybe_unused	__attribute__((__unused__))
#endif

/* FreeBSD: "used only in diagnostics"; without INVARIANTS/WITNESS the
 * uses compile away, so the variable is unused. */
#ifndef __diagused
#	define __diagused	__unused
#endif

/* FreeBSD's <machine/_types.h> types that Haiku has no equivalent of;
 * <sys/tree.h> uses them. */
typedef __UINTPTR_TYPE__	__uintptr_t;
typedef __INTPTR_TYPE__		__intptr_t;

/* POSIX's __CONCAT pastes without expanding its arguments; FreeBSD's
 * expands them first, and <linux/cleanup.h> depends on that. */
#undef __CONCAT
#define __CONCAT1(x, y)	x ## y
#define __CONCAT(x, y)	__CONCAT1(x, y)


#endif /* _LKPI_HAIKU_SYS_CDEFS_H_ */
