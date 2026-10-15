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


#endif /* _LKPI_HAIKU_SYS_TYPES_H_ */
