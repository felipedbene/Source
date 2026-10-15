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


#endif /* _LKPI_HAIKU_DEV_PCI_PCI_PRIVATE_H_ */
