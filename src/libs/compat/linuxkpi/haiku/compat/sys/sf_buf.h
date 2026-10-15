/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2026, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _LKPI_HAIKU_SYS_SF_BUF_H_
#define _LKPI_HAIKU_SYS_SF_BUF_H_

/*
 * The slice of FreeBSD's <sys/sf_buf.h> that <linux/scatterlist.h>'s
 * page-mapping iterators use. Declared so the inlines compile; the
 * rtw88 path never maps sg pages, so these stay unimplemented until
 * something links them.
 */

#include <vm/vm.h>


struct sf_buf;

#define SFB_NOWAIT		0x01
#define SFB_CPUPRIVATE	0x02

struct sf_buf* sf_buf_alloc(struct vm_page* page, int flags);
void sf_buf_free(struct sf_buf* sf);
vm_offset_t sf_buf_kva(struct sf_buf* sf);


#endif /* _LKPI_HAIKU_SYS_SF_BUF_H_ */
