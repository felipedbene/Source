/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_UMA_H_
#define _LKPI_HAIKU_VM_UMA_H_

/*
 * The subset of FreeBSD's UMA zone allocator that LinuxKPI uses, implemented
 * on top of Haiku's object caches (see uma.cpp).
 */

#include <sys/param.h>
#include <sys/malloc.h>
#include <vm/uma_align_mask.h>


struct uma_zone;
typedef struct uma_zone* uma_zone_t;

typedef int (*uma_ctor)(void* mem, int size, void* arg, int flags);
typedef void (*uma_dtor)(void* mem, int size, void* arg);
typedef int (*uma_init)(void* mem, int size, int flags);
typedef void (*uma_fini)(void* mem, int size);
typedef int (*uma_import)(void* arg, void** store, int count, int domain,
	int flags);
typedef void (*uma_release)(void* arg, void** store, int count);

#define UMA_ZONE_ZINIT		0x0002	/* Initialize with zeros */

#define UMA_ALIGN_PTR	(sizeof(void*) - 1)
#define UMA_ALIGN_CACHE	(uma_get_cache_align_mask())
#define UMA_ALIGN_CACHE_AND_MASK(mask) (uma_get_cache_align_mask() | (mask))

#define UMA_ANYDOMAIN	-1


__BEGIN_DECLS

uma_zone_t uma_zcreate(const char* name, size_t size, uma_ctor ctor,
	uma_dtor dtor, uma_init uminit, uma_fini fini, int align, uint32_t flags);
uma_zone_t uma_zcache_create(const char* name, int size, uma_ctor ctor,
	uma_dtor dtor, uma_init zinit, uma_fini zfini, uma_import zimport,
	uma_release zrelease, void* arg, int flags);
void uma_zdestroy(uma_zone_t zone);

void* uma_zalloc_arg(uma_zone_t zone, void* arg, int flags);
void uma_zfree_arg(uma_zone_t zone, void* item, void* arg);

void uma_zone_reserve(uma_zone_t zone, int nitems);
void uma_prealloc(uma_zone_t zone, int itemcnt);

__END_DECLS


static __inline void*
uma_zalloc(uma_zone_t zone, int flags)
{
	return uma_zalloc_arg(zone, NULL, flags);
}


static __inline void
uma_zfree(uma_zone_t zone, void* item)
{
	uma_zfree_arg(zone, item, NULL);
}


#endif /* _LKPI_HAIKU_VM_UMA_H_ */
