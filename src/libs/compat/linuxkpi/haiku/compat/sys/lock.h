/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_LOCK_H_
#define _LKPI_HAIKU_SYS_LOCK_H_

/*
 * Extends freebsd_network's <sys/lock.h> (MPASS) with enough of FreeBSD's
 * lock-class machinery for <linux/lockdep.h> to compile under INVARIANTS.
 *
 * Haiku's mtx and this port's sx do not start with a struct lock_object,
 * so real lock classes cannot exist here. LOCK_CLASS() returns one dummy
 * class whose lc_assert() does nothing (like freebsd_network's
 * mtx_assert/sx_assert) and whose lc_owner() reports "held", making
 * lockdep_is_held() always true, exactly like LinuxKPI without
 * INVARIANTS.
 */

#include_next <sys/lock.h>


struct lock_object;
struct thread;

struct lock_class {
	void (*lc_assert)(const struct lock_object* lock, int what);
	int (*lc_owner)(const struct lock_object* lock, struct thread** owner);
};

#define LA_UNLOCKED		0x00
#define LA_LOCKED		0x01
#define LA_SLOCKED		0x02
#define LA_XLOCKED		0x04
#define LA_RECURSED		0x08
#define LA_NOTRECURSED	0x10

extern struct lock_class lkpi_lock_class_noop;

#define LOCK_CLASS(lock)	(&lkpi_lock_class_noop)


#endif /* _LKPI_HAIKU_SYS_LOCK_H_ */
