/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Extends freebsd_network's <sys/libkern.h> with FreeBSD's ilog2() family.
 * LinuxKPI's <linux/log2.h> relies on these rather than defining its own.
 */

#include_next <sys/libkern.h>

#ifndef _LKPI_HAIKU_SYS_LIBKERN_H_
#define _LKPI_HAIKU_SYS_LIBKERN_H_


static __inline __pure2 int
ilog2_int(int n)
{
	return (8 * sizeof(n) - 1 - __builtin_clz((u_int)n));
}


static __inline __pure2 int
ilog2_long(long n)
{
	return (8 * sizeof(n) - 1 - __builtin_clzl((u_long)n));
}


static __inline __pure2 int
ilog2_long_long(long long n)
{
	return (8 * sizeof(n) - 1 - __builtin_clzll((unsigned long long)n));
}


#define ilog2_var(n)				\
	_Generic((n),					\
		default: ilog2_int,			\
		long: ilog2_long,			\
		unsigned long: ilog2_long,	\
		long long: ilog2_long_long,	\
		unsigned long long: ilog2_long_long	\
	)(n)

#define ilog2_const(n)				\
	(8 * (int)sizeof(unsigned long long) - 1 -	\
	__builtin_clzll(n))

#define ilog2(n) (__builtin_constant_p(n) ? ilog2_const(n) : ilog2_var(n))
#define rounddown_pow_of_two(n)	((__typeof(n))1 << ilog2(n))
#define order_base_2(n)			ilog2(2*(n)-1)
#define roundup_pow_of_two(n)	((__typeof(n))1 << order_base_2(n))


#endif /* _LKPI_HAIKU_SYS_LIBKERN_H_ */
