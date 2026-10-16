/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * The newbus/pci glue linux_pci.c runs on.
 *
 * Registration bridge: module_pci_driver() drivers reach
 * linux_pci_register_driver() from a sysinit, which fills in the
 * driver's bsddriver (name, pci_methods, softc size) and hands it to
 * devclass_add_driver(). freebsd_network's probe pass ran earlier,
 * against the *addon's* own driver_t (see the rtw88 glue), so
 * devclass_add_driver() just records the LinuxKPI driver here and the
 * glue's device methods delegate to it by method id once attach time
 * comes - by then the sysinits have run.
 */

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/bus_dma.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/rman.h>
#include <sys/systm.h>

#include <dev/pci/pcivar.h>
#include <dev/pci/pcireg.h>
#include <dev/pci/pci_private.h>

#include <sys/haiku-module.h>


int pci_enable_aspm = 0;	/* leave ASPM link states alone */

#define LKPI_MAX_PCI_DRIVERS	4

static driver_t* sRegisteredDrivers[LKPI_MAX_PCI_DRIVERS];
static int sRegisteredDriverCount = 0;


int
devclass_add_driver(devclass_t dc __unused, driver_t* driver,
	int pass __unused, devclass_t* dcp __unused)
{
	if (sRegisteredDriverCount >= LKPI_MAX_PCI_DRIVERS)
		return ENOMEM;
	sRegisteredDrivers[sRegisteredDriverCount++] = driver;
	return 0;
}


int
devclass_delete_driver(devclass_t busclass __unused, driver_t* driver)
{
	int i;

	for (i = 0; i < sRegisteredDriverCount; i++) {
		if (sRegisteredDrivers[i] == driver) {
			sRegisteredDrivers[i] =
				sRegisteredDrivers[--sRegisteredDriverCount];
			sRegisteredDrivers[sRegisteredDriverCount] = NULL;
			return 0;
		}
	}
	return ENOENT;
}


static device_method_t*
lkpi_find_method(driver_t* driver, int id)
{
	device_method_t* method;

	for (method = driver->methods; method->name != NULL; method++) {
		if (method->id == id)
			return method;
	}
	return NULL;
}


/*
 * Runs method `id` of the first registered LinuxKPI driver whose probe
 * accepts the device (or of every registered driver, for probe itself).
 * Returns ENXIO when nothing matches.
 */
int
lkpi_pci_dispatch(device_t dev, int id)
{
	int i;

	for (i = 0; i < sRegisteredDriverCount; i++) {
		driver_t* driver = sRegisteredDrivers[i];
		device_method_t* probe =
			lkpi_find_method(driver, ID_device_probe);
		device_method_t* method = lkpi_find_method(driver, id);

		if (probe == NULL || method == NULL)
			continue;
		if (id != ID_device_probe && probe->method(dev) < 0)
			continue;
		return method->method(dev);
	}
	return ENXIO;
}


int
lkpi_pci_registered_driver_count(void)
{
	return sRegisteredDriverCount;
}


static struct resource_list_entry*
lkpi_resource_list_add(struct resource_list* rl, int type, int rid,
	struct resource* res, rman_res_t start, rman_res_t count)
{
	struct resource_list_entry* rle;

	rle = _kernel_malloc(sizeof(*rle), M_ZERO);
	if (rle == NULL)
		return NULL;
	rle->type = type;
	rle->rid = rid;
	rle->res = res;
	rle->start = start;
	rle->end = start + (count != 0 ? count - 1 : 0);
	rle->count = count;
	STAILQ_INSERT_TAIL(&rl->rl_list, rle, link);
	return rle;
}


/*
 * linux_pci.c reads a FreeBSD pci_devinfo through device_get_ivars()
 * (PCIe capability for pci_is_root_bus(), the MSI registers, the BAR
 * resource list, the interrupt line). freebsd_network never sets ivars
 * on PCI devices, so the glue calls this before dispatching attach.
 */
