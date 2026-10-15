/* SPDX-License-Identifier: MIT */
/*
 * FreeBSD <sys/sysent.h> subset: the ABI check in linux_compat.c; this
 * port reports "no ABI", so the FreeBSD-syscall thunk path is skipped.
 */
#ifndef _LKPI_HAIKU_SYS_SYSENT_H_
#define _LKPI_HAIKU_SYS_SYSENT_H_

#define SV_ABI_FREEBSD	9
#define SV_PROC_ABI(p)	(0)

#endif
