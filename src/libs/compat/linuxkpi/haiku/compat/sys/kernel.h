/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_KERNEL_H_
#define _LKPI_HAIKU_SYS_KERNEL_H_

/*
 * Extends freebsd_network's <sys/kernel.h>, whose SYSINIT ignores the
 * subsystem argument entirely. FreeBSD orders by (subsystem, order);
 * LinuxKPI depends on that - module_init() (SI_SUB_OFED_MODINIT, near
 * the end on FreeBSD) declares SI_ORDER_FIRST and must still run after
 * every LinuxKPI subsystem init, or the driver registers against
 * missing zones (first observed as a NULL uma_zalloc in
 * linux_current.c from rtw88's module_init).
 *
 * freebsd_network's runner only executes the six exact order values, so
 * SYSINIT here folds the subsystem into those six buckets, in FreeBSD's
 * subsystem order. The fine-grained order within one subsystem is lost
 * (linker-set order applies); inits with a same-bucket dependency must
 * use different subsystems.
 */

#include_next <sys/kernel.h>


#ifndef SI_ORDER_LAST
#	define SI_ORDER_LAST	SI_ORDER_ANY
#endif

/* subsystem ids; only their relative order matters */
#define SI_SUB_TUNABLES			1
#define SI_SUB_LOCK				3
#define SI_SUB_EVENTHANDLER		4
#define SI_SUB_CPU				5
#define SI_SUB_TASKQ			6
#define SI_SUB_DRIVERS			7
#define SI_SUB_CONFIGURE		16
#define SI_SUB_ROOT_CONF		20	/* module.h derives OFED_MODINIT = -1 */

#define LKPI_SI_BUCKET(subsystem)							\
	((subsystem) <= SI_SUB_LOCK ? SI_ORDER_FIRST			\
	: (subsystem) <= SI_SUB_CPU ? SI_ORDER_SECOND			\
	: (subsystem) <= SI_SUB_TASKQ ? SI_ORDER_THIRD			\
	: (subsystem) <= SI_SUB_DRIVERS ? SI_ORDER_FOURTH		\
	: (subsystem) <= SI_SUB_ROOT_CONF ? SI_ORDER_MIDDLE		\
	: SI_ORDER_ANY)

#undef SYSINIT
#define SYSINIT(uniquifier, subsystem, _order, _func, ident)	\
	static struct sysinit sysinit_##uniquifier = {				\
		.name	= #uniquifier,									\
		.order	= LKPI_SI_BUCKET(subsystem),					\
		.func	= _func,										\
		.arg	= ident,										\
	};															\
	DATA_SET(__freebsd_sysinit, sysinit_##uniquifier)

#undef SYSUNINIT
#define SYSUNINIT(uniquifier, subsystem, _order, _func, ident)	\
	static struct sysinit sysuninit_##uniquifier = {			\
		.name	= #uniquifier,									\
		.order	= LKPI_SI_BUCKET(subsystem),					\
		.func	= _func,										\
		.arg	= ident,										\
	};															\
	DATA_SET(__freebsd_sysuninit, sysuninit_##uniquifier)


#endif /* _LKPI_HAIKU_SYS_KERNEL_H_ */
