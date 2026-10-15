/* SPDX-License-Identifier: MIT */
/*
 * FreeBSD <sys/jail.h> placeholder: included by <linux/list.h> only so
 * FreeBSD LIST_HEAD users are parsed before Linux takes the name, plus
 * the osrelease length <linux/utsname.h> sizes its buffer with.
 */
#ifndef _LKPI_HAIKU_SYS_JAIL_H_
#define _LKPI_HAIKU_SYS_JAIL_H_

#define OSRELEASELEN	32

extern char osrelease[OSRELEASELEN];

#endif
