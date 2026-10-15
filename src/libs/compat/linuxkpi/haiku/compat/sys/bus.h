/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_BUS_H_
#define _LKPI_HAIKU_SYS_BUS_H_

/*
 * Extends freebsd_network's <sys/bus.h> with the newbus calls LinuxKPI's
 * <linux/device.h> uses. Declarations compile the inlines; the functions
 * are implemented (or stubbed) as the files that link them are brought
 * up. The bus topology lock has no Haiku equivalent for drivers; LinuxKPI
 * only brackets detach with it.
 */

#include_next <sys/bus.h>


driver_t* device_get_driver(device_t dev);
devclass_t devclass_create(const char* classname);
int device_probe_and_attach(device_t dev);

#define bus_topo_lock()		do { } while (0)
#define bus_topo_unlock()	do { } while (0)
#define bus_topo_assert()	do { } while (0)

/* No bus rescan on this port. */
#define BUS_RESCAN(dev)		(ENXIO)


#endif /* _LKPI_HAIKU_SYS_BUS_H_ */
