/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Haiku glue for the LinuxKPI rtw88 driver.
 *
 * Unlike the native FreeBSD wlan drivers, rtw88 registers itself with
 * LinuxKPI from a module_init sysinit (module_pci_driver()), which only
 * runs in _fbsd_init_drivers() - after init_hardware's probe pass. So
 * this glue carries its own driver_t: probe matches the supported PCI
 * ids directly, and the device methods delegate into the LinuxKPI-
 * registered driver by method id (lkpi_pci_dispatch()), which exists by
 * attach time. The softc must be sized like LinuxKPI's own bsddriver
 * sizes it: sizeof(struct pci_dev).
 */

#include <sys/bus.h>
#include <sys/kernel.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

#include <linux/pci.h>


HAIKU_FBSD_WLAN_DRIVERS_GLUE(rtw88)
HAIKU_DRIVER_REQUIREMENTS(FBSD_WLAN | FBSD_FAST_TASKQUEUE);
HAIKU_FIRMWARE_VERSION(0);
HAIKU_FIRMWARE_NAME_MAP({
	{"rtw88/rtw8821c_fw.bin", "rtw8821c_fw.bin"},
});

NO_HAIKU_FBSD_MII_DRIVER();
NO_HAIKU_REENABLE_INTERRUPTS();

/*
 * The hard-irq hook for LinuxKPI interrupts. LinuxKPI installs its
 * handler without a FreeBSD filter, so freebsd_network routes the
 * hardware interrupt here first. The MSI vector is exclusive to this
 * device, and the Linux hard-irq handler (rtw_pci_interrupt_handler,
 * run next from the interrupt thread) masks the chip's interrupt
 * sources itself - so just claim the interrupt.
 */
int
HAIKU_CHECK_DISABLE_INTERRUPTS(device_t dev)
{
	return 1;
}

int lkpi_pci_dispatch(device_t dev, int id);
void lkpi_pci_provide_devinfo(device_t dev);


static const struct {
	uint16_t	vendor;
	uint16_t	device;
} kSupportedDevices[] = {
	{ 0x10ec, 0xc821 },		/* RTL8821CE */
	{ 0x10ec, 0xc82b },		/* RTL8821CE (C82B) */
};


static int
rtw88_probe(device_t dev)
{
	size_t i;

	for (i = 0; i < B_COUNT_OF(kSupportedDevices); i++) {
		if (pci_get_vendor(dev) == kSupportedDevices[i].vendor
				&& pci_get_device(dev) == kSupportedDevices[i].device)
			return BUS_PROBE_DEFAULT;
	}
	return ENXIO;
}


/* rtw88's debug trace mask (module parameter on Linux). */
extern unsigned int rtw_debug_mask;

static int
rtw88_attach(device_t dev)
{
	int error;

	/* XXX bring-up diagnostic: driver tracing without the register
	 * I/O spam (RTW_DBG_IO_RW floods the syslog ring). */
	rtw_debug_mask = 0xffffffff & ~0x10000000;

	lkpi_pci_provide_devinfo(dev);
	error = lkpi_pci_dispatch(dev, ID_device_attach);
	/* XXX bring-up diagnostic: attach result and busmaster state. */
	device_printf(dev, "rtw88 attach: %d (pci cmd 0x%04x)\n", error,
		pci_read_config(dev, PCIR_COMMAND, 2));
	return error;
}


static int
rtw88_detach(device_t dev)
{
	return lkpi_pci_dispatch(dev, ID_device_detach);
}


static int
rtw88_suspend(device_t dev)
{
	return lkpi_pci_dispatch(dev, ID_device_suspend);
}


static int
rtw88_resume(device_t dev)
{
	return lkpi_pci_dispatch(dev, ID_device_resume);
}


static device_method_t rtw88_methods[] = {
	DEVMETHOD(device_probe, rtw88_probe),
	DEVMETHOD(device_attach, rtw88_attach),
	DEVMETHOD(device_detach, rtw88_detach),
	DEVMETHOD(device_suspend, rtw88_suspend),
	DEVMETHOD(device_resume, rtw88_resume),
	DEVMETHOD_END
};

static driver_t rtw88_pci_driver = {
	"rtw88",
	rtw88_methods,
	sizeof(struct pci_dev)
};


void
__haiku_init_hardware(void)
{
	driver_t* drivers[] = {
		&rtw88_pci_driver,
		NULL
	};
	_fbsd_init_hardware_pci(drivers);
}
