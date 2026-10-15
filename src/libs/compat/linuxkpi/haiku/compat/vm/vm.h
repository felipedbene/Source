/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_H_
#define _LKPI_HAIKU_VM_VM_H_

/*
 * Extends freebsd_network's <vm/vm.h> (vm_paddr_t, vm_offset_t, vtophys)
 * with the FreeBSD types and constants LinuxKPI's page/io headers use.
 * Constants have FreeBSD/x86 values.
 */

#include_next <vm/vm.h>


typedef u_char vm_prot_t;

#define VM_PROT_NONE		((vm_prot_t) 0x00)
#define VM_PROT_READ		((vm_prot_t) 0x01)
#define VM_PROT_WRITE		((vm_prot_t) 0x02)
#define VM_PROT_EXECUTE		((vm_prot_t) 0x04)
#define VM_PROT_ALL			(VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE)

typedef char vm_memattr_t;

#define VM_MEMATTR_UNCACHEABLE		((vm_memattr_t) 0)
#define VM_MEMATTR_WRITE_COMBINING	((vm_memattr_t) 1)
#define VM_MEMATTR_WRITE_THROUGH	((vm_memattr_t) 4)
#define VM_MEMATTR_WRITE_BACK		((vm_memattr_t) 6)
#define VM_MEMATTR_DEFAULT			VM_MEMATTR_WRITE_BACK

typedef struct vm_page* vm_page_t;


#endif /* _LKPI_HAIKU_VM_VM_H_ */
