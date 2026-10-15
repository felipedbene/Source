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
#include <sys/jail.h>
#include <sys/kernel.h>
#include <sys/libkern.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/smp.h>
#include <sys/syscallsubr.h>
#include <sys/systm.h>

#include <machine/md_var.h>
#include <vm/pmap.h>

#include <stdarg.h>
#include <string.h>


/*
 * Left 0: lkpi_clflushopt() then falls back to pmap_invalidate_cache(),
 * which is always correct. Fill these from CPUID if a driver turns out to
 * flush on a hot path (nothing on the rtw88 path does).
 */
u_int cpu_feature = 0;
u_int cpu_feature2 = 0;
u_int cpu_stdext_feature = 0;
u_int cpu_id = 0;
u_int cpu_vendor_id = 0;		/* unknown: vendor paths are skipped */
u_int cpu_clflush_line_size = 64;

char osrelease[OSRELEASELEN] = "RenkuOS";

/*
 * Like freebsd_network's mp_ncpus (smp.c), a fixed conservative value;
 * only LinuxKPI's topology_num_cores_per_package() reads it.
 */
int mp_ncores = 1;

/*
 * Total physical memory in pages; si_meminfo() (linux_page.c) reports
 * it. Filled by lkpi_vm_counters_init() (lkpi_sched.cpp - the counter
 * lives in the C++-only kernel vm headers) from lkpi_smp_sysinit().
 */
long physmem = 0;

void lkpi_vm_counters_init(void);

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

	lkpi_vm_counters_init();
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


/*
 * FreeBSD's reentrant kernel qsort; Haiku's kernel has no qsort_r
 * export, so sort here (plain recursive quicksort - the only user is
 * list_sort(), on short wait lists).
 */
static void
lkpi_qsort_r(char* base, size_t nmemb, size_t size,
	int (*compar)(const void*, const void*, void*), void* thunk,
	char* swapbuf)
{
	size_t pivot, i, store;

	while (nmemb > 1) {
		pivot = nmemb / 2;

		memcpy(swapbuf, base + pivot * size, size);
		memcpy(base + pivot * size, base + (nmemb - 1) * size, size);
		memcpy(base + (nmemb - 1) * size, swapbuf, size);

		store = 0;
		for (i = 0; i < nmemb - 1; i++) {
			if (compar(base + i * size, base + (nmemb - 1) * size,
					thunk) < 0) {
				if (i != store) {
					memcpy(swapbuf, base + i * size, size);
					memcpy(base + i * size, base + store * size, size);
					memcpy(base + store * size, swapbuf, size);
				}
				store++;
			}
		}
		memcpy(swapbuf, base + store * size, size);
		memcpy(base + store * size, base + (nmemb - 1) * size, size);
		memcpy(base + (nmemb - 1) * size, swapbuf, size);

		/* recurse into the smaller half, loop on the larger */
		if (store < nmemb - store - 1) {
			lkpi_qsort_r(base, store, size, compar, thunk, swapbuf);
			base += (store + 1) * size;
			nmemb -= store + 1;
		} else {
			lkpi_qsort_r(base + (store + 1) * size, nmemb - store - 1,
				size, compar, thunk, swapbuf);
			nmemb = store;
		}
	}
}


void
qsort_r(void* base, size_t nmemb, size_t size,
	int (*compar)(const void*, const void*, void*), void* thunk)
{
	char stackbuf[64];
	char* swapbuf = stackbuf;

	if (size > sizeof(stackbuf)) {
		swapbuf = _kernel_malloc(size, 0);
		if (swapbuf == NULL)
			panic("qsort_r: element too large");
	}
	lkpi_qsort_r(base, nmemb, size, compar, thunk, swapbuf);
	if (swapbuf != stackbuf)
		_kernel_free(swapbuf);
}


unsigned char
bcd2bin(unsigned char bcd)
{
	return (bcd >> 4) * 10 + (bcd & 0x0f);
}


unsigned int
bitcount(unsigned int value)
{
	return (unsigned int)__builtin_popcount(value);
}


int
_intr_drain(int irq __unused)
{
	/* freebsd_network's bus_teardown_intr already synchronizes with a
	 * running handler; nothing further to drain here. */
	return 0;
}


void
vlog(int level __unused, const char* fmt, va_list ap)
{
	driver_vprintf(fmt, ap);
}


/*
 * kernel_fpu_begin()/end() funnel through this. Haiku kernel threads may
 * not touch the FPU without saving state; nothing on the rtw88 path does
 * FPU math, so run the callback directly and let any violation surface
 * as the kernel's own FPU fault rather than silent corruption.
 */
void
lkpi_fpu_safe_exec(void (*func)(void*), void* ctx)
{
	func(ctx);
}


/* MSI interrupt-vector window: none is wired up yet. */
int first_msi_irq = 0;
int num_msi_irqs = 0;


char*
lkpi_strdup(const char* str)
{
	size_t size = strlen(str) + 1;
	char* copy = _kernel_malloc(size, 0);
	if (copy != NULL)
		memcpy(copy, str, size);
	return copy;
}


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
