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

#include <smp.h>
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


extern "C" void
lkpi_sched_bind(struct thread* td, int cpu)
{
	/*
	 * Haiku gives drivers no way to migrate a thread to a chosen CPU.
	 * linux_rcu.c binds to a reader's CPU only to speed up grace periods
	 * - ck_epoch_synchronize_wait() does the actual waiting - so yield
	 * instead, giving that CPU's threads a chance to run.
	 */
	(void)td;
	(void)cpu;
	thread_yield();
}


extern "C" void
lkpi_sched_unbind(struct thread* td)
{
	(void)td;
}


/* From the kernel's private <vm/vm_page.h>, which this port's FreeBSD
 * <vm/vm_page.h> shadow hides from C++ too; declared directly. */
typedef phys_addr_t page_num_t;
page_num_t vm_page_num_pages(void);
page_num_t vm_page_num_free_pages(void);

extern "C" unsigned int vm_free_count(void);
extern "C" long physmem;


extern "C" unsigned int
vm_free_count(void)
{
	return (unsigned int)vm_page_num_free_pages();
}


extern "C" void
lkpi_vm_counters_init(void)
{
	physmem = (long)vm_page_num_pages();
}


/*
 * FreeBSD's smp_rendezvous(): run action on every CPU and wait for all
 * of them. Haiku's call_all_cpus_sync() does exactly that; the per-CPU
 * setup/teardown hooks wrap the action like on FreeBSD.
 */
struct lkpi_rendezvous_args {
	void (*setup)(void* arg);
	void (*action)(void* arg);
	void (*teardown)(void* arg);
	void* arg;
};

extern "C" void smp_no_rendezvous_barrier(void* arg);
extern "C" void smp_rendezvous(void (*setup)(void* arg),
	void (*action)(void* arg), void (*teardown)(void* arg), void* arg);


extern "C" void
smp_no_rendezvous_barrier(void* arg)
{
	(void)arg;
}


static void
lkpi_rendezvous_trampoline(void* cookie, int cpu)
{
	struct lkpi_rendezvous_args* args = (struct lkpi_rendezvous_args*)cookie;
	(void)cpu;

	if (args->setup != NULL && args->setup != smp_no_rendezvous_barrier)
		args->setup(args->arg);
	if (args->action != NULL)
		args->action(args->arg);
	if (args->teardown != NULL && args->teardown != smp_no_rendezvous_barrier)
		args->teardown(args->arg);
}


extern "C" void
smp_rendezvous(void (*setup)(void* arg), void (*action)(void* arg),
	void (*teardown)(void* arg), void* arg)
{
	struct lkpi_rendezvous_args args = { setup, action, teardown, arg };
	call_all_cpus_sync(lkpi_rendezvous_trampoline, &args);
}