void
lkpi_pci_provide_devinfo(device_t dev)
{
	struct pci_devinfo* dinfo;
	int reg;

	if (device_get_ivars(dev) != NULL)
		return;

	dinfo = _kernel_malloc(sizeof(*dinfo), M_ZERO);
	if (dinfo == NULL)
		return;

	dinfo->dev = dev;
	STAILQ_INIT(&dinfo->resources.rl_list);

	if (pci_find_cap(dev, PCIY_EXPRESS, &reg) == 0) {
		dinfo->cfg.pcie.pcie_location = reg;
		dinfo->cfg.pcie.pcie_type =
			pci_read_config(dev, reg + 2, 2) & 0x00f0;
	}
	if (pci_find_cap(dev, PCIY_MSI, &reg) == 0) {
		dinfo->cfg.msi.msi_location = reg;
		dinfo->cfg.msi.msi_ctrl = pci_read_config(dev, reg + 2, 2);
	}

	/* the legacy interrupt line; linux_pci_attach_device() hands it to
	 * pdev->irq, which request_irq() later resolves */
	lkpi_resource_list_add(&dinfo->resources, SYS_RES_IRQ, 0, NULL,
		pci_read_config(dev, PCIR_INTLINE, 1), 1);

	device_set_ivars(dev, dinfo);
}


devclass_t
devclass_create(const char* classname __unused)
{
	/* only identity matters to the callers */
	static struct { int unused; } sDummyClass;
	return (devclass_t)&sDummyClass;
}


devclass_t
devclass_find(const char* classname)
{
	return devclass_create(classname);
}


device_t
devclass_get_device(devclass_t dc __unused, int unit __unused)
{
	/* no device registry; lookups find nothing */
	return NULL;
}


driver_t*
device_get_driver(device_t dev __unused)
{
	if (sRegisteredDriverCount > 0)
		return sRegisteredDrivers[0];
	return NULL;
}


int
device_probe_and_attach(device_t dev __unused)
{
	/* device_reprobe(): nothing on this port rebinds devices */
	return ENXIO;
}


/* -------------------------------------------------------------------- */
/* PCI config helpers that freebsd_network's pcivar does not cover. */

int
pci_disable_busmaster(device_t dev)
{
	uint16_t command = pci_read_config(dev, PCIR_COMMAND, 2);
	command &= ~PCIM_CMD_BUSMASTEREN;
	pci_write_config(dev, PCIR_COMMAND, command, 2);
	return 0;
}


int
pci_disable_io(device_t dev, int space)
{
	uint16_t command = pci_read_config(dev, PCIR_COMMAND, 2);

	switch (space) {
		case SYS_RES_IOPORT:
			command &= ~PCIM_CMD_PORTEN;
			break;
		case SYS_RES_MEMORY:
			command &= ~PCIM_CMD_MEMEN;
			break;
		default:
			return EINVAL;
	}
	pci_write_config(dev, PCIR_COMMAND, command, 2);
	return 0;
}


uint8_t
pci_get_class(device_t dev)
{
	return pci_read_config(dev, PCIR_CLASS, 1);
}


uint8_t
pci_get_subclass(device_t dev)
{
	return pci_read_config(dev, PCIR_SUBCLASS, 1);
}


int
pci_has_pme(device_t dev __unused, int state __unused)
{
	return 0;
}


int
pci_get_max_payload(device_t dev __unused)
{
	return 0;
}


int
pci_enable_msi(device_t dev __unused)
{
	/* No MSI wiring yet; callers fall back to legacy INTx. */
	return ENODEV;
}


int
pci_enable_msix(device_t dev __unused)
{
	return ENODEV;
}


int
pci_save_state(device_t dev __unused)
{
	return 0;
}


int
pci_restore_state(device_t dev __unused)
{
	return 0;
}


device_t
pci_find_bsf(uint8_t bus __unused, uint8_t slot __unused,
	uint8_t func __unused)
{
	return NULL;
}


/* pci_find_dbsf comes from freebsd_network's pci.cpp */

device_t
pci_find_device(uint16_t vendor __unused, uint16_t device __unused)
{
	return NULL;
}


device_t
pci_find_pcie_root_port(device_t dev __unused)
{
	return NULL;
}


device_t
pci_find_class_from(uint8_t class __unused, uint8_t subclass __unused,
	device_t from __unused)
{
	return NULL;
}


device_t
pci_find_base_class_from(uint8_t class __unused, device_t from __unused)
{
	return NULL;
}


int
lkpi_pci_get_id(device_t bus __unused, device_t dev, int type __unused,
	uintptr_t* rid)
{
	/* Linux devfn: slot in the upper bits, function in the lower 3. */
	*rid = (pci_get_slot(dev) << 3) | pci_get_function(dev);
	return 0;
}


/*
 * BAR bookkeeping: linux_pci.c classifies BARs via pci_find_bar()'s raw
 * value. Reading the config space on each call is cheap and avoids
 * tracking; one map per slot suffices for the immediate use the callers
 * make of it.
 */
