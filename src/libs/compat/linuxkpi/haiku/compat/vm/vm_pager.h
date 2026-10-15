/* SPDX-License-Identifier: MIT */
/*
 * FreeBSD <vm/vm_pager.h> subset for LinuxKPI's shmem/cdev-pager paths;
 * panicking stubs (lkpi_vm.c) - no pagers exist on this port.
 */

#include <vm/vm.h>

void vm_pager_page_unswapped(struct vm_page* page);
vm_object_t cdev_pager_lookup(void* handle);
void cdev_mgtdev_pager_free_pages(vm_object_t object);
