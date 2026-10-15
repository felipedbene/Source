/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SYSLOG_H_
#define _LKPI_HAIKU_SYS_SYSLOG_H_

/*
 * Extends freebsd_network's <sys/syslog.h> (LOG_ERR, LOG_WARNING) with
 * the remaining BSD log priorities, which LinuxKPI's pr_*() use.
 */

#include_next <sys/syslog.h>


#define LOG_EMERG	0	/* system is unusable */
#define LOG_ALERT	1	/* action must be taken immediately */
#define LOG_CRIT	2	/* critical conditions */
#define LOG_NOTICE	5	/* normal but significant condition */
#define LOG_INFO	6	/* informational */
#define LOG_DEBUG	7	/* debug-level messages */


#endif /* _LKPI_HAIKU_SYS_SYSLOG_H_ */
