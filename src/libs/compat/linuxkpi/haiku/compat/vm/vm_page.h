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
};

#define VM_PAGE_TO_PHYS(page)	((page)->phys_addr)

vm_page_t PHYS_TO_VM_PAGE(vm_paddr_t pa);

/* Declared so <linux/mm.h>'s inlines compile; implemented if something
 * on the rtw88 path turns out to use them (nothing does). */
void vm_page_dirty(vm_page_t page);
void vm_page_reference(vm_page_t page);
void vm_page_wire(vm_page_t page);
bool vm_page_unwire(vm_page_t page, int queue);
int vm_page_tryxbusy(vm_page_t page);
void vm_page_xunbusy(vm_page_t page);


#endif /* _LKPI_HAIKU_VM_VM_PAGE_H_ */
