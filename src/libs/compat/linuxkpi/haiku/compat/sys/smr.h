/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SMR_H_
#define _LKPI_HAIKU_SYS_SMR_H_

/*
 * No SMR on Haiku (see <sys/_smr.h>). Read sections must never be entered;
 * the panic makes any future user of pctrie's *_unlocked() paths obvious.
 */

#include <sys/_smr.h>
#include <sys/smr_types.h>
#include <sys/systm.h>


static __inline void
smr_enter(smr_t smr __unused)
{
	panic("smr_enter: SMR is not supported");
}


static __inline void
smr_exit(smr_t smr __unused)
{
}


#endif /* _LKPI_HAIKU_SYS_SMR_H_ */
