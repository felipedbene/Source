/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_PRELUDE_H_
#define _LKPI_HAIKU_PRELUDE_H_

/*
 * Force-included into every LinuxKPI translation unit, ahead of
 * <linux/kconfig.h> (see the Jamfile).
 *
 * Haiku's <SupportDefs.h> defines atomic_set(), atomic_add(), atomic_and(),
 * atomic_or() & co. as static inlines on int32*, while LinuxKPI's
 * <asm/atomic.h> defines Linux's atomic_set() and atomic_add() on atomic_t*.
 * Both cannot be visible under the same names in one translation unit.
 *
 * Include SupportDefs.h first, with Haiku's names rewritten to
 * __haiku_atomic_*. The shadow <machine/atomic.h> next to this file points
 * FreeBSD's atomic_*_int()/_32() macros at the renamed inlines, and the
 * Linux names are then free for <asm/atomic.h> to define.
 */

/*
 * Same story for umode_t: POSIX <sys/types.h> typedefs it as uint32,
 * LinuxKPI's <asm/types.h> as unsigned short. Nothing on the Haiku/FreeBSD
 * side of a LinuxKPI translation unit uses it.
 */
/*
 * freebsd_network's <sys/_types.h> falls back to char* varargs unless
 * this is set; make sure it is before any include order can reach it.
 */
#define __GNUCLIKE_BUILTIN_VARARGS 1

/*
 * On Haiku, gcc's <stddef.h> defines __size_t as an EMPTY macro (a BeOS
 * relic), which breaks FreeBSD's <sys/bitset.h>. Parse it now, then make
 * __size_t mean what FreeBSD expects.
 */
#include <stddef.h>
#undef __size_t
#define __size_t			__SIZE_TYPE__

#define umode_t				__haiku_umode_t

/*
 * And for struct tm (POSIX <time.h>, pulled in by <sys/types.h>):
 * LinuxKPI's <linux/time.h> defines its own, identical, struct tm.
 */
#define tm					__haiku_tm

/*
 * And for two timer functions whose names Linux also uses:
 * Haiku's add_timer() (KernelExport.h, via <kernel/lock.h> below) and
 * POSIX timer_delete() (<time.h>) vs. <linux/timer.h>'s.
 */
#define add_timer			__haiku_add_timer
#define timer_delete		__haiku_timer_delete

/*
 * And for the mutex TYPE: <linux/mutex.h> defines its own struct mutex.
 * Only the type is renamed - mutex_lock() & co. keep their names (they
 * are real kernel symbols; Linux's mutex_lock & co. are macros, defined
 * later, which do not disturb anything parsed here).
 */
#define mutex				__haiku_mutex

#define atomic_set			__haiku_atomic_set
#define atomic_get_and_set	__haiku_atomic_get_and_set
#define atomic_test_and_set	__haiku_atomic_test_and_set
#define atomic_add			__haiku_atomic_add
#define atomic_and			__haiku_atomic_and
#define atomic_or			__haiku_atomic_or
#define atomic_get			__haiku_atomic_get

#include <SupportDefs.h>

/*
 * Haiku headers whose inline functions call the atomics by their bare names
 * must be parsed while the renames are still active. Only lock.h is known
 * to; it is reachable from every freebsd_network sys/ header.
 */
#include <kernel/lock.h>

/*
 * freebsd_network's mtx is built on the (just renamed) Haiku mutex, and
 * its inline mtx_lock() & co. call mutex_lock() & co. by name. They must
 * be parsed before <linux/mutex.h> can define macros with those names, so
 * parse them here, in every LinuxKPI translation unit.
 */
#include <sys/mutex.h>

#undef umode_t
#undef tm
#undef add_timer
#undef timer_delete
#undef mutex
#undef atomic_set
#undef atomic_get_and_set
#undef atomic_test_and_set
#undef atomic_add
#undef atomic_and
#undef atomic_or
#undef atomic_get


#endif /* _LKPI_HAIKU_PRELUDE_H_ */
