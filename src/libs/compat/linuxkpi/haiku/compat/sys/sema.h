/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SEMA_H_
#define _LKPI_HAIKU_SYS_SEMA_H_

/*
 * FreeBSD's counting semaphore (<sys/sema.h>), on Haiku semaphores -
 * a direct fit. Only the calls LinuxKPI's <linux/semaphore.h> makes.
 */

#include <OS.h>


struct sema {
	sem_id	id;
};


static inline void
sema_init(struct sema* sema, int value, const char* description)
{
	sema->id = create_sem(value, description);
	if (sema->id < 0)
		panic("sema_init: create_sem failed");
}


static inline void
sema_destroy(struct sema* sema)
{
	delete_sem(sema->id);
	sema->id = -1;
}


static inline void
sema_post(struct sema* sema)
{
	release_sem(sema->id);
}


static inline void
sema_wait(struct sema* sema)
{
	status_t status;
	do {
		status = acquire_sem(sema->id);
	} while (status == B_INTERRUPTED);
}


/* nonzero when the semaphore was taken */
static inline int
sema_trywait(struct sema* sema)
{
	return acquire_sem_etc(sema->id, 1, B_RELATIVE_TIMEOUT, 0) == B_OK;
}


#endif /* _LKPI_HAIKU_SYS_SEMA_H_ */
