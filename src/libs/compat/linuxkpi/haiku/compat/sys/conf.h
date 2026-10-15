/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_CONF_H_
#define _LKPI_HAIKU_SYS_CONF_H_

/*
 * The devfs/cdev slice of FreeBSD's <sys/conf.h> that LinuxKPI's char-
 * device emulation (linux/cdev.h, linux_compat.c) compiles against.
 * Haiku has no devfs for LinuxKPI devices: make_dev_s() fails with
 * ENODEV (lkpi_cdev.c), so no Linux /dev node ever exists - the wifi
 * path does not use them.
 */

#include <sys/types.h>
#include <sys/queue.h>


struct cdevsw;
struct file;
struct thread;
struct ucred;

struct cdev {
	void*			si_drv1;
	void*			si_drv2;
	struct cdevsw*	si_devsw;
	LIST_ENTRY(cdev) si_list;
	char			si_name[64];
};

typedef int d_open_t(struct cdev* dev, int oflags, int devtype,
	struct thread* td);
typedef int d_fdopen_t(struct cdev* dev, int oflags, struct thread* td,
	struct file* fp);
typedef int d_close_t(struct cdev* dev, int fflag, int devtype,
	struct thread* td);

struct cdevsw {
	int				d_version;
	const char*		d_name;
	d_open_t*		d_open;
	d_fdopen_t*		d_fdopen;
	d_close_t*		d_close;
	LIST_HEAD(, cdev) d_devs;
};

/* the global devfs lock; nothing concurrent exists here */
#define dev_lock()		do { } while (0)
#define dev_unlock()	do { } while (0)

#define D_VERSION	0x17122009

/* FreeBSD caps single devfs transfers; any large bound serves here. */
#define DEVFS_IOSIZE_MAX	((ssize_t)1 << 30)

struct make_dev_args {
	int				mda_flags;
	struct cdevsw*	mda_devsw;
	struct ucred*	mda_cr;
	uid_t			mda_uid;
	gid_t			mda_gid;
	int				mda_mode;
	int				mda_unit;
	void*			mda_si_drv1;
	void*			mda_si_drv2;
};

void make_dev_args_init(struct make_dev_args* args);
int make_dev_s(struct make_dev_args* args, struct cdev** result,
	const char* fmt, ...) __printflike(3, 4);
void destroy_dev(struct cdev* dev);
const char* devtoname(struct cdev* dev);


#endif /* _LKPI_HAIKU_SYS_CONF_H_ */
