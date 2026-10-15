/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Extends freebsd_network's <sys/systm.h> with what FreeBSD gets there from
 * <sys/kassert.h>.
 */

#include_next <sys/systm.h>

#ifndef _LKPI_HAIKU_SYS_SYSTM_H_
#define _LKPI_HAIKU_SYS_SYSTM_H_


/* FreeBSD skips lock operations once the scheduler is stopped (panic) or
 * the kernel debugger holds the system; drivers see neither on Haiku. */
#define SCHEDULER_STOPPED()	0
#define kdb_active			0

/* Declared so <linux/uaccess.h>'s inlines compile; implemented when
 * something on the rtw88 path turns out to use them (nothing does). */
int copyin_nofault(const void* uaddr, void* kaddr, size_t len);
int copyout_nofault(const void* kaddr, void* uaddr, size_t len);

#ifndef __assert_unreachable
#	define __assert_unreachable() \
		do { \
			panic("executing segment marked as unreachable at %s:%d (%s)\n", \
				__FILE__, __LINE__, __func__); \
			__builtin_unreachable(); \
		} while (0)
#endif


#endif /* _LKPI_HAIKU_SYS_SYSTM_H_ */
