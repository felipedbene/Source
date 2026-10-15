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
#include <sys/lock.h>
#include <sys/smp.h>
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
