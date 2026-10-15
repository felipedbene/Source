/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * FreeBSD/x86 machine-dependent variables and cache maintenance LinuxKPI
 * references (see haiku/compat/machine/md_var.h).
 */

#include <sys/param.h>
#include <sys/cpuset.h>
#include <sys/interrupt.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/smp.h>
#include <sys/syscallsubr.h>
#include <sys/systm.h>

#include <machine/md_var.h>
#include <vm/pmap.h>


/*
 * Left 0: lkpi_clflushopt() then falls back to pmap_invalidate_cache(),
 * which is always correct. Fill these from CPUID if a driver turns out to
 * flush on a hot path (nothing on the rtw88 path does).
 */
u_int cpu_feature = 0;
u_int cpu_feature2 = 0;
u_int cpu_stdext_feature = 0;

/*
 * Like freebsd_network's mp_ncpus (smp.c), a fixed conservative value;
 * only LinuxKPI's topology_num_cores_per_package() reads it.
 */
int mp_ncores = 1;

/*
 * Total physical memory in pages. Only si_meminfo() (linux_page.c)
 * reports it; fill from vm_page_num_pages() when that file is brought up.
 */
long physmem = 0;

/* From the kernel's private <smp.h>, which can only be included from C++. */
int32 smp_get_num_cpus(void);


cpuset_t all_cpus;
cpuset_t* cpuset_root = &all_cpus;


int
intr_setaffinity(int irq __unused, int mode __unused, void* mask __unused)
{
	/* Affinity is only ever a hint in LinuxKPI; report "unsupported"
	 * like Linux does without affinity control. */
	return ENOSYS;
}


/*
 * freebsd_network's smp.c fixes mp_ncpus/mp_maxid at 1 CPU. LinuxKPI
 * keeps real per-CPU state (tasklets, RCU), so raise them to the real
 * count for this driver's copy of the library, capped at what the DPCPU
 * arrays were sized for.
 */
static void
lkpi_smp_sysinit(void* arg __unused)
{
	int cpus = (int)smp_get_num_cpus();
	int i;

	if (cpus > MAXCPU)
		cpus = MAXCPU;
	mp_ncpus = cpus;
	mp_ncores = cpus;
	mp_maxid = cpus - 1;

	CPU_ZERO(&all_cpus);
	for (i = 0; i < cpus; i++)
		CPU_SET(i, &all_cpus);
}
SYSINIT(lkpi_smp, SI_SUB_CPU, SI_ORDER_FIRST, lkpi_smp_sysinit, NULL);


static void
lkpi_lc_assert_noop(const struct lock_object* lock __unused, int what __unused)
{
}


static int
lkpi_lc_owner_held(const struct lock_object* lock __unused,
	struct thread** owner)
{
	*owner = NULL;
	return 1;
}

/* See haiku/compat/sys/lock.h. */
struct lock_class lkpi_lock_class_noop = {
	.lc_assert = lkpi_lc_assert_noop,
	.lc_owner = lkpi_lc_owner_held,
};


int
kern_kldload(struct thread* td __unused, const char* file __unused,
	int* fileid __unused)
{
	/* No kld on Haiku; request_module() callers treat this as "module
	 * not available", which is accurate. */
	return ENOENT;
}


void
pmap_invalidate_cache(void)
{
	__asm __volatile("wbinvd");
}


void
pmap_force_invalidate_cache_range(vm_offset_t start __unused,
	vm_offset_t end __unused)
{
	/* No clflush without CPUID_CLFSH; flush everything instead. */
	pmap_invalidate_cache();
}


int
pmap_change_attr(vm_offset_t va __unused, vm_size_t size __unused,
	int memattr __unused)
{
	/* Haiku gives drivers no way to change caching attributes of an
	 * existing mapping; fail honestly (nothing on the rtw88 path calls
	 * set_memory_*()). */
	return EINVAL;
}
