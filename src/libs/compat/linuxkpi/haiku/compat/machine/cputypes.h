/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_MACHINE_CPUTYPES_H_
#define _LKPI_HAIKU_MACHINE_CPUTYPES_H_

/*
 * FreeBSD/x86 CPU vendor ids. cpu_vendor_id stays 0 here (lkpi_machdep.c)
 * so vendor-specific paths are skipped.
 */

#define CPU_VENDOR_INTEL		1
#define CPU_VENDOR_AMD			2
#define CPU_VENDOR_CYRIX		3
#define CPU_VENDOR_UMC			4
#define CPU_VENDOR_CENTAUR		5
#define CPU_VENDOR_TRANSMETA	6
#define CPU_VENDOR_NSC			7
#define CPU_VENDOR_HYGON		8

#endif /* _LKPI_HAIKU_MACHINE_CPUTYPES_H_ */
