/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_VM_VM_OBJECT_H_
#define _LKPI_HAIKU_VM_VM_OBJECT_H_

/*
 * FreeBSD <vm/vm_object.h> placeholder. The rtw88 path uses no VM object
 * APIs (RECON.md); <linux/mm.h>'s mmap helpers only need the type and
 * lock macros to compile.
 */

#include <vm/vm.h>


struct vm_object {
	int	dummy;
};

#define VM_OBJECT_WLOCK(object)			do { } while (0)
#define VM_OBJECT_WUNLOCK(object)		do { } while (0)
#define VM_OBJECT_RLOCK(object)			do { } while (0)
#define VM_OBJECT_RUNLOCK(object)		do { } while (0)
#define VM_OBJECT_ASSERT_WLOCKED(object)	do { } while (0)
#define VM_OBJECT_ASSERT_LOCKED(object)		do { } while (0)

void vm_object_deallocate(vm_object_t object);

/* No pages ever belong to it here; only identity comparisons. */
extern struct vm_object lkpi_kernel_object_store;
#define kernel_object	(&lkpi_kernel_object_store)


#endif /* _LKPI_HAIKU_VM_OBJECT_H_ */
