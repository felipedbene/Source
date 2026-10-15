/* SPDX-License-Identifier: MIT */
/*
 * FreeBSD <vm/swap_pager.h> subset for <linux/swap.h>: no swap pager on
 * Haiku, so there are no swap pages to report (lkpi_vm.c).
 */
void swap_pager_status(int* total, int* used);
