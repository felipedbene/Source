/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_NET_IF_VAR_H_
#define _LKPI_HAIKU_NET_IF_VAR_H_

/*
 * Extends freebsd_network's <net/if_var.h> with the accessors newer
 * FreeBSD code uses, and makes the net epoch types available (LinuxKPI
 * uses them without including <sys/epoch.h> itself).
 */

#include_next <net/if_var.h>

#include <sys/epoch.h>


static inline if_transmit_fn_t
if_gettransmitfn(if_t ifp)
{
	return ifp->if_transmit;
}


#endif /* _LKPI_HAIKU_NET_IF_VAR_H_ */
