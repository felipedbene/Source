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
	int				p_flag;			/* P_*; the kernel team never exits */
	sigset_t		p_siglist;
	struct vmspace*	p_vmspace;		/* always NULL: no user address spaces */
};

#define P_WEXIT		0x02000			/* working on exiting (never set) */

/*
 * Every pid LinuxKPI sees here is a Haiku thread id (task->pid = td_tid),
 * so steer linux_current.c's "pid or tid?" split (pid > PID_MAX) to the
 * tdfind() branch for every valid id.
 */
#define PID_MAX		0

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
		u_int		code;		/* syscall number; always 0 here */
		long		args[8];
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

/*
 * The process-table slice of FreeBSD's <sys/proc.h> that linux_current.c
 * uses, on the one kernel proc and the shadow-thread table
 * (lkpi_thread.c). PROC_LOCK is the shadow table's spinlock, so nothing
 * may sleep under it - the FreeBSD code holding it only runs atomics.
 * tdfind() and pfind() return with the proc locked, as on FreeBSD.
 */
void lkpi_proc_lock(struct proc* p);
void lkpi_proc_unlock(struct proc* p);
struct proc* lkpi_allproc_first(void);
struct thread* lkpi_proc_thread_first(struct proc* p);
struct thread* lkpi_proc_thread_next(struct thread* td);
struct thread* tdfind(lwpid_t tid, pid_t pid);
struct proc* pfind(pid_t pid);
void thread_reap_barrier(void);
int linux_alloc_current_noop(struct thread* td, int flags);
void kern_yield(int prio);

/* FreeBSD's per-thread lock, as one global spinlock over all shadow
 * threads (only linux_rcu.c's priority boosting takes it). */
void lkpi_thread_lock(struct thread* td);
void lkpi_thread_unlock(struct thread* td);
#define thread_lock(td)		lkpi_thread_lock(td)
#define thread_unlock(td)	lkpi_thread_unlock(td)

/* Locked around the (one-entry) process list, as FreeBSD's is. */
struct sx;
extern struct sx allproc_lock;
__END_DECLS

#define curproc	(curthread->td_proc)

#define PROC_LOCK(p)			lkpi_proc_lock(p)
#define PROC_UNLOCK(p)			lkpi_proc_unlock(p)
#define PROC_LOCK_ASSERT(p, what)	do { } while (0)

#define FOREACH_THREAD_IN_PROC(p, td)					\
	for ((td) = lkpi_proc_thread_first(p); (td) != NULL;	\
		(td) = lkpi_proc_thread_next(td))

#define FOREACH_PROC_IN_SYSTEM(p)						\
	for ((p) = lkpi_allproc_first(); (p) != NULL; (p) = NULL)

#define THREAD_NO_SLEEPING()	do { } while (0)
#define THREAD_SLEEPING_OK()	do { } while (0)
#define THREAD_CAN_SLEEP()		(lkpi_thread_can_sleep())

typedef void (*thread_dtor_fn)(void*, struct thread*);
EVENTHANDLER_DECLARE(thread_dtor, thread_dtor_fn);


#endif /* _LKPI_HAIKU_SYS_PROC_H_ */
