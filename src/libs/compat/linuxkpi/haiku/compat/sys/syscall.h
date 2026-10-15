/* SPDX-License-Identifier: MIT */
/*
 * FreeBSD <sys/syscall.h> placeholder: linux_compat.c compares the
 * syscall number recorded in td_sa (always 0 here) against SYS_ioctl.
 */
#ifndef _LKPI_HAIKU_SYS_SYSCALL_H_
#define _LKPI_HAIKU_SYS_SYSCALL_H_

#define SYS_ioctl	54

#endif
