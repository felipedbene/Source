/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_VNODE_H_
#define _LKPI_HAIKU_SYS_VNODE_H_

/*
 * A minimal FreeBSD <sys/vnode.h> for LinuxKPI, which uses struct vnode
 * as Linux's struct inode (linux/fs.h maps inode = vnode, i_private =
 * v_data). No vnodes exist on this port - the wifi path never opens
 * Linux device nodes - so vget()/vrele() are implemented as ENODEV/no-op
 * stubs if anything ever links them.
 */

struct cdev;
struct stat;
struct ucred;
struct vm_object;

struct mount {
	int	mnt_flag;
};

#define MNT_NOEXEC	0x0004

struct vnode {
	void*				v_data;
	struct vm_object*	v_object;
	struct mount*		v_mount;
	struct cdev*		v_rdev;
	int					v_usecount;
};

/* vn_lock() flags; only passed to the stub below */
#define LK_SHARED	0x01
#define LK_EXCLUSIVE	0x02
#define LK_RETRY	0x04

#define NOCRED	((struct ucred*)0)

int vget(struct vnode* vp, int flags);
void vrele(struct vnode* vp);
void vref(struct vnode* vp);
int vn_lock(struct vnode* vp, int flags);
void VOP_UNLOCK(struct vnode* vp);
int VOP_STAT(struct vnode* vp, struct stat* sb, struct ucred* active_cred,
	struct ucred* file_cred);

struct kinfo_file;
int vn_fill_kinfo_vnode(struct vnode* vp, struct kinfo_file* kif);


#endif /* _LKPI_HAIKU_SYS_VNODE_H_ */
