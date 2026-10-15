/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_KERNEL_H_
#define _LKPI_HAIKU_SYS_KERNEL_H_

/*
 * Extends freebsd_network's <sys/kernel.h> with the sysinit orders
 * LinuxKPI additionally uses.
 */

#include_next <sys/kernel.h>


#ifndef SI_ORDER_LAST
#	define SI_ORDER_LAST	SI_ORDER_ANY
#endif


#endif /* _LKPI_HAIKU_SYS_KERNEL_H_ */
