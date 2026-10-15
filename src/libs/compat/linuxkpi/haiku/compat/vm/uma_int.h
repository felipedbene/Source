/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_UMA_INT_H_
#define _LKPI_HAIKU_VM_UMA_INT_H_

/*
 * The UMA internals linux_page.c's is_vmalloc_addr() peeks at. Haiku's
 * object caches are not page-keyed slabs, so vtoslab() cannot exist;
 * the stub says "not a slab address" (lkpi_vm.c), which only widens
 * is_vmalloc_addr()'s "no" answer.
 */

#include <vm/vm.h>


#define UMA_SLAB_MASK	(PAGE_SIZE - 1)

void* vtoslab(vm_offset_t va);


#endif /* _LKPI_HAIKU_VM_UMA_INT_H_ */
