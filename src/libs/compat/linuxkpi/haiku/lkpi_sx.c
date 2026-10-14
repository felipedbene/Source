/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

/*
 * Shared/exclusive locks for LinuxKPI, see <sys/sx.h>.
 *
 * Writers are preferred: once a writer waits, new readers block. That matches
 * Linux rw_semaphore semantics, which is what LinuxKPI uses sx for.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/sx.h>


void
sx_init_flags(struct sx* sx, const char* description, int opts)
{
	sx->sx_lock = 0;
	sx->sx_flags = opts;
	sx->sx_recurse = 0;
	sx->sx_readers = 0;
	sx->sx_waiters = 0;
	sx->sx_xwaiters = 0;
	mtx_init(&sx->sx_mtx, description, NULL, MTX_DEF);
	cv_init(&sx->sx_cv, description);
}


void
sx_destroy(struct sx* sx)
{
	KASSERT(sx->sx_lock == 0 && sx->sx_readers == 0,
		("sx_destroy: %p still locked", sx));
	cv_destroy(&sx->sx_cv);
	mtx_destroy(&sx->sx_mtx);
}


/* Waits on the condition variable. Returns EINTR if interrupted. */
static int
sx_wait_locked(struct sx* sx, bool interruptible)
{
	int error = 0;

	sx->sx_waiters++;
	if (interruptible)
		error = cv_wait_sig(&sx->sx_cv, &sx->sx_mtx);
	else
		cv_wait(&sx->sx_cv, &sx->sx_mtx);
	sx->sx_waiters--;

	return error != 0 ? EINTR : 0;
}


int
lkpi_sx_xlock(struct sx* sx, bool interruptible)
{
	struct thread* td = curthread;
	int error = 0;

	mtx_lock(&sx->sx_mtx);
	if (sx->sx_lock == (uintptr_t)td) {
		if ((sx->sx_flags & SX_RECURSE) == 0)
			panic("sx_xlock: recursing on non-recursive lock %p", sx);
		sx->sx_recurse++;
		mtx_unlock(&sx->sx_mtx);
		return 0;
	}

	sx->sx_xwaiters++;
	while (sx->sx_lock != 0 || sx->sx_readers > 0) {
		error = sx_wait_locked(sx, interruptible);
		if (error != 0)
			break;
	}
	sx->sx_xwaiters--;

	if (error == 0)
		sx->sx_lock = (uintptr_t)td;
	else if (sx->sx_xwaiters == 0 && sx->sx_waiters > 0) {
		// We may have been the writer readers were blocked on.
		cv_broadcast(&sx->sx_cv);
	}
	mtx_unlock(&sx->sx_mtx);
	return error;
}


int
lkpi_sx_slock(struct sx* sx, bool interruptible)
{
	int error = 0;

	mtx_lock(&sx->sx_mtx);
	while (sx->sx_lock != 0 || sx->sx_xwaiters > 0) {
		error = sx_wait_locked(sx, interruptible);
		if (error != 0)
			break;
	}
	if (error == 0)
		sx->sx_readers++;
	mtx_unlock(&sx->sx_mtx);
	return error;
}


int
sx_try_xlock(struct sx* sx)
{
	struct thread* td = curthread;
	int locked = 0;

	mtx_lock(&sx->sx_mtx);
	if (sx->sx_lock == (uintptr_t)td && (sx->sx_flags & SX_RECURSE) != 0) {
		sx->sx_recurse++;
		locked = 1;
	} else if (sx->sx_lock == 0 && sx->sx_readers == 0) {
		sx->sx_lock = (uintptr_t)td;
		locked = 1;
	}
	mtx_unlock(&sx->sx_mtx);
	return locked;
}


int
sx_try_slock(struct sx* sx)
{
	int locked = 0;

	mtx_lock(&sx->sx_mtx);
	if (sx->sx_lock == 0 && sx->sx_xwaiters == 0) {
		sx->sx_readers++;
		locked = 1;
	}
	mtx_unlock(&sx->sx_mtx);
	return locked;
}


void
sx_xunlock(struct sx* sx)
{
	mtx_lock(&sx->sx_mtx);
	KASSERT(sx->sx_lock == (uintptr_t)curthread,
		("sx_xunlock: %p not owned", sx));
	if (sx->sx_recurse > 0) {
		sx->sx_recurse--;
	} else {
		sx->sx_lock = 0;
		if (sx->sx_waiters > 0)
			cv_broadcast(&sx->sx_cv);
	}
	mtx_unlock(&sx->sx_mtx);
}


void
sx_sunlock(struct sx* sx)
{
	mtx_lock(&sx->sx_mtx);
	KASSERT(sx->sx_readers > 0, ("sx_sunlock: %p not share locked", sx));
	sx->sx_readers--;
	if (sx->sx_readers == 0 && sx->sx_waiters > 0)
		cv_broadcast(&sx->sx_cv);
	mtx_unlock(&sx->sx_mtx);
}


void
sx_unlock(struct sx* sx)
{
	if (sx->sx_lock == (uintptr_t)curthread)
		sx_xunlock(sx);
	else
		sx_sunlock(sx);
}


void
sx_downgrade(struct sx* sx)
{
	mtx_lock(&sx->sx_mtx);
	KASSERT(sx->sx_lock == (uintptr_t)curthread && sx->sx_recurse == 0,
		("sx_downgrade: %p not exclusively owned once", sx));
	sx->sx_lock = 0;
	sx->sx_readers++;
	if (sx->sx_waiters > 0)
		cv_broadcast(&sx->sx_cv);
	mtx_unlock(&sx->sx_mtx);
}


int
sx_has_waiters(struct sx* sx)
{
	return sx->sx_waiters > 0;
}
