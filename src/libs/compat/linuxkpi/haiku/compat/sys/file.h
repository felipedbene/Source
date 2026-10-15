/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_FILE_H_
#define _LKPI_HAIKU_SYS_FILE_H_

/*
 * A minimal FreeBSD file(9) <sys/file.h> for LinuxKPI; shadows the POSIX
 * flock() header the include paths would otherwise find. Haiku has no
 * devfs for LinuxKPI char devices, so struct file objects are never
 * actually created on this port - linux/fs.h only needs the type to
 * compile, and the wifi path never opens Linux device nodes.
 */

#include <sys/types.h>

#include <sys/refcount.h>
#include <sys/selinfo.h>


struct vnode;
struct cap_rights;
struct fileops;

struct file {
	void*			f_data;
	struct fileops*	f_ops;
	struct vnode*	f_vnode;
	short			f_type;
	int				f_flag;
	volatile u_int	f_count;
	off_t			f_offset;
};

/* file types (FreeBSD values) */
#define DTYPE_VNODE		1
#define DTYPE_DEV		11

/* Implemented (as ENODEV stubs) if something ever links them: there is
 * no Linux fd table on this port. */
int fget(struct thread* td, int fd, struct cap_rights* rights,
	struct file** fpp);
int fget_unlocked(struct thread* td, int fd, struct cap_rights* rights,
	struct file** fpp);
int fdrop(struct file* fp, struct thread* td);
void finit(struct file* fp, u_int flag, short type, void* data,
	struct fileops* ops);
int falloc(struct thread* td, struct file** resultfp, int* resultfd,
	int flags);
void fdclose(struct thread* td, struct file* fp, int fd);


#endif /* _LKPI_HAIKU_SYS_FILE_H_ */
