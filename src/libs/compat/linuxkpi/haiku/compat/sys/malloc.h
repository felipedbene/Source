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
 * Allocations in LinuxKPI translation units are size-tracked
 * (lkpi_malloc.c): linux/slab.h needs malloc_usable_size()/zfree(), and
 * kfree() of a >PAGE_SIZE kmalloc() must go back to contigfree, which
 * Haiku's heap free cannot do on its own.
 */
void* lkpi_malloc_tracked(size_t size, int flags);
void* lkpi_contigmalloc_tracked(size_t size, int flags, vm_paddr_t low,
	vm_paddr_t high, unsigned long alignment, unsigned long boundary);
void lkpi_free_tracked(void* ptr);
void lkpi_contigfree_tracked(void* ptr, unsigned long size);
void* lkpi_realloc(void* ptr, size_t size, int flags);
size_t malloc_usable_size(const void* ptr);
void lkpi_zfree(void* ptr);

#undef malloc
#undef free
#undef contigmalloc
#undef contigfree
#define malloc(size, tag, flags)	lkpi_malloc_tracked(size, flags)
#define free(ptr, tag)				lkpi_free_tracked(ptr)
#define contigmalloc(size, type, flags, low, high, alignment, boundary) \
	lkpi_contigmalloc_tracked(size, flags, low, high, alignment, boundary)
#define contigmalloc_domainset(size, type, ds, flags, low, high, alignment, \
			boundary) \
	lkpi_contigmalloc_tracked(size, flags, low, high, alignment, boundary)
#define contigfree(addr, size, type)	lkpi_contigfree_tracked(addr, size)
#define realloc(ptr, size, type, flags)	lkpi_realloc(ptr, size, flags)
#define zfree(addr, type)				lkpi_zfree(addr)


#endif /* _LKPI_HAIKU_SYS_MALLOC_H_ */