struct pci_map*
pci_find_bar(device_t dev, int reg)
{
	static struct pci_map sMaps[8];
	static int sNext = 0;
	struct pci_map* pm;

	pm = &sMaps[sNext];
	sNext = (sNext + 1) % 8;

	pm->pm_reg = reg;
	pm->pm_value = pci_read_config(dev, reg, 4);
	if ((pm->pm_value & PCIM_BAR_SPACE) == PCIM_BAR_MEM_SPACE
			&& (pm->pm_value & PCIM_BAR_MEM_TYPE) == PCIM_BAR_MEM_64) {
		pm->pm_value |=
			(uint64_t)pci_read_config(dev, reg + 4, 4) << 32;
	}
	pm->pm_size = 0;	/* unused by LinuxKPI's callers */
	return pm;
}


/*
 * BAR reservation: freebsd_network's bus_alloc_resource() maps the BAR
 * and stores the mapped address as the bus handle, which is exactly what
 * LinuxKPI's pci_iomap() reads back out of the resource.
 */
struct resource*
pci_reserve_map(device_t dev __unused, device_t child, int type, int rid,
	rman_res_t start __unused, rman_res_t end __unused,
	rman_res_t count __unused, u_int num __unused, u_int flags __unused)
{
	struct pci_devinfo* dinfo = device_get_ivars(child);
	struct resource* res;
	int allocRid = rid;

	res = bus_alloc_resource(child, type, &allocRid, 0, ~0, 1, RF_ACTIVE);
	if (res == NULL || dinfo == NULL)
		return res;

	lkpi_resource_list_add(&dinfo->resources, type, rid, res,
		rman_get_start(res), rman_get_size(res));
	return res;
}


struct resource_list_entry*
resource_list_find(struct resource_list* rl, int type, int rid)
{
	struct resource_list_entry* rle;
	struct pci_devinfo* dinfo;

	if (rl == NULL)
		return NULL;
	STAILQ_FOREACH(rle, &rl->rl_list, link) {
		if (rle->type == type && rle->rid == rid)
			return rle;
	}

	/*
	 * On FreeBSD, pci_alloc_msi() records the allocated vectors as
	 * SYS_RES_IRQ rid 1..n on the devinfo resource list, which LinuxKPI
	 * then reads (_lkpi_pci_enable_msi_range()). freebsd_network's
	 * pci_alloc_msi() keeps the vector in its pci_info instead, where
	 * bus_alloc_resource() hands it out for IRQ rids >= 1 - so create
	 * the missing entry from there on first use. Every resource list on
	 * this port is embedded in a pci_devinfo (lkpi_pci_provide_devinfo),
	 * which the dev backpointer check verifies.
	 */
	if (type == SYS_RES_IRQ && rid >= 1) {
		struct resource* res;
		int allocRid = rid;

		dinfo = (struct pci_devinfo*)((char*)rl
			- offsetof(struct pci_devinfo, resources));
		if (dinfo->dev == NULL || device_get_ivars(dinfo->dev) != dinfo)
			return NULL;

		res = bus_alloc_resource(dinfo->dev, SYS_RES_IRQ, &allocRid,
			0, ~0, 1, RF_ACTIVE | RF_SHAREABLE);
		if (res == NULL)
			return NULL;
		return lkpi_resource_list_add(rl, type, rid, res,
			rman_get_start(res), 1);
	}
	return NULL;
}


int
bus_translate_resource(device_t dev __unused, int type __unused,
	rman_res_t start, rman_res_t* newstart)
{
	*newstart = start;
	return 0;
}


device_t
bus_generic_add_child(device_t dev __unused, u_int order __unused,
	const char* name __unused, int unit __unused)
{
	return NULL;
}


void
bus_identify_children(device_t dev __unused)
{
}


void
bus_attach_children(device_t dev __unused)
{
}


void*
vga_pci_map_bios(device_t dev __unused, size_t* size)
{
	*size = 0;
	return NULL;
}


void
vga_pci_unmap_bios(device_t dev __unused, void* bios __unused)
{
}


/* -------------------------------------------------------------------- */
/* busdma: _bus_dmamap_load_phys() itself lives in freebsd_network's
 * bus_dma.cpp, which can bounce what a device's DMA mask excludes. */

bool
bus_dma_id_mapped(bus_dma_tag_t dmat __unused, vm_paddr_t buf __unused,
	bus_size_t buflen __unused)
{
	/* no IOMMU identity window; always take the mapping path */
	return false;
}
