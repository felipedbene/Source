/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_RMAN_H_
#define _LKPI_HAIKU_SYS_RMAN_H_

/*
 * Extends freebsd_network's <sys/rman.h> with the resource-range type
 * newer FreeBSD code spells rman_res_t.
 */

#include_next <sys/rman.h>


typedef uint64_t rman_res_t;


#endif /* _LKPI_HAIKU_SYS_RMAN_H_ */
