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

struct vm_object;

struct vnode {
	void*				v_data;
	struct vm_object*	v_object;
	int					v_usecount;
};

int vget(struct vnode* vp, int flags);
void vrele(struct vnode* vp);
void vref(struct vnode* vp);


#endif /* _LKPI_HAIKU_SYS_VNODE_H_ */
