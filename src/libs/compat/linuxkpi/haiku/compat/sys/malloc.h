/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_MALLOC_H_
#define _LKPI_HAIKU_SYS_MALLOC_H_

/*
 * Extends freebsd_network's <sys/malloc.h> with the FreeBSD malloc API
 * LinuxKPI's <linux/slab.h> and <linux/gfp.h> use. Flag values must stay
 * disjoint from freebsd_network's M_NOWAIT/M_WAITOK/M_ZERO.
 */

#include_next <sys/malloc.h>

#include <sys/stdint.h>


#ifndef M_USE_RESERVE
#	define M_USE_RESERVE	0x0400	/* can alloc out of reserve memory */
#endif

#define WOULD_OVERFLOW(nmemb, size) \
	((size) != 0 && (nmemb) > SIZE_MAX / (size))

/* No NUMA domains on Haiku. */
#define malloc_domainset(size, type, policy, flags) \
	malloc(size, type, flags)

/* What malloc() rounds an allocation up to; at-least-identity is valid. */
#define malloc_size(size)	(size)

/*
 * Declared so <linux/slab.h>'s inlines compile; implemented when
 * linux_slab.c is brought up (the Haiku kernel heap has no usable-size
 * query, so these need LinuxKPI-side allocation tracking).
 */
size_t malloc_usable_size(const void* ptr);
void lkpi_zfree(void* ptr);
#define zfree(addr, type)	lkpi_zfree(addr)


#endif /* _LKPI_HAIKU_SYS_MALLOC_H_ */
