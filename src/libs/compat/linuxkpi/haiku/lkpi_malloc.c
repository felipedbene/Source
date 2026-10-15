/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Size-tracked malloc for LinuxKPI translation units.
 *
 * Haiku's kernel heap has no usable-size query, but linux/slab.h needs
 * ksize()/malloc_usable_size() (krealloc copies exactly that many bytes)
 * and kfree_sensitive() needs zfree() (zero before free). A size-tag
 * header in front of each allocation is not an option: LinuxKPI-allocated
 * memory can be freed by freebsd_network/net80211 code and vice versa,
 * and a header offset would corrupt the heap on any such cross-boundary
 * free. Instead, allocations made through LinuxKPI's malloc()/
 * contigmalloc() macros (see the sys/malloc.h shadow) are recorded in a
 * pointer-keyed hash table on the side.
 *
 * The table also fixes a real mismatch: lkpi___kmalloc() gets anything
 * larger than PAGE_SIZE from contigmalloc() (an area on Haiku), but
 * lkpi_kfree() releases everything with free(). FreeBSD's free(9) copes;
 * Haiku's heap free cannot release an area. The tracked free dispatches
 * on how the memory was obtained.
 *
 * Cross-boundary consequences, by direction:
 *  - allocated outside LinuxKPI, freed here: lookup misses, plain
 *    _kernel_free() - same behavior as freebsd_network's own free().
 *  - allocated here, freed outside: frees fine (same heap), but the hash
 *    entry leaks (32 bytes). There is no way to test pointer liveness,
 *    so no sweep is possible; such crossings are rare on the wifi path.
 */

#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/systm.h>

#include <KernelExport.h>
#include <string.h>


/* From the kernel's private <heap.h>, which can only be included from C++. */
extern void* realloc_etc(void* address, size_t newSize, uint32 flags);
#define LKPI_HEAP_DONT_WAIT_FOR_MEMORY	0x01	/* HEAP_DONT_WAIT_FOR_MEMORY */


#define LKPI_MALLOC_HASH_SIZE	1024	/* must be a power of two */

struct lkpi_alloc_entry {
	struct lkpi_alloc_entry*	next;
	void*						ptr;
	size_t						size;
	bool						contig;
};

static struct lkpi_alloc_entry* sAllocHash[LKPI_MALLOC_HASH_SIZE];
static spinlock sAllocHashLock = B_SPINLOCK_INITIALIZER;


static inline struct lkpi_alloc_entry**
lkpi_alloc_bucket(const void* ptr)
{
	/* Fibonacci hash of the address, minus its alignment zeros. */
	uint64 hash = ((uint64)(addr_t)ptr >> 4) * 11400714819323198485ULL;
	return &sAllocHash[(hash >> 32) & (LKPI_MALLOC_HASH_SIZE - 1)];
}


static void
lkpi_alloc_insert(struct lkpi_alloc_entry* entry)
{
	struct lkpi_alloc_entry** bucket = lkpi_alloc_bucket(entry->ptr);
	cpu_status state = disable_interrupts();
	acquire_spinlock(&sAllocHashLock);
	entry->next = *bucket;
	*bucket = entry;
	release_spinlock(&sAllocHashLock);
	restore_interrupts(state);
}


/* Unlinks and returns the entry for ptr, or NULL if it is not tracked. */
static struct lkpi_alloc_entry*
lkpi_alloc_remove(const void* ptr)
{
	struct lkpi_alloc_entry** link = lkpi_alloc_bucket(ptr);
	struct lkpi_alloc_entry* entry;

	cpu_status state = disable_interrupts();
	acquire_spinlock(&sAllocHashLock);
	while ((entry = *link) != NULL) {
		if (entry->ptr == ptr) {
			*link = entry->next;
			break;
		}
		link = &entry->next;
	}
	release_spinlock(&sAllocHashLock);
	restore_interrupts(state);
	return entry;
}


