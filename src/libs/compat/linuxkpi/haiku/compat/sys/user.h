/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_USER_H_
#define _LKPI_HAIKU_SYS_USER_H_

/*
 * The kinfo_file slice of FreeBSD's <sys/user.h> that linux_compat.c's
 * fo_fill_kinfo fills in; nothing ever reads it on this port.
 */

struct kinfo_file {
	int		kf_type;
	char	kf_path[256];
};

#define KF_TYPE_VNODE	1
#define KF_TYPE_DEV		12

#define FILEDESC_SLOCK(fdp)		do { } while (0)
#define FILEDESC_SUNLOCK(fdp)	do { } while (0)

#endif /* _LKPI_HAIKU_SYS_USER_H_ */
