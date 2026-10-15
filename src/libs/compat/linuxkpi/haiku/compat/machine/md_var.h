/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_MACHINE_MD_VAR_H_
#define _LKPI_HAIKU_MACHINE_MD_VAR_H_

/*
 * The FreeBSD/x86 machine-dependent bits LinuxKPI's <linux/page.h> cache
 * flushing uses: the CPUID feature words (FreeBSD's machine/md_var.h) and
 * the CPUID bits for them (machine/specialreg.h). clflush()/clflushopt()
 * come from freebsd_network's <machine/cpufunc.h>.
 *
 * The feature words live in lkpi_machdep.c and are currently 0, so
 * lkpi_clflushopt() falls back to pmap_invalidate_cache(); nothing on the
 * rtw88 path flushes caches.
 */

extern u_int cpu_feature;			/* CPUID leaf 1, %edx */
extern u_int cpu_feature2;			/* CPUID leaf 1, %ecx */
extern u_int cpu_stdext_feature;	/* CPUID leaf 7, %ebx */
extern u_int cpu_id;				/* CPUID leaf 1, %eax */
extern u_int cpu_vendor_id;			/* CPU_VENDOR_*; 0 = unknown */
extern u_int cpu_clflush_line_size;

#define CPUID_TO_FAMILY(id) \
	((((id) >> 8) & 0x0f) + (((id) >> 20) & 0xff))
#define CPUID_TO_MODEL(id) \
	((((id) >> 4) & 0x0f) | (((id) >> 12) & 0xf0))
#define CPUID_TO_STEPPING(id)	((id) & 0x0f)

#define CPUID_PAT				0x00010000
#define CPUID_CLFSH				0x00080000
#define CPUID2_SSE41			0x00080000
#define CPUID2_HV				0x80000000
#define CPUID_STDEXT_CLFLUSHOPT	0x00800000


#endif /* _LKPI_HAIKU_MACHINE_MD_VAR_H_ */
