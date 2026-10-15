/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SOCKETVAR_H_
#define _LKPI_HAIKU_SYS_SOCKETVAR_H_

/*
 * The slice of FreeBSD's <sys/socketvar.h> that <linux/net.h>'s kernel-
 * socket inlines touch. No kernel sockets exist on this port; nothing on
 * the wifi path creates one, so these stay declarations until something
 * links them.
 */

#include <sys/socket.h>


struct sockaddr;
struct thread;
struct ucred;

struct socket {
	int		so_state;
	struct {
		int (*pr_peeraddr)(struct socket* so, struct sockaddr* sa);
		int (*pr_sockaddr)(struct socket* so, struct sockaddr* sa);
	}*		so_proto;
};

#define SS_ISCONNECTED	0x0002

int socreate(int dom, struct socket** aso, int type, int proto,
	struct ucred* cred, struct thread* td);
int soclose(struct socket* so);


#endif /* _LKPI_HAIKU_SYS_SOCKETVAR_H_ */
