/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_VMMETER_H_
#define _LKPI_HAIKU_SYS_VMMETER_H_

/*
 * The subset of FreeBSD's <sys/vmmeter.h> that LinuxKPI uses: only
 * vm_free_count() (linux_page.c's si_meminfo()).
 */

u_int vm_free_count(void);


#endif /* _LKPI_HAIKU_SYS_VMMETER_H_ */
