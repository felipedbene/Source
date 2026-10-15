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

#define bitcount32(x)	((unsigned)__builtin_popcount((uint32_t)(x)))
#define bitcount64(x)	((unsigned)__builtin_popcountll((uint64_t)(x)))
#define bitcountl(x)	((unsigned)__builtin_popcountl((u_long)(x)))

static __inline u_long
ulmin(u_long a, u_long b)
{
	return (a < b ? a : b);
}


static __inline u_long
ulmax(u_long a, u_long b)
{
	return (a > b ? a : b);
}


static __inline char*
strchrnul(const char* p, int ch)
{
	while (*p != '\0' && *p != (char)ch)
		p++;
	return (char*)p;
}


/* The kernel exports these (src/system/kernel/lib) but no kernel header
 * declares them for C. */
long strtol(const char* cp, char** endp, int base);
unsigned long strtoul(const char* cp, char** endp, int base);
long long strtoll(const char* cp, char** endp, int base);
unsigned long long strtoull(const char* cp, char** endp, int base);

#define strtouq(cp, endp, base)	strtoull(cp, endp, base)
#define strtoq(cp, endp, base)	strtoll(cp, endp, base)


#endif /* _LKPI_HAIKU_SYS_LIBKERN_H_ */
