/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * UMA zones on top of Haiku object caches.
 *
 * UMA's init/fini run when an item enters/leaves the backing store, which is
 * what object cache constructors/destructors do. UMA's ctor/dtor run on every
 * allocation/free, so we call those ourselves.
 *
 * Cache zones (uma_zcache_create) have no backing store of their own: items
 * come from the zone's import function, one at a time. That is slower than
 * UMA's per-CPU buckets but has the same semantics.
 */

#include <sys/param.h>
#include <sys/malloc.h>
#include <vm/uma.h>

#include <string.h>

#include <slab/Slab.h>


struct uma_zone {
	object_cache*	cache;		/* NULL for cache zones */
	size_t			size;		/* 0 if unknown (cache zones may pass -1) */
	uma_ctor		ctor;
	uma_dtor		dtor;
	uma_init		init;
	uma_fini		fini;
	uma_import		import;
	uma_release		release;
	void*			arg;
	uint32_t		flags;
};


static inline uint32
uma_to_oc_flags(int how)
{
	if (how & M_NOWAIT)
		return CACHE_DONT_WAIT_FOR_MEMORY;

	return 0;
}


/*
 * UMA's init-once contract does not survive Haiku's object cache: the slab
 * stores its free-list link inside free objects, and the KDEBUG_LEVEL 2
 * paranoia fills overwrite the whole object on both free (0xdeadbeef) and
 * alloc (0xcccccccc) - any constructed state is destroyed before the object
 * reaches the caller (this scribbled pctrie nodes, crashes #6/#7). So init
 * runs on every allocation and fini on every free instead; init functions
 * are idempotent, so this is correct, just slightly more work per alloc.
 */
static int
zone_item_init(struct uma_zone* zone, void* item, int flags)
{
	if ((zone->flags & UMA_ZONE_ZINIT) != 0)
		memset(item, 0, zone->size);

	if (zone->init != NULL && zone->init(item, zone->size, flags) != 0)
		return ENOMEM;

	return 0;
}


static struct uma_zone*
zone_alloc(void)
{
	return _kernel_malloc(sizeof(struct uma_zone), M_WAITOK | M_ZERO);
}


uma_zone_t
uma_zcreate(const char* name, size_t size, uma_ctor ctor, uma_dtor dtor,
	uma_init uminit, uma_fini fini, int align, uint32_t flags)
{
	struct uma_zone* zone = zone_alloc();

	if (zone == NULL)
		return NULL;

	zone->size = size;
	zone->ctor = ctor;
	zone->dtor = dtor;
	zone->init = uminit;
	zone->fini = fini;
	zone->flags = flags;

	/* UMA alignment is a mask, object cache alignment is a byte count.
	 * No cache-level constructor/destructor: the object cache cannot
	 * preserve constructed state (see zone_item_init() above). */
	zone->cache = create_object_cache_etc(name, size, (size_t)align + 1, 0, 0,
		0, 0, NULL, NULL, NULL, NULL);
	if (zone->cache == NULL) {
		_kernel_free(zone);
		return NULL;
	}

	return zone;
}


uma_zone_t
uma_zcache_create(const char* name, int size, uma_ctor ctor, uma_dtor dtor,
	uma_init zinit, uma_fini zfini, uma_import zimport, uma_release zrelease,
	void* arg, int flags)
{
	struct uma_zone* zone = zone_alloc();
	if (zone == NULL)
		return NULL;

	zone->size = size > 0 ? size : 0;
	zone->ctor = ctor;
	zone->dtor = dtor;
	zone->init = zinit;
	zone->fini = zfini;
	zone->import = zimport;
	zone->release = zrelease;
	zone->arg = arg;
	zone->flags = flags;
	return zone;
}


void
uma_zdestroy(uma_zone_t zone)
{
	if (zone == NULL)
		return;

	if (zone->cache != NULL)
		delete_object_cache(zone->cache);
	_kernel_free(zone);
}


static void
zone_item_release(struct uma_zone* zone, void* item)
{
	if (zone->fini != NULL)
		zone->fini(item, zone->size);

	if (zone->cache != NULL) {
		object_cache_free(zone->cache, item, 0);
		return;
	}

	zone->release(zone->arg, &item, 1);
}


void*
uma_zalloc_arg(uma_zone_t zone, void* arg, int flags)
{
	void* item;

	if (zone->cache != NULL) {
		item = object_cache_alloc(zone->cache, uma_to_oc_flags(flags));
		if (item == NULL)
			return NULL;
		if (zone_item_init(zone, item, flags) != 0) {
			object_cache_free(zone->cache, item, 0);
			return NULL;
		}
	} else {
		if (zone->import(zone->arg, &item, 1, UMA_ANYDOMAIN, flags) != 1)
			return NULL;
		if (zone_item_init(zone, item, flags) != 0) {
			zone->release(zone->arg, &item, 1);
			return NULL;
		}
	}

	if (zone->ctor != NULL && zone->ctor(item, zone->size, arg, flags) != 0) {
		zone_item_release(zone, item);
		return NULL;
	}

	if ((flags & M_ZERO) != 0 && zone->size > 0)
		memset(item, 0, zone->size);

	return item;
}


void
uma_zfree_arg(uma_zone_t zone, void* item, void* arg)
{
	if (item == NULL)
		return;

	if (zone->dtor != NULL)
		zone->dtor(item, zone->size, arg);
	zone_item_release(zone, item);
}


void
uma_zone_reserve(uma_zone_t zone, int nitems)
{
	if (zone->cache != NULL)
		object_cache_set_minimum_reserve(zone->cache, nitems);
}


void
uma_prealloc(uma_zone_t zone, int itemcnt)
{
	if (zone->cache != NULL)
		object_cache_reserve(zone->cache, itemcnt, 0);
}


unsigned int
uma_get_cache_align_mask(void)
{
	return CACHE_LINE_SIZE - 1;
}
