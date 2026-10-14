/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SX_H_
#define _LKPI_HAIKU_SYS_SX_H_

/*
 * Shadows freebsd_network's <sys/sx.h> for LinuxKPI code only.
 *
 * LinuxKPI builds Linux mutexes and rw_semaphores on sx, and needs more than
 * freebsd_network's rw_lock wrapper offers: try-locks, downgrade, recursion,
 * interruptible waits, and an owner word it reads directly via SX_OWNER() and
 * compares against curthread. This sx is built on freebsd_network's mtx and cv
 * (see lkpi_sx.c).
 *
 * sx_lock holds the owning shadow thread while exclusively locked and is 0
 * otherwise, so SX_OWNER(sx_lock) yields the owner or NULL like on FreeBSD.
 */

#include <sys/param.h>
#include <sys/mutex.h>
#include <sys/condvar.h>


struct sx {
	volatile uintptr_t	sx_lock;		/* owning struct thread*, or 0 */
	struct mtx			sx_mtx;			/* protects all fields below */
	struct cv			sx_cv;
	int					sx_flags;
	int					sx_recurse;		/* exclusive recursion depth - 1 */
	int					sx_readers;
	int					sx_waiters;
	int					sx_xwaiters;	/* exclusive waiters; block new readers */
};

#define SX_LOCK_FLAGMASK	((uintptr_t)0)
#define SX_OWNER(x)			((x) & ~SX_LOCK_FLAGMASK)

/* sx_init_flags() flags */
#define SX_DUPOK		0x01
#define SX_NOPROFILE	0x02
#define SX_NOWITNESS	0x04
#define SX_QUIET		0x08
#define SX_NOADAPTIVE	0x10
#define SX_RECURSE		0x20
#define SX_NEW			0x40

/* sx_assert() flags, accepted but not checked */
#define SA_LOCKED		0x01
#define SA_SLOCKED		0x02
#define SA_XLOCKED		0x04
#define SA_UNLOCKED		0x08
#define SA_RECURSED		0x10
#define SA_NOTRECURSED	0x20


__BEGIN_DECLS

void sx_init_flags(struct sx* sx, const char* description, int opts);
void sx_destroy(struct sx* sx);

int lkpi_sx_xlock(struct sx* sx, bool interruptible);
int lkpi_sx_slock(struct sx* sx, bool interruptible);
int sx_try_xlock(struct sx* sx);
int sx_try_slock(struct sx* sx);
void sx_xunlock(struct sx* sx);
void sx_sunlock(struct sx* sx);
void sx_unlock(struct sx* sx);
void sx_downgrade(struct sx* sx);
int sx_has_waiters(struct sx* sx);

__END_DECLS


#define sx_init(sx, desc)	sx_init_flags((sx), (desc), 0)

#define sx_xlock(sx)		((void)lkpi_sx_xlock((sx), false))
#define sx_xlock_sig(sx)	lkpi_sx_xlock((sx), true)
#define sx_slock(sx)		((void)lkpi_sx_slock((sx), false))
#define sx_slock_sig(sx)	lkpi_sx_slock((sx), true)

#define sx_xholder(sx)		((struct thread*)SX_OWNER((sx)->sx_lock))
#define sx_xlocked(sx)		(sx_xholder(sx) == curthread)

#define sx_assert(sx, what)	do { } while (0)


#endif /* _LKPI_HAIKU_SYS_SX_H_ */
