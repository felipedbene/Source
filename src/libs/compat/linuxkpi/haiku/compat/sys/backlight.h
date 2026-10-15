/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_BACKLIGHT_H_
#define _LKPI_HAIKU_SYS_BACKLIGHT_H_

/*
 * FreeBSD's backlight(9) user interface types, which linux_pci.c's
 * backlight glue fills in. No backlight device is ever registered on
 * this port (lkpi_cdev.c's backlight_register says so).
 */

#include <sys/types.h>


#define BACKLIGHTMAXLEVELS		100
#define BACKLIGHTMAXNAMELENGTH	64

struct backlight_props {
	uint32_t	brightness;
	uint32_t	nlevels;
	uint32_t	levels[BACKLIGHTMAXLEVELS];
};

enum backlight_info_type {
	BACKLIGHT_TYPE_PANEL = 0,
	BACKLIGHT_TYPE_KEYBOARD
};

struct backlight_info {
	char						name[BACKLIGHTMAXNAMELENGTH];
	enum backlight_info_type	type;
};


#endif /* _LKPI_HAIKU_SYS_BACKLIGHT_H_ */
