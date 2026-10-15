/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_FCNTL_H_
#define _LKPI_HAIKU_SYS_FCNTL_H_

/*
 * FreeBSD's kernel <sys/fcntl.h>: POSIX's plus the kernel-only open
 * flag spellings linux_compat.c's devfs emulation tests.
 */

#include <fcntl.h>

#ifndef FREAD
#	define FREAD	0x0001
#	define FWRITE	0x0002
#endif
#ifndef FASYNC
#	define FASYNC	O_NONBLOCK << 1	/* distinct bit; only compared */
#endif
#ifndef O_FSYNC
#	define O_FSYNC	O_SYNC
#endif

#endif /* _LKPI_HAIKU_SYS_FCNTL_H_ */
