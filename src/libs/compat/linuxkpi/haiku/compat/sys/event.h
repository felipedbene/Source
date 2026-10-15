/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_EVENT_H_
#define _LKPI_HAIKU_SYS_EVENT_H_

/*
 * Extends freebsd_network's <sys/event.h> (struct knlist) with the kqueue
 * types LinuxKPI's poll emulation compiles against. There is no kqueue on
 * Haiku; no knote is ever registered, so the filterops are never called.
 */

#include_next <sys/event.h>


#define EVFILT_READ		(-1)
#define EVFILT_WRITE	(-2)

struct knote;

struct filterops {
	int		f_isfd;
	int		(*f_attach)(struct knote* kn);
	void	(*f_detach)(struct knote* kn);
	int		(*f_event)(struct knote* kn, long hint);
	int		(*f_copy)(struct knote* kn);
};

struct knote {
	int			kn_filter;
	const struct filterops*	kn_fop;
	void*		kn_hook;
};

/* FreeBSD's trivial f_copy for knotes without special state. */
int knote_triv_copy(struct knote* kn);


#endif /* _LKPI_HAIKU_SYS_EVENT_H_ */
