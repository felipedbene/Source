/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_METHOD_IDS_H_
#define _LKPI_HAIKU_SYS_METHOD_IDS_H_

/*
 * Extends freebsd_network's newbus-lite method ids with the DEVMETHODs
 * linux_pci.c declares. freebsd_network's dispatcher never looks these
 * up; the values only need to stay clear of its enum.
 */

#include_next <sys/method-ids.h>


#define ID_bus_add_child			1001
#define ID_backlight_update_status	1002
#define ID_backlight_get_status		1003
#define ID_backlight_get_info		1004
#define ID_pci_iov_init				1005
#define ID_pci_iov_uninit			1006
#define ID_pci_iov_add_vf			1007


#endif /* _LKPI_HAIKU_SYS_METHOD_IDS_H_ */
