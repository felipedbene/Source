/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SOCKET_H_
#define _LKPI_HAIKU_SYS_SOCKET_H_

/*
 * Extends freebsd_network's <sys/socket.h>, renaming the POSIX struct
 * ucred it pulls in out of the way: in LinuxKPI translation units, struct
 * ucred is the FreeBSD-shaped one from this port's <sys/proc.h>
 * (td_ucred->cr_uid).
 */

#define ucred	__haiku_posix_ucred
#include_next <sys/socket.h>
#undef ucred


#endif /* _LKPI_HAIKU_SYS_SOCKET_H_ */
