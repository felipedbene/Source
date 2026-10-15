/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_DEV_PCI_PCIVAR_H_
#define _LKPI_HAIKU_DEV_PCI_PCIVAR_H_

/*
 * Extends freebsd_network's <dev/pci/pcivar.h> with the calls LinuxKPI's
 * <linux/pci.h> additionally uses. Declarations compile the inlines;
 * implementations come with the linux_pci.c glue (lkpi_pci shim).
 */

#include_next <dev/pci/pcivar.h>


int pci_disable_busmaster(device_t dev);
int pci_save_state(device_t dev);
int pci_restore_state(device_t dev);
device_t pci_find_bsf(uint8_t bus, uint8_t slot, uint8_t func);
device_t pci_find_dbsf(uint32_t domain, uint8_t bus, uint8_t slot,
	uint8_t func);
device_t pci_find_device(uint16_t vendor, uint16_t device);
device_t pci_find_pcie_root_port(device_t dev);
uint8_t pci_get_class(device_t dev);
uint8_t pci_get_subclass(device_t dev);
int pci_get_max_payload(device_t dev);
int pci_has_pme(device_t dev, int state);
int pci_enable_msi(device_t dev);
int pci_enable_msix(device_t dev);

/* FreeBSD tunable gating ASPM fiddling; 0 = leave link states alone. */
extern int pci_enable_aspm;

#ifndef PCIV_INVALID
#	define PCIV_INVALID	0xffff
#endif

/* newer FreeBSD spellings of older names */
#ifndef PCIEM_CTL2_COMP_TIMO_DISABLE
#	define PCIEM_CTL2_COMP_TIMO_DISABLE	PCIEM_CTL2_COMP_TIMEOUT_DIS
#endif
#ifndef PCI_POWERSTATE_D3_HOT
#	define PCI_POWERSTATE_D3_HOT	PCI_POWERSTATE_D3
#endif


#endif /* _LKPI_HAIKU_DEV_PCI_PCIVAR_H_ */
