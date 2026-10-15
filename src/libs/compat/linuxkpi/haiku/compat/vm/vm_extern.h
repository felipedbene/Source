/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_EXTERN_H_
#define _LKPI_HAIKU_VM_VM_EXTERN_H_

/*
 * Extends freebsd_network's <vm/vm_extern.h> with the calls LinuxKPI's
 * mmap emulation names; unreachable stubs here (lkpi_vm.c/lkpi_cdev.c).
 */

#include_next <vm/vm_extern.h>

#include <vm/vm_map.h>

#endif /* _LKPI_HAIKU_VM_VM_EXTERN_H_ */
