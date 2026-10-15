/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_CAPSICUM_H_
#define _LKPI_HAIKU_SYS_CAPSICUM_H_

/*
 * FreeBSD capability rights, as far as LinuxKPI's fd helpers
 * (linux/file.h) name them. No capsicum on Haiku; fd lookups are ENODEV
 * stubs (there is no Linux fd table on this port).
 */

#include <sys/types.h>


typedef struct cap_rights {
	uint64_t	cr_rights[2];
} cap_rights_t;

extern cap_rights_t cap_no_rights;


#endif /* _LKPI_HAIKU_SYS_CAPSICUM_H_ */
