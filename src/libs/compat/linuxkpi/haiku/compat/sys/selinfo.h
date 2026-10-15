/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SELINFO_H_
#define _LKPI_HAIKU_SYS_SELINFO_H_

/*
 * FreeBSD's select/kqueue plumbing types, as far as LinuxKPI's poll
 * emulation (linux/fs.h, linux_compat.c) needs them to compile. There is
 * no kqueue on Haiku; the functions become no-op stubs when
 * linux_compat.c is enabled - the wifi path never polls a Linux device
 * node.
 */

#include <sys/event.h>	/* freebsd_network's struct knlist */

struct knote;
struct thread;

struct selinfo {
	struct knlist	si_note;
};

void selrecord(struct thread* selector, struct selinfo* sip);
void selwakeup(struct selinfo* sip);
void seldrain(struct selinfo* sip);

void knlist_init(struct knlist* knl, void* lock,
	void (*kl_lock)(void*), void (*kl_unlock)(void*),
	void (*kl_assert_lock)(void*, int));
void knlist_add(struct knlist* knl, struct knote* kn, int islocked);
void knlist_remove(struct knlist* knl, struct knote* kn, int islocked);
void knlist_destroy(struct knlist* knl);
void knlist_clear(struct knlist* knl, int islocked);
int knlist_empty(struct knlist* knl);

#define KNOTE_LOCKED(knl, hint)		do { } while (0)
#define KNOTE_UNLOCKED(knl, hint)	do { } while (0)


#endif /* _LKPI_HAIKU_SYS_SELINFO_H_ */
