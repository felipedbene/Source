/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_CPUSET_H_
#define _LKPI_HAIKU_SYS_CPUSET_H_

/*
 * The CPU_*() subset of FreeBSD's <sys/cpuset.h> that LinuxKPI uses, on
 * the vendored generic bitsets. The kernel cpuset management API
 * (cpuset_setithread() & co.) is left out.
 */

#include <sys/_cpuset.h>

#include <sys/bitset.h>
#include <sys/smp.h>


#define CPU_ZERO(set)			BIT_ZERO(CPU_SETSIZE, set)
#define CPU_FILL(set)			BIT_FILL(CPU_SETSIZE, set)
#define CPU_SETOF(cpu, set)		BIT_SETOF(CPU_SETSIZE, cpu, set)
#define CPU_SET(cpu, set)		BIT_SET(CPU_SETSIZE, cpu, set)
#define CPU_CLR(cpu, set)		BIT_CLR(CPU_SETSIZE, cpu, set)
#define CPU_ISSET(cpu, set)		BIT_ISSET(CPU_SETSIZE, cpu, set)
#define CPU_COPY(from, to)		BIT_COPY(CPU_SETSIZE, from, to)
#define CPU_CMP(set1, set2)		BIT_CMP(CPU_SETSIZE, set1, set2)
#define CPU_EMPTY(set)			BIT_EMPTY(CPU_SETSIZE, set)
#define CPU_ISFULLSET(set)		BIT_ISFULLSET(CPU_SETSIZE, set)
#define CPU_FFS(set)			BIT_FFS(CPU_SETSIZE, set)
#define CPU_COUNT(set)			((int)BIT_COUNT(CPU_SETSIZE, set))
#define CPU_AND(dst, set1, set2) BIT_AND2(CPU_SETSIZE, dst, set1, set2)
#define CPU_OR(dst, set1, set2)	BIT_OR2(CPU_SETSIZE, dst, set1, set2)
#define CPU_ANDNOT(dst, set1, set2) BIT_ANDNOT2(CPU_SETSIZE, dst, set1, set2)

/* Every CPU up to mp_maxid is present. */
#define CPU_ABSENT(cpu)			0
#define CPU_FOREACH(i)							\
	for ((i) = 0; (i) <= (int)mp_maxid; (i)++)

extern cpuset_t all_cpus;
extern cpuset_t* cpuset_root;		/* the root set: all CPUs */

#define CPU_WHICH_IRQ	5	/* an irq number, as FreeBSD numbers it */


#endif /* _LKPI_HAIKU_SYS_CPUSET_H_ */
