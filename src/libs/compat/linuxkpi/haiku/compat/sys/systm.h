/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Extends freebsd_network's <sys/systm.h> with what FreeBSD gets there from
 * <sys/kassert.h>.
 */

#include_next <sys/systm.h>

#ifndef _LKPI_HAIKU_SYS_SYSTM_H_
#define _LKPI_HAIKU_SYS_SYSTM_H_


#ifndef __assert_unreachable
#	define __assert_unreachable() \
		do { \
			panic("executing segment marked as unreachable at %s:%d (%s)\n", \
				__FILE__, __LINE__, __func__); \
			__builtin_unreachable(); \
		} while (0)
#endif


#endif /* _LKPI_HAIKU_SYS_SYSTM_H_ */
