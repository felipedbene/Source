/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_MACHINE_PARAM_H_
#define _LKPI_HAIKU_MACHINE_PARAM_H_

/*
 * The FreeBSD <machine/param.h> constants LinuxKPI needs. MAXCPU sizes
 * cpuset_t (sys/_cpuset.h) and matches Haiku's SMP_MAX_CPUS on x86_64.
 *
 * freebsd_network's <sys/smp.h> hardcodes MAXCPU to 1; in LinuxKPI
 * translation units the sys/smp.h shadow re-points it at LKPI_MAXCPU
 * after the include, so cpuset_t is sized the same in every unit.
 */

#define LKPI_MAXCPU		64

#undef MAXCPU
#define MAXCPU			LKPI_MAXCPU

/* CACHE_LINE_SIZE comes from freebsd_network's <sys/param.h>. */


#endif /* _LKPI_HAIKU_MACHINE_PARAM_H_ */
