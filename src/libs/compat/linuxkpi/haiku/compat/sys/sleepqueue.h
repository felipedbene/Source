/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SLEEPQUEUE_H_
#define _LKPI_HAIKU_SYS_SLEEPQUEUE_H_

/*
 * The subset of FreeBSD's <sys/sleepqueue.h> that LinuxKPI uses
 * (linux_schedule.c, linux_compat.c). sleepq_set_timeout() takes ticks
 * directly; FreeBSD's sbintime_t variant is not provided.
 */

struct lock_object;

/* sleepq_add() flags */
#define SLEEPQ_SLEEP			0x00
#define SLEEPQ_CONDVAR			0x01
#define SLEEPQ_TYPE				0xff
#define SLEEPQ_INTERRUPTIBLE	0x100

void sleepq_lock(const void* wchan);
void sleepq_release(const void* wchan);
void sleepq_add(const void* wchan, struct lock_object* lock,
	const char* wmesg, int flags, int queue);
void sleepq_set_timeout(const void* wchan, int timo);
void sleepq_wait(const void* wchan, int pri);
int sleepq_wait_sig(const void* wchan, int pri);
int sleepq_timedwait(const void* wchan, int pri);
int sleepq_timedwait_sig(const void* wchan, int pri);
int sleepq_signal(const void* wchan, int flags, int pri, int queue);
int sleepq_broadcast(const void* wchan, int flags, int pri, int queue);


#endif /* _LKPI_HAIKU_SYS_SLEEPQUEUE_H_ */
