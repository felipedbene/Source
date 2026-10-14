/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_PCPU_H_
#define _LKPI_HAIKU_SYS_PCPU_H_

/*
 * Shadows freebsd_network's <sys/pcpu.h> for LinuxKPI code only.
 *
 * freebsd_network deliberately defines curthread as NULL, but LinuxKPI needs a
 * real, stable struct thread per thread: it hangs "current" (td_lkpi_task) off
 * it and compares sx owners against it. Those are shadow threads (see
 * lkpi_thread.c), not Haiku's Thread, so the reasoning behind the NULL
 * curthread in freebsd_network still holds for everything outside LinuxKPI.
 */

#include <OS.h>
#include <sys/smp.h>


struct thread;

__BEGIN_DECLS
struct thread* lkpi_curthread(void);
__END_DECLS

#define curthread (lkpi_curthread())


#endif /* _LKPI_HAIKU_SYS_PCPU_H_ */
