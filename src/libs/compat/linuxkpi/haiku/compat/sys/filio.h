/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_FILIO_H_
#define _LKPI_HAIKU_SYS_FILIO_H_

/*
 * The BSD ioctl encoding and file ioctls LinuxKPI's devfs ioctl
 * emulation decodes (FreeBSD <sys/ioccom.h>/<sys/filio.h> values). No
 * ioctl ever reaches it on this port.
 */

#define IOCPARM_SHIFT	13
#define IOCPARM_MASK	((1 << IOCPARM_SHIFT) - 1)
#define IOCPARM_LEN(x)	(((x) >> 16) & IOCPARM_MASK)
#define IOCPARM_MAX		(1 << IOCPARM_SHIFT)

#define IOC_VOID	0x20000000
#define IOC_OUT		0x40000000
#define IOC_IN		0x80000000
#define IOC_INOUT	(IOC_IN | IOC_OUT)

#define _IOC(inout, group, num, len) \
	((unsigned long)((inout) | (((len) & IOCPARM_MASK) << 16) | \
	((group) << 8) | (num)))
#define _IO(g, n)		_IOC(IOC_VOID, (g), (n), 0)
#define _IOR(g, n, t)	_IOC(IOC_OUT, (g), (n), sizeof(t))
#define _IOW(g, n, t)	_IOC(IOC_IN, (g), (n), sizeof(t))

#define FIOCLEX		_IO('f', 1)
#define FIONCLEX	_IO('f', 2)
#define FIONBIO		_IOW('f', 126, int)
#define FIOASYNC	_IOW('f', 125, int)
#define FIOSETOWN	_IOW('f', 124, int)
#define FIOGETOWN	_IOR('f', 123, int)
#define FIODGNAME	_IOW('f', 120, struct fiodgname_arg)

struct fiodgname_arg {
	int		len;
	void*	buf;
};

/* lkpi_cdev.c; the FIODGNAME path never runs here */
void* fiodgname_buf_get_ptr(void* fgnp, unsigned long com);

#endif /* _LKPI_HAIKU_SYS_FILIO_H_ */
