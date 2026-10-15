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

#define VM_OBJECT_WLOCK(object)		do { } while (0)
#define VM_OBJECT_WUNLOCK(object)	do { } while (0)
#define VM_OBJECT_RLOCK(object)		do { } while (0)
#define VM_OBJECT_RUNLOCK(object)	do { } while (0)


#endif /* _LKPI_HAIKU_VM_OBJECT_H_ */
