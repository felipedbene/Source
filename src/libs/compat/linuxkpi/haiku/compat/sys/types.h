/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_TYPES_H_
#define _LKPI_HAIKU_SYS_TYPES_H_

/*
 * Extends freebsd_network's <sys/types.h> (which has u_quad_t) with the
 * FreeBSD types LinuxKPI additionally uses.
 */

#include_next <sys/types.h>


/* Parsed from inside <SupportDefs.h>, before int64 exists. */
typedef __INT64_TYPE__ quad_t;

/* LinuxKPI's <asm/types.h> needs vm_paddr_t without going through
 * <vm/vm.h>; same type as freebsd_network's typedef there
 * (phys_addr_t, which is not defined yet here either). */
typedef __UINT64_TYPE__ vm_paddr_t;

/* BSD legacy dev_t split; only <linux/kdev_t.h>'s old_encode_dev()
 * uses them here. */
#ifndef major
#	define major(d)		((int)(((unsigned)(d) >> 8) & 0xff))
#	define minor(d)		((int)((d) & 0xffff00ff))
#	define makedev(M, m)	((dev_t)(((M) << 8) | (m)))
#endif


#endif /* _LKPI_HAIKU_SYS_TYPES_H_ */
