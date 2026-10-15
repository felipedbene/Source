/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_MACHINE_ATOMIC_H_
#define _LKPI_HAIKU_MACHINE_ATOMIC_H_

/*
 * Shadows freebsd_network's <machine/atomic.h> for LinuxKPI code only,
 * built on the __haiku_atomic_* names that lkpi_prelude.h gave Haiku's
 * 32-bit atomic inlines (the bare names are Linux's in LinuxKPI
 * translation units; the 64-bit atomic_*64 names are not renamed).
 *
 * Semantics are FreeBSD's, which freebsd_network's version does not
 * always follow: atomic_set_*() or's bits in (it is not a store; stores
 * are atomic_store_*()), and atomic_readandclear_*() returns the old
 * value. All variants are sequentially consistent, which satisfies any
 * _acq/_rel. Haiku pointers are 64-bit.
 */

#include <KernelExport.h>


/* FreeBSD fcmpset: on failure, *old is updated to the current value. */
static inline int
__lkpi_atomic_fcmpset_32(volatile void* ptr, int32* old, int32 newValue)
{
	int32 previous = __haiku_atomic_test_and_set((int32*)ptr, newValue, *old);
	if (previous == *old)
		return 1;
	*old = previous;
	return 0;
}


static inline int
__lkpi_atomic_fcmpset_64(volatile void* ptr, int64* old, int64 newValue)
{
	int64 previous = atomic_test_and_set64((int64*)ptr, newValue, *old);
	if (previous == *old)
		return 1;
	*old = previous;
	return 0;
}


#define	atomic_load_32(ptr)			__haiku_atomic_get((int32*)ptr)
#define atomic_store_32(ptr, val)	(void)__haiku_atomic_set((int32*)ptr, (int32)(val))
#define atomic_set_32(ptr, val)		(void)__haiku_atomic_or((int32*)ptr, (int32)(val))
#define atomic_clear_32(ptr, val)	(void)__haiku_atomic_and((int32*)ptr, (int32)~(val))
#define atomic_fetchadd_32(ptr, val) __haiku_atomic_add((int32*)ptr, (int32)(val))
#define atomic_add_32(ptr, val)		(void)__haiku_atomic_add((int32*)ptr, (int32)(val))
#define atomic_subtract_32(ptr, val) (void)__haiku_atomic_add((int32*)ptr, -(int32)(val))
#define atomic_swap_32(ptr, val)	__haiku_atomic_get_and_set((int32*)ptr, (int32)(val))
#define atomic_readandclear_32(ptr) __haiku_atomic_get_and_set((int32*)ptr, 0)
#define atomic_cmpset_32(ptr, old, new) \
	(__haiku_atomic_test_and_set((int32*)ptr, new, old) == (int32)(old))
#define atomic_fcmpset_32(ptr, old, new) \
	__lkpi_atomic_fcmpset_32(ptr, (int32*)(old), (int32)(new))

#define atomic_load_acq_32		atomic_load_32
#define atomic_store_rel_32		atomic_store_32
#define atomic_set_acq_32		atomic_set_32
#define atomic_cmpset_acq_32	atomic_cmpset_32

#define	atomic_load_64(ptr)			atomic_get64((int64*)ptr)
#define atomic_store_64(ptr, val)	(void)atomic_set64((int64*)ptr, (int64)(val))
#define atomic_set_64(ptr, val)		(void)atomic_or64((int64*)ptr, (int64)(val))
#define atomic_clear_64(ptr, val)	(void)atomic_and64((int64*)ptr, (int64)~(val))
#define atomic_fetchadd_64(ptr, val) atomic_add64((int64*)ptr, (int64)(val))
#define atomic_swap_64(ptr, val)	atomic_get_and_set64((int64*)ptr, (int64)(val))
#define atomic_readandclear_64(ptr)	atomic_get_and_set64((int64*)ptr, 0)
#define atomic_cmpset_64(ptr, old, new) \
	(atomic_test_and_set64((int64*)ptr, new, old) == (int64)(old))
#define atomic_fcmpset_64(ptr, old, new) \
	__lkpi_atomic_fcmpset_64(ptr, (int64*)(old), (int64)(new))

#define atomic_load_acq_64		atomic_load_64
#define atomic_store_rel_64		atomic_store_64
#define atomic_cmpset_acq_64	atomic_cmpset_64


#define atomic_load_int			atomic_load_32
#define atomic_store_int		atomic_store_32
#define atomic_set_int			atomic_set_32
#define atomic_clear_int		atomic_clear_32
#define atomic_fetchadd_int		atomic_fetchadd_32
#define atomic_add_int			atomic_add_32
#define atomic_subtract_int		atomic_subtract_32
#define atomic_swap_int			atomic_swap_32
#define atomic_readandclear_int	atomic_readandclear_32
#define atomic_cmpset_int		atomic_cmpset_32
#define atomic_fcmpset_int		atomic_fcmpset_32

#define atomic_load_acq_int		atomic_load_32
#define atomic_store_rel_int	atomic_store_32
#define atomic_cmpset_acq_int	atomic_cmpset_32

/* unsigned long and pointers are 64-bit on x86_64 */
#define	atomic_load_long		atomic_load_64
#define	atomic_store_long		atomic_store_64
#define	atomic_set_long			atomic_set_64
#define	atomic_clear_long		atomic_clear_64
#define	atomic_fetchadd_long	atomic_fetchadd_64
#define	atomic_swap_long		atomic_swap_64
#define	atomic_readandclear_long	atomic_readandclear_64
#define	atomic_cmpset_long		atomic_cmpset_64
#define	atomic_fcmpset_long		atomic_fcmpset_64

#define	atomic_clear_rel_long	atomic_clear_64
#define	atomic_cmpset_acq_long	atomic_cmpset_64

#define	atomic_load_ptr(ptr)	((void*)atomic_load_64(ptr))
#define	atomic_store_ptr		atomic_store_64
#define	atomic_swap_ptr			atomic_swap_64
#define	atomic_readandclear_ptr	atomic_readandclear_64
#define	atomic_cmpset_ptr		atomic_cmpset_64
#define	atomic_fcmpset_ptr		atomic_fcmpset_64

#define	atomic_store_rel_ptr	atomic_store_64


#define mb()    memory_full_barrier()
#define wmb()   memory_write_barrier()
#define rmb()   memory_read_barrier()


#endif	/* _LKPI_HAIKU_MACHINE_ATOMIC_H_ */
