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

#include <sys/queue.h>
#include <sys/rman.h>


driver_t* device_get_driver(device_t dev);
devclass_t devclass_create(const char* classname);
int device_probe_and_attach(device_t dev);
int devclass_add_driver(devclass_t dc, driver_t* driver, int pass,
	devclass_t* dcp);
int devclass_delete_driver(devclass_t busclass, driver_t* driver);
int bus_translate_resource(device_t dev, int type, rman_res_t start,
	rman_res_t* newstart);

#define BUS_PASS_DEFAULT	(-1)

/*
 * FreeBSD's resource lists; freebsd_network tracks resources per device
 * instead, so lookups find nothing (lkpi_pci.c).
 */
struct resource;

struct resource_list_entry {
	STAILQ_ENTRY(resource_list_entry) link;
	int				type;
	int				rid;
	int				flags;
	struct resource*	res;
	rman_res_t		start;
	rman_res_t		end;
	rman_res_t		count;
};

struct resource_list {
	STAILQ_HEAD(, resource_list_entry) rl_list;
};

struct resource_list_entry* resource_list_find(struct resource_list* rl,
	int type, int rid);

device_t bus_generic_add_child(device_t dev, u_int order, const char* name,
	int unit);

/* NULL on this port: the Linux root device attaches to no newbus parent. */
extern device_t root_bus;

#define bus_topo_lock()		do { } while (0)
#define bus_topo_unlock()	do { } while (0)
#define bus_topo_assert()	do { } while (0)

/* No bus rescan on this port. */
#define BUS_RESCAN(dev)		(ENXIO)


#endif /* _LKPI_HAIKU_SYS_BUS_H_ */
