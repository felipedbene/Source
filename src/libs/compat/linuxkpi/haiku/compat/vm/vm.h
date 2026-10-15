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


/* On FreeBSD from <machine/param.h>; freebsd_network's <sys/param.h>
 * only brings PAGE_SHIFT and PAGE_MASK. */
#ifndef PAGE_SIZE
#	define PAGE_SIZE	B_PAGE_SIZE
#endif

typedef size_t vm_size_t;
typedef uint64 vm_pindex_t;
typedef int64 vm_ooffset_t;

struct vm_object;
typedef struct vm_object* vm_object_t;
struct vm_map;
typedef struct vm_map* vm_map_t;

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

/*
 * No direct map on Haiku. LinuxKPI tests PMAP_HAS_DMAP at runtime, so
 * the macros must compile; reaching the conversions is a bug.
 */
#define PMAP_HAS_DMAP		0
#define PHYS_IN_DMAP(pa)	0
#define PHYS_TO_DMAP(pa) \
	({ panic("PHYS_TO_DMAP: no direct map"); (vm_offset_t)0; })
#define DMAP_TO_PHYS(va) \
	({ panic("DMAP_TO_PHYS: no direct map"); (vm_paddr_t)0; })


#endif /* _LKPI_HAIKU_VM_VM_H_ */
