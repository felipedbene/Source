/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * FreeBSD <sys/sched.h> calls LinuxKPI uses, on the kernel's private
 * <thread.h>, which is C++-only. sched_pin() prevents migration off the
 * current CPU, exactly like Haiku's pinned_to_cpu count.
 */

#include <thread.h>


struct thread;

extern "C" void lkpi_sched_pin(void);
extern "C" void lkpi_sched_unpin(void);
extern "C" void lkpi_sched_relinquish(struct thread* td);


extern "C" void
lkpi_sched_pin(void)
{
	thread_pin_to_current_cpu(thread_get_current_thread());
}


extern "C" void
lkpi_sched_unpin(void)
{
	thread_unpin_from_current_cpu(thread_get_current_thread());
}


extern "C" void
lkpi_sched_relinquish(struct thread* td)
{
	/* td is always curthread; LinuxKPI uses this for cond_resched(). */
	(void)td;
	thread_yield();
}
