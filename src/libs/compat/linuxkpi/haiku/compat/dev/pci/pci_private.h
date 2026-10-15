/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_DEV_PCI_PCI_PRIVATE_H_
#define _LKPI_HAIKU_DEV_PCI_PCI_PRIVATE_H_

/*
 * The pieces of FreeBSD's <dev/pci/pci_private.h> (and pcivar BAR
 * bookkeeping) that LinuxKPI's <linux/pci.h> inlines touch.
 */

#include <sys/bus.h>


/* Every device LinuxKPI sees through this port is a PCI device. */
static inline bool
is_pci_device(device_t dev)
{
	return true;
}

typedef uint64_t pci_addr_t;

struct pci_map {
	pci_addr_t	pm_value;	/* raw BAR value */
	pci_addr_t	pm_size;
	int			pm_reg;
};

/* Implemented with the rest of the linux_pci.c glue (lkpi_pci shim). */
struct pci_map* pci_find_bar(device_t dev, int reg);
struct resource* pci_reserve_map(device_t dev, device_t child, int type,
	int rid, rman_res_t start, rman_res_t end, rman_res_t count,
	u_int num, u_int flags);

/*
 * The config-space bookkeeping linux_pci.c reads via device_get_ivars().
 * freebsd_network stores no pci_devinfo behind ivars, so the MSI-descriptor
 * path must not run until the lkpi_pci shim provides one.
 */
struct pcicfg_msi {
	uint16_t	msi_ctrl;
	uint8_t		msi_location;
	uint16_t	msi_data;
	uint64_t	msi_addr;
	int			msi_alloc;
};

struct pcicfg_pcie {
	uint8_t		pcie_location;	/* 0: not PCI Express */
	uint8_t		pcie_type;
};

struct pcicfg {
	struct pcicfg_msi	msi;
	struct pcicfg_pcie	pcie;
};

struct pci_devinfo {
	device_t				dev;
	struct pcicfg			cfg;
	struct resource_list	resources;
};

device_t pci_find_class_from(uint8_t class, uint8_t subclass,
	device_t from);
device_t pci_find_base_class_from(uint8_t class, device_t from);
int pci_disable_io(device_t dev, int space);

/*
 * The PCI_GET_ID kobj method: translate a device to its bus id (RID).
 * freebsd_network has no kobj bus methods; the shim derives the Linux
 * devfn from the slot/function instead.
 */
#define PCI_ID_RID	1
int lkpi_pci_get_id(device_t bus, device_t dev, int type, uintptr_t* rid);
#define PCI_GET_ID(bus, dev, type, ridp) \
	lkpi_pci_get_id((bus), (dev), (type), (ridp))

void* vga_pci_map_bios(device_t dev, size_t* size);
void vga_pci_unmap_bios(device_t dev, void* bios);


#endif /* _LKPI_HAIKU_DEV_PCI_PCI_PRIVATE_H_ */
