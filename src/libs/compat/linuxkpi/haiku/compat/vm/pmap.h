/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_PMAP_H_
#define _LKPI_HAIKU_VM_PMAP_H_

/*
 * The subset of FreeBSD's pmap API LinuxKPI's <linux/page.h> uses.
 * Implemented in lkpi_machdep.c. vtophys()/pmap_kextract() come from
 * freebsd_network's <vm/vm.h>.
 */

#include <vm/vm.h>


void pmap_invalidate_cache(void);
void pmap_force_invalidate_cache_range(vm_offset_t start, vm_offset_t end);
int pmap_change_attr(vm_offset_t va, vm_size_t size, int memattr);
void pmap_page_set_memattr(struct vm_page* page, vm_memattr_t memattr);
void pmap_qenter(vm_offset_t va, struct vm_page** pages, int count);
void pmap_qremove(vm_offset_t va, int count);
struct vm_page* pmap_extract_and_hold(void* pmap, vm_offset_t va,
	vm_prot_t prot);
bool pmap_page_is_mapped(struct vm_page* page);
void* pmap_mapdev_attr(vm_paddr_t pa, vm_size_t size, vm_memattr_t attr);
void pmap_unmapdev(void* va, vm_size_t size);
void pmap_remove_all(struct vm_page* page);


#endif /* _LKPI_HAIKU_VM_PMAP_H_ */
