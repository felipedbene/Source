/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_CTYPE_H_
#define _LKPI_HAIKU_SYS_CTYPE_H_

/*
 * Extends freebsd_network's <sys/ctype.h> (isprint only) with the rest of
 * FreeBSD's kernel ctype macros, ASCII-only like FreeBSD's.
 */

#include_next <sys/ctype.h>


#define isascii(c)	(((c) & ~0x7f) == 0)
#define isupper(c)	((c) >= 'A' && (c) <= 'Z')
#define islower(c)	((c) >= 'a' && (c) <= 'z')
#define isalpha(c)	(isupper(c) || islower(c))
#define isdigit(c)	((c) >= '0' && (c) <= '9')
#define isalnum(c)	(isalpha(c) || isdigit(c))
#define isxdigit(c)	(isdigit(c) \
	|| ((c) >= 'A' && (c) <= 'F') \
	|| ((c) >= 'a' && (c) <= 'f'))
#define isspace(c)	((c) == ' ' || ((c) >= '\t' && (c) <= '\r'))
#define iscntrl(c)	(((c) >= 0 && (c) <= 0x1f) || (c) == 0x7f)
#define isgraph(c)	((c) > ' ' && (c) <= '~')
#define ispunct(c)	(isgraph(c) && !isalnum(c))

#define toupper(c)	(islower(c) ? (c) - 0x20 : (c))
#define tolower(c)	(isupper(c) ? (c) + 0x20 : (c))


#endif /* _LKPI_HAIKU_SYS_CTYPE_H_ */
