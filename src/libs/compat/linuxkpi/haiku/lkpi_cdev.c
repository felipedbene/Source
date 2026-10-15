/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Stubs for the devfs/file/select layer linux_compat.c's Linux
 * char-device emulation is built on. Haiku has no devfs for LinuxKPI
 * devices: make_dev_s() fails with ENODEV, so no Linux /dev node, file
 * or knote ever exists on this port, and everything downstream of one is
 * unreachable. The wifi path registers no char devices.
 */

#include <sys/param.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/filio.h>
#include <sys/selinfo.h>
#include <sys/sglist.h>
#include <sys/systm.h>
#include <sys/vnode.h>

#include <vm/vm.h>
#include <vm/vm_map.h>
#include <vm/vm_pager.h>

#include <string.h>


device_t root_bus = NULL;


void
make_dev_args_init(struct make_dev_args* args)
{
	memset(args, 0, sizeof(*args));
}


int
make_dev_s(struct make_dev_args* args __unused, struct cdev** result,
	const char* fmt __unused, ...)
{
	*result = NULL;
	return ENODEV;
}


void
destroy_dev(struct cdev* dev __unused)
{
}


const char*
devtoname(struct cdev* dev)
{
	return dev->si_name;
}


/* No fd table exists behind LinuxKPI on Haiku. */

int
fget(struct thread* td __unused, int fd __unused,
	struct cap_rights* rights __unused, struct file** fpp)
{
	*fpp = NULL;
	return EBADF;
}


int
fget_unlocked(struct thread* td __unused, int fd __unused,
	struct cap_rights* rights __unused, struct file** fpp)
{
	*fpp = NULL;
	return EBADF;
}


int
fdrop(struct file* fp __unused, struct thread* td __unused)
{
	return 0;
}


int
_fdrop(struct file* fp __unused, struct thread* td __unused)
{
	return 0;
}


void
finit(struct file* fp, u_int flag, short type, void* data,
	const struct fileops* ops)
{
	fp->f_flag = (int)flag;
	fp->f_type = type;
	fp->f_data = data;
	fp->f_ops = ops;
}


int
falloc(struct thread* td __unused, struct file** resultfp,
	int* resultfd __unused, int flags __unused)
{
	*resultfp = NULL;
	return ENODEV;
}


void
fdclose(struct thread* td __unused, struct file* fp __unused,
	int fd __unused)
{
}


/* user-memory stores: no user memory sits behind LinuxKPI here */

int
subyte(volatile void* base __unused, int byte __unused)
{
	return -1;
}


int
suword64(volatile void* base __unused, int64_t word __unused)
{
	return -1;
}


int
kcmp_cmp(uintptr_t a, uintptr_t b)
{
	return a == b ? 0 : (a < b ? 1 : 2);
}


void*
fiodgname_buf_get_ptr(void* fgnp __unused, unsigned long com __unused)
{
	return NULL;
}


/* async-I/O ownership: never signaled on this port */

pid_t
fgetown(struct sigio** sigiop __unused)
{
	return 0;
}


int
fsetown(pid_t pgid __unused, struct sigio** sigiop __unused)
{
	return 0;
}


void
funsetown(struct sigio** sigiop __unused)
{
}


/* the "invalid operation" fileops */

int
invfo_truncate(struct file* fp __unused, off_t length __unused,
	struct ucred* active_cred __unused, struct thread* td __unused)
{
	return EINVAL;
}


int
invfo_chmod(struct file* fp __unused, mode_t mode __unused,
	struct ucred* active_cred __unused, struct thread* td __unused)
{
	return EINVAL;
}


int
invfo_chown(struct file* fp __unused, uid_t uid __unused, gid_t gid __unused,
	struct ucred* active_cred __unused, struct thread* td __unused)
{
	return EINVAL;
}


int
invfo_sendfile(struct file* fp __unused, int sockfd __unused,
	struct uio* hdr_uio __unused, struct uio* trl_uio __unused,
	off_t offset __unused, size_t nbytes __unused, off_t* sent __unused,
	int flags __unused, struct thread* td __unused)
{
	return EINVAL;
}


/* select/kqueue: no knote is ever registered */

void
selrecord(struct thread* selector __unused, struct selinfo* sip __unused)
{
}


void
selwakeup(struct selinfo* sip __unused)
{
}


void
seldrain(struct selinfo* sip __unused)
{
}


void
knlist_init(struct knlist* knl __unused, void* lock __unused,
	void (*kl_lock)(void*) __unused, void (*kl_unlock)(void*) __unused,
	void (*kl_assert_lock)(void*, int) __unused)
{
}


void
knlist_add(struct knlist* knl __unused, struct knote* kn __unused,
	int islocked __unused)
{
}


void
knlist_remove(struct knlist* knl __unused, struct knote* kn __unused,
	int islocked __unused)
{
}


void
knlist_destroy(struct knlist* knl __unused)
{
}


void
knlist_clear(struct knlist* knl __unused, int islocked __unused)
{
}


int
knlist_empty(struct knlist* knl __unused)
{
	return 1;
}


int
knote_triv_copy(struct knote* kn __unused)
{
	return 0;
}


/* vnodes: none exist behind LinuxKPI */

int
vget(struct vnode* vp __unused, int flags __unused)
{
	return ENOENT;
}


void
vrele(struct vnode* vp __unused)
{
}


void
vref(struct vnode* vp __unused)
{
}


int
vn_lock(struct vnode* vp __unused, int flags __unused)
{
	return ENOENT;
}


void
VOP_UNLOCK(struct vnode* vp __unused)
{
}


int
VOP_STAT(struct vnode* vp __unused, struct stat* sb __unused,
	struct ucred* active_cred __unused, struct ucred* file_cred __unused)
{
	return ENOTSUP;
}


int
vn_fill_kinfo_vnode(struct vnode* vp __unused,
	struct kinfo_file* kif __unused)
{
	return ENOTSUP;
}


/* mmap plumbing: unreachable without a device file */

struct sglist*
sglist_alloc(int nsegs __unused, int mflags __unused)
{
	panic("linuxkpi: %s: not supported on Haiku", __func__);
	return NULL;
}


int
sglist_append_phys(struct sglist* sg __unused, vm_paddr_t paddr __unused,
	size_t len __unused)
{
	panic("linuxkpi: %s: not supported on Haiku", __func__);
	return ENOTSUP;
}


void
sglist_free(struct sglist* sg __unused)
{
}


vm_object_t
vm_pager_allocate(int type __unused, void* handle __unused,
	vm_ooffset_t size __unused, vm_prot_t prot __unused,
	vm_ooffset_t offset __unused, struct ucred* cred __unused)
{
	return NULL;
}


vm_object_t
cdev_pager_allocate(void* handle __unused, int type __unused,
	struct cdev_pager_ops* ops __unused, vm_ooffset_t size __unused,
	vm_prot_t prot __unused, vm_ooffset_t foff __unused,
	struct ucred* cred __unused)
{
	return NULL;
}


int
vm_mmap_object(struct vm_map* map __unused, vm_offset_t* addr __unused,
	vm_size_t size __unused, vm_prot_t prot __unused,
	vm_prot_t maxprot __unused, int flags __unused,
	vm_object_t object __unused, vm_ooffset_t foff __unused,
	int writecounted __unused, struct thread* td __unused)
{
	return ENODEV;
}
