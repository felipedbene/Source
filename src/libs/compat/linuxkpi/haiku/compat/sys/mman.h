/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_MMAN_H_
#define _LKPI_HAIKU_SYS_MMAN_H_

/*
 * Extends POSIX <sys/mman.h> with the FreeBSD mmap flags linux_compat.c
 * names.
 */

#include_next <sys/mman.h>

#ifndef MAP_COPY
#	define MAP_COPY	MAP_PRIVATE
#endif

#endif /* _LKPI_HAIKU_SYS_MMAN_H_ */
