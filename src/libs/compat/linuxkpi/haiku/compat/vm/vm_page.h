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


#endif /* _LKPI_HAIKU_VM_VM_PAGE_H_ */
