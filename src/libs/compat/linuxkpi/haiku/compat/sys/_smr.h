/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS__SMR_H_
#define _LKPI_HAIKU_SYS__SMR_H_

/*
 * Haiku has no SMR (safe memory reclamation). It is only needed for pctrie's
 * lock-free lookups, which LinuxKPI does not use: its pctries are always
 * accessed under a lock. These types let pctrie compile; see <sys/smr.h>.
 */

#include <sys/types.h>


typedef uint32_t		smr_seq_t;
typedef int32_t			smr_delta_t;
typedef struct smr*		smr_t;

#define SMR_ENTERED(smr)			(false)
#define SMR_ASSERT_ENTERED(smr)		do { } while (0)
#define SMR_ASSERT_NOT_ENTERED(smr)	do { } while (0)
#define SMR_ASSERT(ex, fn)			do { } while (0)


#endif /* _LKPI_HAIKU_SYS__SMR_H_ */
