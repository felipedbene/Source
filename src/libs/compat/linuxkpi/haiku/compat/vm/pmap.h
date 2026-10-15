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


#endif /* _LKPI_HAIKU_VM_PMAP_H_ */
