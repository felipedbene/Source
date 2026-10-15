/* SPDX-License-Identifier: MIT */
/*
 * FreeBSD <vm/vm_pager.h> subset for LinuxKPI's shmem/cdev-pager paths;
 * panicking stubs (lkpi_vm.c) - no pagers exist on this port.
 */

#include <vm/vm.h>


/* object types (FreeBSD's enum obj_type values) */
#define OBJT_DEVICE		3
#define OBJT_SG			6
#define OBJT_MGTDEVICE	7

/* pager return codes */
#define VM_PAGER_OK		0
#define VM_PAGER_BAD	1
#define VM_PAGER_FAIL	2
#define VM_PAGER_PEND	3
#define VM_PAGER_ERROR	4
#define VM_PAGER_AGAIN	5

struct ucred;

struct cdev_pager_ops {
	int (*cdev_pg_fault)(vm_object_t object, vm_ooffset_t offset, int prot,
		struct vm_page** mres);
	int (*cdev_pg_populate)(vm_object_t object, vm_pindex_t pidx,
		int fault_type, vm_prot_t max_prot, vm_pindex_t* first,
		vm_pindex_t* last);
	int (*cdev_pg_ctor)(void* handle, vm_ooffset_t size, vm_prot_t prot,
		vm_ooffset_t foff, struct ucred* cred, u_short* color);
	void (*cdev_pg_dtor)(void* handle);
};

void vm_pager_page_unswapped(struct vm_page* page);
vm_object_t vm_pager_allocate(int type, void* handle, vm_ooffset_t size,
	vm_prot_t prot, vm_ooffset_t offset, struct ucred* cred);
vm_object_t cdev_pager_allocate(void* handle, int type,
	struct cdev_pager_ops* ops, vm_ooffset_t size, vm_prot_t prot,
	vm_ooffset_t foff, struct ucred* cred);
vm_object_t cdev_pager_lookup(void* handle);
void cdev_mgtdev_pager_free_pages(vm_object_t object);
