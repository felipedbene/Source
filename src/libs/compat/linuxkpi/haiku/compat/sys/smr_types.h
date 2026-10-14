/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SMR_TYPES_H_
#define _LKPI_HAIKU_SYS_SMR_TYPES_H_

/*
 * SMR-protected pointer accessors, with the same memory ordering as FreeBSD's
 * but without SMR itself (see <sys/_smr.h>).
 */

#include <sys/_smr.h>


#define SMR_POINTER(type)						\
struct {										\
	type	__ptr;		/* Do not access directly */	\
}

#define smr_entered_load(p, smr)					\
	__atomic_load_n(&(p)->__ptr, __ATOMIC_ACQUIRE)

#define smr_serialized_load(p, ex)					\
	__atomic_load_n(&(p)->__ptr, __ATOMIC_RELAXED)

#define smr_serialized_store(p, v, ex) do {			\
	__typeof((p)->__ptr) _v = (v);					\
	__atomic_store_n(&(p)->__ptr, _v, __ATOMIC_RELEASE);	\
} while (0)

#define smr_serialized_swap(p, v, ex)				\
	__atomic_exchange_n(&(p)->__ptr, (v), __ATOMIC_ACQ_REL)

#define smr_unserialized_load(p, ex)				\
	__atomic_load_n(&(p)->__ptr, __ATOMIC_RELAXED)

#define smr_unserialized_store(p, v, ex) do {		\
	__typeof((p)->__ptr) _v = (v);					\
	__atomic_store_n(&(p)->__ptr, _v, __ATOMIC_RELAXED);	\
} while (0)


#endif /* _LKPI_HAIKU_SYS_SMR_TYPES_H_ */
