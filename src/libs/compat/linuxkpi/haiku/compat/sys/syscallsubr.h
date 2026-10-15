/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SYSCALLSUBR_H_
#define _LKPI_HAIKU_SYS_SYSCALLSUBR_H_

/*
 * The one call LinuxKPI's <linux/kmod.h> uses from FreeBSD's
 * <sys/syscallsubr.h>. There is no kld on Haiku; the implementation
 * (lkpi_machdep.c) fails with ENOENT, so request_module() is a no-op
 * that reports the module as unavailable.
 */

struct thread;

int kern_kldload(struct thread* td, const char* file, int* fileid);


#endif /* _LKPI_HAIKU_SYS_SYSCALLSUBR_H_ */
