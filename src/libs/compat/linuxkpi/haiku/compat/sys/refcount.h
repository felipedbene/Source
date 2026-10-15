/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_REFCOUNT_H_
#define _LKPI_HAIKU_SYS_REFCOUNT_H_

/*
 * The subset of FreeBSD's <sys/refcount.h> that LinuxKPI's <linux/kref.h>
 * and <linux/refcount.h> use, on the compiler atomics. Sequentially
 * consistent throughout, which satisfies FreeBSD's weaker ordering.
 */

#include <sys/types.h>


static inline void
refcount_init(volatile u_int* count, u_int value)
{
	__atomic_store_n(count, value, __ATOMIC_SEQ_CST);
}


static inline u_int
refcount_load(volatile u_int* count)
{
	return __atomic_load_n(count, __ATOMIC_SEQ_CST);
}


static inline u_int
refcount_acquire(volatile u_int* count)
{
	return __atomic_fetch_add(count, 1, __ATOMIC_SEQ_CST);
}


static inline bool
refcount_acquire_if_not_zero(volatile u_int* count)
{
	u_int old = refcount_load(count);
	while (old != 0) {
		if (__atomic_compare_exchange_n(count, &old, old + 1, 1,
				__ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
			return true;
	}
	return false;
}


/* Returns true when this call dropped the count to zero. */
static inline bool
refcount_release(volatile u_int* count)
{
	return __atomic_fetch_sub(count, 1, __ATOMIC_SEQ_CST) == 1;
}


static inline bool
refcount_release_if_not_last(volatile u_int* count)
{
	u_int old = refcount_load(count);
	while (old > 1) {
		if (__atomic_compare_exchange_n(count, &old, old - 1, 1,
				__ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
			return true;
	}
	return false;
}


static inline bool
refcount_release_if_last(volatile u_int* count)
{
	u_int old = refcount_load(count);
	while (old == 1) {
		if (__atomic_compare_exchange_n(count, &old, 0, 1,
				__ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
			return true;
	}
	return false;
}


#endif /* _LKPI_HAIKU_SYS_REFCOUNT_H_ */
