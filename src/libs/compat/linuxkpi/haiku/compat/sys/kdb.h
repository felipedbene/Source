/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_KDB_H_
#define _LKPI_HAIKU_SYS_KDB_H_

/*
 * FreeBSD <sys/kdb.h>: drivers never see the kernel debugger holding the
 * system here (also defined by this port's <sys/systm.h>).
 */

#ifndef kdb_active
#	define kdb_active	0
#endif


#endif /* _LKPI_HAIKU_SYS_KDB_H_ */
