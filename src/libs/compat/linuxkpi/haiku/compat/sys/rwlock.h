/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_RWLOCK_H_
#define _LKPI_HAIKU_SYS_RWLOCK_H_

/*
 * FreeBSD rwlock(9), as far as LinuxKPI uses it (plain read/write locking, no
 * try-locks or upgrades), on Haiku's rw_lock.
 */

#include <lock.h>


struct rwlock {
	rw_lock l;
};

#define RW_DUPOK		0x01
#define RW_NOPROFILE	0x02
#define RW_NOWITNESS	0x04
#define RW_QUIET		0x08
#define RW_RECURSE		0x10
#define RW_NEW			0x20

#define rw_init(rw, name)			rw_lock_init(&(rw)->l, name)
#define rw_init_flags(rw, name, opts)	rw_lock_init(&(rw)->l, name)
#define rw_destroy(rw)				rw_lock_destroy(&(rw)->l)
#define rw_rlock(rw)				rw_lock_read_lock(&(rw)->l)
#define rw_runlock(rw)				rw_lock_read_unlock(&(rw)->l)
#define rw_wlock(rw)				rw_lock_write_lock(&(rw)->l)
#define rw_wunlock(rw)				rw_lock_write_unlock(&(rw)->l)

#define rw_assert(rw, what)


#endif /* _LKPI_HAIKU_SYS_RWLOCK_H_ */
