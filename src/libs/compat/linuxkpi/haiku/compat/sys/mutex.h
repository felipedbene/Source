/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_MUTEX_H_
#define _LKPI_HAIKU_SYS_MUTEX_H_

/*
 * Extends freebsd_network's <sys/mutex.h> with the mtx_init() flags
 * LinuxKPI passes; there is no witness here, so they do nothing.
 */

#include_next <sys/mutex.h>


#define MTX_NOWITNESS	0x00000000
#define MTX_NEW			0x00000000

/* No Giant lock on this port. */
#define DROP_GIANT()	do { } while (0)
#define PICKUP_GIANT()	do { } while (0)


#endif /* _LKPI_HAIKU_SYS_MUTEX_H_ */
