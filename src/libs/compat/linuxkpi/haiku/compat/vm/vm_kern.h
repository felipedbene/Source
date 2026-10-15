/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_KERN_H_
#define _LKPI_HAIKU_VM_VM_KERN_H_

/*
 * FreeBSD <vm/vm_kern.h> subset for LinuxKPI's dma/page paths.
 * Implemented when linux_page.c is brought up (contiguous allocations
 * via Haiku areas, like freebsd_network's _kernel_contigmalloc).
 */

#include <vm/vm.h>


vm_offset_t kmem_alloc_contig(vm_size_t size, int flags, vm_paddr_t low,
	vm_paddr_t high, u_long alignment, vm_paddr_t boundary,
	vm_memattr_t memattr);
void kmem_free(void* addr, vm_size_t size);


#endif /* _LKPI_HAIKU_VM_KERN_H_ */