/* Returns the tracked size of ptr, or 0 if it is not tracked. */
size_t
malloc_usable_size(const void* ptr)
{
	struct lkpi_alloc_entry* entry;
	size_t size = 0;

	cpu_status state = disable_interrupts();
	acquire_spinlock(&sAllocHashLock);
	for (entry = *lkpi_alloc_bucket(ptr); entry != NULL; entry = entry->next) {
		if (entry->ptr == ptr) {
			size = entry->size;
			break;
		}
	}
	release_spinlock(&sAllocHashLock);
	restore_interrupts(state);
	return size;
}


void*
lkpi_malloc_tracked(size_t size, int flags)
{
	struct lkpi_alloc_entry* entry;
	void* ptr;

	entry = _kernel_malloc(sizeof(*entry), flags & ~M_ZERO);
	if (entry == NULL)
		return NULL;

	ptr = _kernel_malloc(size, flags);
	if (ptr == NULL) {
		_kernel_free(entry);
		return NULL;
	}

	entry->ptr = ptr;
	entry->size = size;
	entry->contig = false;
	lkpi_alloc_insert(entry);
	return ptr;
}


void*
lkpi_contigmalloc_tracked(size_t size, int flags, vm_paddr_t low,
	vm_paddr_t high, unsigned long alignment, unsigned long boundary)
{
	struct lkpi_alloc_entry* entry;
	void* ptr;

	entry = _kernel_malloc(sizeof(*entry), flags & ~M_ZERO);
	if (entry == NULL)
		return NULL;

	ptr = _kernel_contigmalloc("lkpi_malloc", __LINE__, size, flags, low,
		high, alignment, boundary);
	if (ptr == NULL) {
		_kernel_free(entry);
		return NULL;
	}

	entry->ptr = ptr;
	entry->size = size;
	entry->contig = true;
	lkpi_alloc_insert(entry);
	return ptr;
}


void
lkpi_free_tracked(void* ptr)
{
	struct lkpi_alloc_entry* entry;

	if (ptr == NULL)
		return;

	entry = lkpi_alloc_remove(ptr);
	if (entry == NULL) {
		/* Not allocated through LinuxKPI; same as freebsd_network's free. */
		_kernel_free(ptr);
		return;
	}

	if (entry->contig)
		_kernel_contigfree(ptr, entry->size);
	else
		_kernel_free(ptr);
	_kernel_free(entry);
}


void
lkpi_contigfree_tracked(void* ptr, unsigned long size)
{
	struct lkpi_alloc_entry* entry;

	if (ptr == NULL)
		return;

	entry = lkpi_alloc_remove(ptr);
	if (entry == NULL) {
		/* Untracked, but the caller vouches it is contiguous memory. */
		_kernel_contigfree(ptr, size);
		return;
	}

	if (entry->contig)
		_kernel_contigfree(ptr, entry->size);
	else
		_kernel_free(ptr);
	_kernel_free(entry);
}


/* kfree_sensitive(): zero before freeing. An untracked pointer cannot be
 * zeroed (unknown size); it is just freed. */
void
lkpi_zfree(void* ptr)
{
	struct lkpi_alloc_entry* entry;

	if (ptr == NULL)
		return;

	entry = lkpi_alloc_remove(ptr);
	if (entry == NULL) {
		_kernel_free(ptr);
		return;
	}

	memset(ptr, 0, entry->size);
	if (entry->contig)
		_kernel_contigfree(ptr, entry->size);
	else
		_kernel_free(ptr);
	_kernel_free(entry);
}


void*
lkpi_realloc(void* ptr, size_t size, int flags)
{
	size_t osize;
	void* nptr;

	if (ptr == NULL)
		return lkpi_malloc_tracked(size, flags);

	osize = malloc_usable_size(ptr);
	if (osize == 0) {
		/* Untracked: only the kernel heap knows the old size. The result
		 * stays untracked; M_ZERO cannot be honored here. */
		return realloc_etc(ptr, size, (flags & M_NOWAIT) != 0
			? LKPI_HEAP_DONT_WAIT_FOR_MEMORY : 0);
	}

	nptr = lkpi_malloc_tracked(size, flags);
	if (nptr == NULL)
		return NULL;

	memcpy(nptr, ptr, MIN(osize, size));
	lkpi_free_tracked(ptr);
	return nptr;
}
