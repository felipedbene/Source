/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_STDARG_H_
#define _LKPI_HAIKU_SYS_STDARG_H_

/*
 * LinuxKPI's top-level <stdarg.h> wraps FreeBSD's <sys/stdarg.h>. As it is
 * first in the include path, it is what Haiku's own headers get as well, and
 * the compiler's <stdarg.h> can no longer be reached from here. So, like
 * FreeBSD, define it on the compiler builtins, and set GCC's include guards so
 * its stdarg.h doesn't define it again should it be included anyway.
 */

#define _STDARG_H
#define _ANSI_STDARG_H_
#define __GNUC_VA_LIST
#define _VA_LIST
#define _VA_LIST_
#define _VA_LIST_DEFINED

typedef __builtin_va_list	__gnuc_va_list;
typedef __builtin_va_list	va_list;

#define va_start(ap, last)	__builtin_va_start((ap), (last))
#define va_arg(ap, type)	__builtin_va_arg((ap), type)
#define va_copy(dest, src)	__builtin_va_copy((dest), (src))
#define va_end(ap)			__builtin_va_end(ap)


#endif /* _LKPI_HAIKU_SYS_STDARG_H_ */
