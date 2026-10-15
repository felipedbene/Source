/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_PAGE_H_
#define _LKPI_HAIKU_VM_VM_PAGE_H_

/*
 * A minimal FreeBSD <vm/vm_page.h> for LinuxKPI. Shadows the kernel's
 * private C++-only <vm/vm_page.h>, which include paths would otherwise
 * find.
 *
 * The rtw88 wifi path uses no page APIs (RECON.md); LinuxKPI only needs
 * the types to compile. The accessors are implemented as stubs when
 * linux_page.c is brought up.
 */

#include <vm/vm.h>


struct vm_page {
	vm_paddr_t	phys_addr;
	struct vm_object* object;
	vm_pindex_t	pindex;
	int			flags;		/* PG_* */
	int			oflags;		/* VPO_* */
	u_int		dirty;
};

#define PG_FICTITIOUS	0x0004
#define VPO_UNMANAGED	0x0001

#define VM_PAGE_BITS_ALL	(~0u)

/* page queues */
#define PQ_INACTIVE		0
#define PQ_ACTIVE		1

/* vm_page_alloc*() request flags (FreeBSD semantics, local values) */
#define VM_ALLOC_WIRED		0x0001
#define VM_ALLOC_ZERO		0x0002
#define VM_ALLOC_NORECLAIM	0x0004
#define VM_ALLOC_NOCREAT	0x0008
#define VM_ALLOC_WAITFAIL	0x0010

#define VM_PAGE_TO_PHYS(page)	((page)->phys_addr)

vm_page_t PHYS_TO_VM_PAGE(vm_paddr_t pa);

/* Declared so <linux/mm.h> and linux_page.c compile; the rtw88 path
 * allocates no VM pages, so these are panicking stubs (lkpi_vm.c). */
void vm_page_dirty(vm_page_t page);
void vm_page_reference(vm_page_t page);
void vm_page_wire(vm_page_t page);
bool vm_page_unwire(vm_page_t page, int queue);
bool vm_page_unwire_noq(vm_page_t page);
int vm_page_tryxbusy(vm_page_t page);
void vm_page_xunbusy(vm_page_t page);
void vm_page_free(vm_page_t page);
void vm_page_valid(vm_page_t page);
vm_page_t vm_page_alloc_noobj(int req);
vm_page_t vm_page_alloc_noobj_contig(int req, u_long npages,
	vm_paddr_t low, vm_paddr_t high, u_long alignment, vm_paddr_t boundary,
	vm_memattr_t memattr);
vm_page_t vm_page_alloc_noobj_contig_domain(int domain, int req,
	u_long npages, vm_paddr_t low, vm_paddr_t high, u_long alignment,
	vm_paddr_t boundary, vm_memattr_t memattr);
int vm_page_reclaim_contig(int req, u_long npages, vm_paddr_t low,
	vm_paddr_t high, u_long alignment, vm_paddr_t boundary);
void vm_page_initfake(vm_page_t page, vm_paddr_t paddr,
	vm_memattr_t memattr);
void vm_page_updatefake(vm_page_t page, vm_paddr_t paddr,
	vm_memattr_t memattr);
vm_page_t vm_page_getfake(vm_paddr_t paddr, vm_memattr_t memattr);
void vm_page_putfake(vm_page_t page);
vm_page_t vm_page_replace(vm_page_t mnew, struct vm_object* object,
	vm_pindex_t pindex, vm_page_t mold);
int vm_page_busy_acquire(vm_page_t page, int allocflags);

struct pctrie_iter;
void vm_page_iter_init(struct pctrie_iter* pages, vm_object_t object);
vm_page_t vm_page_grab_iter(vm_object_t object, vm_pindex_t pindex,
	int allocflags, struct pctrie_iter* pages);
int vm_page_iter_insert(vm_page_t page, vm_object_t object,
	vm_pindex_t pindex, struct pctrie_iter* pages);
void vm_page_remove(vm_page_t page);


#endif /* _LKPI_HAIKU_VM_VM_PAGE_H_ */
