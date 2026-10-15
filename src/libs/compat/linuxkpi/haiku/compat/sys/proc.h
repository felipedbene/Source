/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_PROC_H_
#define _LKPI_HAIKU_SYS_PROC_H_

/*
 * Shadow struct thread/proc for LinuxKPI. Only the fields LinuxKPI touches are
 * present; all of them are zero unless noted. One shadow exists per Haiku
 * thread that enters LinuxKPI, keyed by thread_id (see lkpi_thread.c).
 */

#include <sys/param.h>
#include <sys/pcpu.h>
#include <sys/queue.h>
#include <sys/eventhandler.h>

#include <signal.h>


struct task_struct;
struct vmspace;

typedef int32_t lwpid_t;

struct ucred {
	uid_t			cr_uid;
};

struct proc {
	pid_t			p_pid;			/* Haiku team id of the kernel team */
	sigset_t		p_siglist;
	struct vmspace*	p_vmspace;		/* always NULL: no user address spaces */
};

struct thread {
	lwpid_t			td_tid;			/* Haiku thread_id */
	struct proc*	td_proc;		/* shared kernel proc */
	struct ucred*	td_ucred;		/* shared root cred */
	char			td_name[B_OS_NAME_LENGTH];
	int				td_pflags;
	u_char			td_priority;
	int				td_pinned;
	int				td_critnest;
	int				td_intr_nesting_level;
	int				td_inhibitors;
	int				td_owepreempt;
	int				td_ast_pending;
	sigset_t		td_siglist;
	sigset_t		td_sigmask;
	struct {
		sigset_t	ksi_dummy;
	}				td_sa;
	void*			td_fpop;
	struct task_struct*	td_lkpi_task;

	/* private to lkpi_thread.c */
	SLIST_ENTRY(thread)	td_hash_link;
};

#define TDP_NOFAULTING	0x00000001
#define TDP_ITHREAD		0x00000002
#define TDP_KTHREAD		0x00000004

/* FreeBSD AST flags; Haiku gives drivers no pending-reschedule signal, so
 * td_ast_pending() is never true and need_resched() stays false. */
#define TDA_SCHED		0

static inline bool
td_ast_pending(struct thread* td, int ast)
{
	return false;
}

__BEGIN_DECLS
bool lkpi_thread_can_sleep(void);

/* LinuxKPI's hook for lazily creating "current"; linux_current.c sets it
 * (FreeBSD declares it in its base sys/proc.h). Defined in lkpi_thread.c. */
extern int (*lkpi_alloc_current)(struct thread* td, int flags);
__END_DECLS

#define THREAD_NO_SLEEPING()	do { } while (0)
#define THREAD_SLEEPING_OK()	do { } while (0)
#define THREAD_CAN_SLEEP()		(lkpi_thread_can_sleep())

typedef void (*thread_dtor_fn)(void*, struct thread*);
EVENTHANDLER_DECLARE(thread_dtor, thread_dtor_fn);


#endif /* _LKPI_HAIKU_SYS_PROC_H_ */
