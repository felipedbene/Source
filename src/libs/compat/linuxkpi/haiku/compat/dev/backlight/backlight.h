/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_DEV_BACKLIGHT_BACKLIGHT_H_
#define _LKPI_HAIKU_DEV_BACKLIGHT_BACKLIGHT_H_

/*
 * FreeBSD's backlight(9) driver API; registration always fails here
 * (lkpi_cdev.c) - no backlight devices behind LinuxKPI on this port.
 */

#include <sys/backlight.h>
#include <sys/bus.h>


struct cdev;

struct cdev* backlight_register(const char* name, device_t dev);
int backlight_destroy(struct cdev* dev);
void backlight_schedule_update_status(struct cdev* dev);


#endif /* _LKPI_HAIKU_DEV_BACKLIGHT_BACKLIGHT_H_ */
