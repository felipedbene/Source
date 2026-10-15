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
/* From the kernel's private <smp.h>, which can only be included from C++. */
int32 smp_get_current_cpu(void);
__END_DECLS

#define curthread (lkpi_curthread())

/* freebsd_network's <sys/smp.h> pins curcpu to 0; LinuxKPI keeps real
 * per-CPU state (RCU, idr caches), so it needs the real CPU index. */
static inline int
lkpi_pcpu_get_cpuid(void)
{
	return (int)smp_get_current_cpu();
}

#define PCPU_GET(member)	lkpi_pcpu_get_##member()

#undef curcpu
#define curcpu	(lkpi_pcpu_get_cpuid())

/*
 * FreeBSD's dynamic per-CPU storage, as plain per-TU arrays (LinuxKPI
 * only uses the _STATIC form). MAXCPU comes via <sys/smp.h> above;
 * lkpi_machdep.c raises mp_maxid to the real CPU count at load.
 */
#define DPCPU_DEFINE_STATIC(type, name) \
	static type pcpu_entry_##name[MAXCPU]
#define DPCPU_ID_PTR(cpu, name)	(&pcpu_entry_##name[cpu])
#define DPCPU_ID_GET(cpu, name)	(pcpu_entry_##name[cpu])
#define DPCPU_PTR(name)			DPCPU_ID_PTR(curcpu, name)
#define DPCPU_GET(name)			DPCPU_ID_GET(curcpu, name)


#endif /* _LKPI_HAIKU_SYS_PCPU_H_ */
