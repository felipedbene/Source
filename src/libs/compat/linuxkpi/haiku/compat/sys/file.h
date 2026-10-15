/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_FILE_H_
#define _LKPI_HAIKU_SYS_FILE_H_

/*
 * A minimal FreeBSD file(9) <sys/file.h> for LinuxKPI; shadows the POSIX
 * flock() header the include paths would otherwise find. Haiku has no
 * devfs for LinuxKPI char devices, so struct file objects are never
 * actually created on this port - linux/fs.h and linux_compat.c's
 * devfs emulation only need the types to compile, and the wifi path
 * never opens Linux device nodes. The functions become ENODEV stubs
 * (lkpi_cdev.c) if anything ever links them.
 */

#include <sys/types.h>

#include <sys/refcount.h>
#include <sys/selinfo.h>
#include <vm/vm.h>


struct cap_rights;
struct fileops;
struct filedesc;
struct kinfo_file;
struct knote;
struct kaiocb;
struct proc;
struct sigio;
struct stat;
struct thread;
struct ucred;
struct uio;
struct vm_map;
struct vnode;

struct file {
	void*			f_data;
	const struct fileops*	f_ops;
	struct vnode*	f_vnode;
	short			f_type;
	int				f_flag;
	volatile u_int	f_count;
	off_t			f_offset;
};

/* file types (FreeBSD values) */
#define DTYPE_VNODE		1
#define DTYPE_DEV		11

typedef int fo_rdwr_t(struct file* fp, struct uio* uio,
	struct ucred* active_cred, int flags, struct thread* td);
typedef int fo_truncate_t(struct file* fp, off_t length,
	struct ucred* active_cred, struct thread* td);
typedef int fo_ioctl_t(struct file* fp, u_long com, void* data,
	struct ucred* active_cred, struct thread* td);
typedef int fo_poll_t(struct file* fp, int events,
	struct ucred* active_cred, struct thread* td);
typedef int fo_kqfilter_t(struct file* fp, struct knote* kn);
typedef int fo_stat_t(struct file* fp, struct stat* sb,
	struct ucred* active_cred);
typedef int fo_close_t(struct file* fp, struct thread* td);
typedef void fo_fdclose_t(struct file* fp, int fd, struct thread* td);
typedef int fo_chmod_t(struct file* fp, mode_t mode,
	struct ucred* active_cred, struct thread* td);
typedef int fo_chown_t(struct file* fp, uid_t uid, gid_t gid,
	struct ucred* active_cred, struct thread* td);
typedef int fo_sendfile_t(struct file* fp, int sockfd, struct uio* hdr_uio,
	struct uio* trl_uio, off_t offset, size_t nbytes, off_t* sent,
	int flags, struct thread* td);
typedef int fo_seek_t(struct file* fp, off_t offset, int whence,
	struct thread* td);
typedef int fo_fill_kinfo_t(struct file* fp, struct kinfo_file* kif,
	struct filedesc* fdp);
typedef int fo_mmap_t(struct file* fp, struct vm_map* map,
	vm_offset_t* addr, vm_size_t size, vm_prot_t prot,
	vm_prot_t cap_maxprot, int flags, vm_ooffset_t foff,
	struct thread* td);
typedef int fo_aio_queue_t(struct file* fp, struct kaiocb* job);
typedef int fo_add_seals_t(struct file* fp, int flags);
typedef int fo_get_seals_t(struct file* fp, int* flags);
typedef int fo_fallocate_t(struct file* fp, off_t offset, off_t len,
	struct thread* td);
typedef int fo_fspacectl_t(struct file* fp, int cmd, off_t* offset,
	off_t* length, int flags, struct ucred* active_cred,
	struct thread* td);
typedef int fo_cmp_t(struct file* fp, struct file* fp1, struct thread* td);
typedef int fo_fork_t(struct filedesc* fdp, struct file* fp,
	struct file** fp1, struct proc* p1, struct thread* td);
typedef int fo_spare_t(struct file* fp);
typedef int fo_flags_t;

struct fileops {
	fo_rdwr_t*		fo_read;
	fo_rdwr_t*		fo_write;
	fo_truncate_t*	fo_truncate;
	fo_ioctl_t*		fo_ioctl;
	fo_poll_t*		fo_poll;
	fo_kqfilter_t*	fo_kqfilter;
	fo_stat_t*		fo_stat;
	fo_close_t*		fo_close;
	fo_fdclose_t*	fo_fdclose;
	fo_chmod_t*		fo_chmod;
	fo_chown_t*		fo_chown;
	fo_sendfile_t*	fo_sendfile;
	fo_seek_t*		fo_seek;
	fo_fill_kinfo_t*	fo_fill_kinfo;
	fo_mmap_t*		fo_mmap;
	fo_aio_queue_t*	fo_aio_queue;
	fo_add_seals_t*	fo_add_seals;
	fo_get_seals_t*	fo_get_seals;
	fo_fallocate_t*	fo_fallocate;
	fo_fspacectl_t*	fo_fspacectl;
	fo_cmp_t*		fo_cmp;
	fo_fork_t*		fo_fork;
	fo_spare_t*		fo_spares[8];
	fo_flags_t		fo_flags;
};

#define DFLAG_PASSABLE	0x01
#define DFLAG_SEEKABLE	0x02

/* the "invalid operation" implementations (lkpi_cdev.c) */
fo_rdwr_t invfo_rdwr;
fo_truncate_t invfo_truncate;
fo_ioctl_t invfo_ioctl;
fo_poll_t invfo_poll;
fo_kqfilter_t invfo_kqfilter;
fo_chmod_t invfo_chmod;
fo_chown_t invfo_chown;
fo_sendfile_t invfo_sendfile;

/* Implemented (as ENODEV stubs) if something ever links them: there is
 * no Linux fd table on this port. */
int fget(struct thread* td, int fd, struct cap_rights* rights,
	struct file** fpp);
int fget_unlocked(struct thread* td, int fd, struct cap_rights* rights,
	struct file** fpp);
int fdrop(struct file* fp, struct thread* td);
int _fdrop(struct file* fp, struct thread* td);
void finit(struct file* fp, u_int flag, short type, void* data,
	const struct fileops* ops);
int falloc(struct thread* td, struct file** resultfp, int* resultfd,
	int flags);
void fdclose(struct thread* td, struct file* fp, int fd);
int kcmp_cmp(uintptr_t a, uintptr_t b);

/* async-I/O ownership (never signaled here) */
pid_t fgetown(struct sigio** sigiop);
int fsetown(pid_t pgid, struct sigio** sigiop);
void funsetown(struct sigio** sigiop);


#endif /* _LKPI_HAIKU_SYS_FILE_H_ */
