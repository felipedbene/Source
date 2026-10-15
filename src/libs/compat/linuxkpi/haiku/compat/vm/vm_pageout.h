/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_PAGEOUT_H_
#define _LKPI_HAIKU_VM_VM_PAGEOUT_H_

/*
 * The slice of FreeBSD's <vm/vm_pageout.h> LinuxKPI uses. There is no
 * pagedaemon proc here (pageproc stays NULL, so curproc never matches),
 * and vm_wait() just naps (lkpi_vm.c).
 */

#include <vm/vm.h>


extern struct proc* pageproc;

void vm_wait(struct vm_object* object);


#endif /* _LKPI_HAIKU_VM_VM_PAGEOUT_H_ */
