/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SGLIST_H_
#define _LKPI_HAIKU_SYS_SGLIST_H_

/*
 * The sglist(9) calls linux_compat.c's SG-pager mmap path makes; that
 * path is unreachable here (lkpi_cdev.c stubs), so these never allocate.
 */

#include <vm/vm.h>


struct sglist;

struct sglist* sglist_alloc(int nsegs, int mflags);
int sglist_append_phys(struct sglist* sg, vm_paddr_t paddr, size_t len);
void sglist_free(struct sglist* sg);


#endif /* _LKPI_HAIKU_SYS_SGLIST_H_ */
