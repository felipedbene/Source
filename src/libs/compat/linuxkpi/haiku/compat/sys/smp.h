/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SMP_H_
#define _LKPI_HAIKU_SYS_SMP_H_

/*
 * Extends freebsd_network's <sys/smp.h> (mp_ncpus, mp_maxid) with what
 * LinuxKPI additionally uses, and re-points MAXCPU at the real limit
 * (see machine/param.h). mp_ncores is defined in lkpi_machdep.c.
 */

#include <machine/param.h>	/* LKPI_MAXCPU */

#undef MAXCPU
#include_next <sys/smp.h>

#undef MAXCPU
#define MAXCPU	LKPI_MAXCPU


extern int mp_ncores;


#endif /* _LKPI_HAIKU_SYS_SMP_H_ */
