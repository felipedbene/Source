/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_MAP_H_
#define _LKPI_HAIKU_VM_VM_MAP_H_

/*
 * A minimal FreeBSD <vm/vm_map.h> for linux_page.c's get_user_pages
 * paths. There are no user address spaces behind LinuxKPI on this port
 * (struct proc's p_vmspace stays NULL); the functions are panicking
 * stubs (lkpi_vm.c) and nothing on the rtw88 path maps user memory.
 */

#include <vm/vm.h>


struct vm_map {
	void*	pmap;
};

struct vmspace {
	struct vm_map	vm_map;
};

bool vm_map_range_valid(struct vm_map* map, vm_offset_t start,
	vm_offset_t end);
int vm_fault_quick_hold_pages(struct vm_map* map, vm_offset_t addr,
	vm_size_t len, vm_prot_t prot, struct vm_page** ma, int max_count);

struct thread;
int vm_mmap_object(struct vm_map* map, vm_offset_t* addr, vm_size_t size,
	vm_prot_t prot, vm_prot_t maxprot, int flags, vm_object_t object,
	vm_ooffset_t foff, int writecounted, struct thread* td);


#endif /* _LKPI_HAIKU_VM_VM_MAP_H_ */
