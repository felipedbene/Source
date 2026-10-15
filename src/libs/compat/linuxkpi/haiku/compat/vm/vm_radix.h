/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_RADIX_H_
#define _LKPI_HAIKU_VM_VM_RADIX_H_

/*
 * FreeBSD <vm/vm_radix.h>: linux_page.c only needs struct pctrie_iter
 * (vendored pctrie) for the vm_page_grab_iter() path, which is a
 * panicking stub here - no VM objects hold pages on this port.
 */

#include <sys/pctrie.h>


struct vm_object;
struct vm_page;

/* No VM object ever holds pages on this port, so iteration finds none
 * (lkpi_vm.c). */
void vm_page_iter_limit_init(struct pctrie_iter* pages,
	struct vm_object* object, unsigned long limit);

#define VM_RADIX_FOREACH_FROM(m, pages, start) \
	for ((m) = ((void)(pages), (void)(start), (struct vm_page*)0); \
		(m) != (struct vm_page*)0; )


#endif /* _LKPI_HAIKU_VM_VM_RADIX_H_ */
