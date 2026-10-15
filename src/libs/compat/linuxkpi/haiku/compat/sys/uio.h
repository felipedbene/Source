/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_UIO_H_
#define _LKPI_HAIKU_SYS_UIO_H_

/*
 * Extends POSIX <sys/uio.h> (struct iovec) with FreeBSD's kernel
 * struct uio, which LinuxKPI's devfs read/write emulation compiles
 * against. No uio ever reaches this port - there are no Linux device
 * nodes.
 */

#include_next <sys/uio.h>


enum uio_rw { UIO_READ, UIO_WRITE };

enum uio_seg {
	UIO_USERSPACE,
	UIO_SYSSPACE,
	UIO_NOCOPY
};

struct uio {
	struct iovec*	uio_iov;
	int				uio_iovcnt;
	off_t			uio_offset;
	ssize_t			uio_resid;
	enum uio_seg	uio_segflg;
	enum uio_rw		uio_rw;
	struct thread*	uio_td;
};


#endif /* _LKPI_HAIKU_SYS_UIO_H_ */
