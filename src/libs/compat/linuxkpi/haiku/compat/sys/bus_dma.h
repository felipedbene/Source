/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_BUS_DMA_H_
#define _LKPI_HAIKU_SYS_BUS_DMA_H_

/*
 * Extends freebsd_network's <sys/bus_dma.h> with the newer busdma calls
 * linux_pci.c's DMA mapping uses; implemented in the lkpi_pci shim (the
 * physical-address load is load-bearing for every DMA mapping the
 * driver makes).
 */

#include_next <sys/bus_dma.h>


bool bus_dma_id_mapped(bus_dma_tag_t dmat, vm_paddr_t buf,
	bus_size_t buflen);
int _bus_dmamap_load_phys(bus_dma_tag_t dmat, bus_dmamap_t map,
	vm_paddr_t buf, bus_size_t buflen, int flags,
	bus_dma_segment_t* segs, int* segp);


#endif /* _LKPI_HAIKU_SYS_BUS_DMA_H_ */
