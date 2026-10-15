/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * FreeBSD VM-page layer stubs for linux_page.c.
 *
 * The rtw88 wifi path uses no page APIs at all (RECON.md): no
 * alloc_page/kmap/virt_to_page, no get_user_pages, no shmem. linux_page.c
 * only has to compile and link, so everything that would hand out a
 * vm_page panics with a clear message - better a loud first-use report
 * than memory corruption from a faked page. The handful of calls that
 * can be answered honestly (vm_wait, swap_pager_status) are.
 */

#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/systm.h>

#include <vm/vm.h>
#include <vm/vm_kern.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <vm/pmap.h>
#include <vm/swap_pager.h>
#include <vm/uma_int.h>

#include <OS.h>


#define LKPI_VM_UNSUPPORTED() \
	panic("linuxkpi: %s: VM page API not supported on Haiku", __func__)


struct proc* pageproc = NULL;	/* no pagedaemon; never matches curproc */

/* kernel_object: no pages ever belong to it; identity comparisons only */
struct vm_object lkpi_kernel_object_store;


vm_page_t
PHYS_TO_VM_PAGE(vm_paddr_t pa __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


vm_page_t
vm_page_alloc_noobj(int req __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


vm_page_t
vm_page_alloc_noobj_contig(int req __unused, u_long npages __unused,
	vm_paddr_t low __unused, vm_paddr_t high __unused,
	u_long alignment __unused, vm_paddr_t boundary __unused,
	vm_memattr_t memattr __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


vm_page_t
vm_page_alloc_noobj_contig_domain(int domain __unused, int req __unused,
	u_long npages __unused, vm_paddr_t low __unused,
	vm_paddr_t high __unused, u_long alignment __unused,
	vm_paddr_t boundary __unused, vm_memattr_t memattr __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


int
vm_page_reclaim_contig(int req __unused, u_long npages __unused,
	vm_paddr_t low __unused, vm_paddr_t high __unused,
	u_long alignment __unused, vm_paddr_t boundary __unused)
{
	return ENOMEM;
}


void
vm_page_dirty(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_page_reference(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_page_wire(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


bool
vm_page_unwire(vm_page_t page __unused, int queue __unused)
{
	LKPI_VM_UNSUPPORTED();
	return false;
}


bool
vm_page_unwire_noq(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
	return false;
}


int
vm_page_tryxbusy(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
	return 0;
}


void
vm_page_xunbusy(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_page_free(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_page_valid(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_page_initfake(vm_page_t page __unused, vm_paddr_t paddr __unused,
	vm_memattr_t memattr __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_page_updatefake(vm_page_t page __unused, vm_paddr_t paddr __unused,
	vm_memattr_t memattr __unused)
{
	LKPI_VM_UNSUPPORTED();
}


vm_page_t
vm_page_getfake(vm_paddr_t paddr __unused, vm_memattr_t memattr __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


void
vm_page_putfake(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


vm_page_t
vm_page_replace(vm_page_t mnew __unused, struct vm_object* object __unused,
	vm_pindex_t pindex __unused, vm_page_t mold __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


void
vm_page_iter_limit_init(struct pctrie_iter* pages __unused,
	struct vm_object* object __unused, unsigned long limit __unused)
{
	/* no VM object ever holds pages here; iteration finds none */
}


void
pmap_remove_all(struct vm_page* page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


int
vm_page_busy_acquire(vm_page_t page __unused, int allocflags __unused)
{
	LKPI_VM_UNSUPPORTED();
	return 0;
}


void
vm_page_iter_init(struct pctrie_iter* pages __unused,
	vm_object_t object __unused)
{
	LKPI_VM_UNSUPPORTED();
}


vm_page_t
vm_page_grab_iter(vm_object_t object __unused, vm_pindex_t pindex __unused,
	int allocflags __unused, struct pctrie_iter* pages __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


int
vm_page_iter_insert(vm_page_t page __unused, vm_object_t object __unused,
	vm_pindex_t pindex __unused, struct pctrie_iter* pages __unused)
{
	LKPI_VM_UNSUPPORTED();
	return ENOTSUP;
}


void
vm_page_remove(vm_page_t page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_object_deallocate(vm_object_t object __unused)
{
	LKPI_VM_UNSUPPORTED();
}


int
vm_object_set_memattr(vm_object_t object, vm_memattr_t memattr)
{
	object->memattr = memattr;
	return 0;
}


void
vm_pager_page_unswapped(struct vm_page* page __unused)
{
	LKPI_VM_UNSUPPORTED();
}


vm_object_t
cdev_pager_lookup(void* handle __unused)
{
	/* no cdev pagers exist on this port, so nothing can be found */
	return NULL;
}


void
cdev_mgtdev_pager_free_pages(vm_object_t object __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
vm_wait(struct vm_object* object __unused)
{
	/* "wait for free pages": nap and let the system make progress */
	snooze(10000);
}


bool
vm_map_range_valid(struct vm_map* map __unused, vm_offset_t start __unused,
	vm_offset_t end __unused)
{
	return false;	/* no user address spaces behind LinuxKPI */
}


int
vm_fault_quick_hold_pages(struct vm_map* map __unused,
	vm_offset_t addr __unused, vm_size_t len __unused,
	vm_prot_t prot __unused, struct vm_page** ma __unused,
	int max_count __unused)
{
	return -1;	/* get_user_pages() reports -EFAULT */
}


struct vm_page*
pmap_extract_and_hold(void* pmap __unused, vm_offset_t va __unused,
	vm_prot_t prot __unused)
{
	return NULL;
}


bool
pmap_page_is_mapped(struct vm_page* page __unused)
{
	LKPI_VM_UNSUPPORTED();
	return false;
}


void
pmap_page_set_memattr(struct vm_page* page __unused,
	vm_memattr_t memattr __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
pmap_qenter(vm_offset_t va __unused, struct vm_page** pages __unused,
	int count __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void
pmap_qremove(vm_offset_t va __unused, int count __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void*
pmap_mapdev_attr(vm_paddr_t pa __unused, vm_size_t size __unused,
	vm_memattr_t attr __unused)
{
	LKPI_VM_UNSUPPORTED();
	return NULL;
}


void
pmap_unmapdev(void* va __unused, vm_size_t size __unused)
{
	LKPI_VM_UNSUPPORTED();
}


/*
 * dma_alloc_coherent() lands here; this must really work. Haiku's
 * contiguous allocator honors the range/alignment/boundary constraints
 * (freebsd_network's _kernel_contigmalloc on create_area).
 */
vm_offset_t
kmem_alloc_contig(vm_size_t size, int flags, vm_paddr_t low, vm_paddr_t high,
	u_long alignment, vm_paddr_t boundary, vm_memattr_t memattr __unused)
{
	return (vm_offset_t)_kernel_contigmalloc("lkpi_kmem", __LINE__, size,
		flags, low, high, alignment, boundary);
}


void
kmem_free(void* addr, vm_size_t size)
{
	_kernel_contigfree(addr, size);
}


vm_offset_t
kva_alloc(vm_size_t size __unused)
{
	LKPI_VM_UNSUPPORTED();
	return 0;
}


void
kva_free(vm_offset_t addr __unused, vm_size_t size __unused)
{
	LKPI_VM_UNSUPPORTED();
}


void*
vtoslab(vm_offset_t va __unused)
{
	/* Haiku's object caches are not page-keyed slabs; "not a slab
	 * address" only widens is_vmalloc_addr()'s "no" answer. */
	return NULL;
}


void
swap_pager_status(int* total, int* used)
{
	/* no swap pager on Haiku */
	*total = 0;
	*used = 0;
}
