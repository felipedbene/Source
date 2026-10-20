#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Run rtw_tx_report_enqueue() and rtw_tx_report_purge_timer() from rtw88's
tx.c: when the purge timer fires, each frame that has waited the full
RTW_TX_PROBE_TIMEOUT for a firmware TX report is completed exactly once
through rtw_tx_report_tx_status() as not acked, outside q_lock, so net80211
runs its M_TXCB callbacks; younger frames stay queued in order and the timer
is re-armed for the oldest of them, also across the wrap of Haiku's 32-bit
tick counter. Freeing frames without a status (the old skb_queue_purge())
fails."""
from pathlib import Path
import os
import subprocess
import tempfile

root = (Path(os.environ['SOURCE_ROOT']) if 'SOURCE_ROOT' in os.environ
        else Path(__file__).resolve().parents[8])
source = (root / 'src/add-ons/kernel/drivers/network/wlan/rtw88/dev/rtw88/'
          'tx.c').read_text()


def extract(signature, body, optional=False):
    if optional and body not in source:
        return ''
    start = source.rindex(signature, 0, source.index(body) + len(body))
    return source[start:source.index('\n}\n', start) + 3]


# the definitions, not the forward declaration; the helper is newer
functions = (extract('static void rtw_tx_report_tx_status(', 'bool acked)\n{')
             + extract('static uint32_t rtw_tx_report_fail(',
                       'rtw_tx_report_fail(struct rtw_dev *rtwdev)\n{', True)
             + extract('void rtw_tx_report_purge_timer(', 'timer_list *t)\n{')
             + extract('void rtw_tx_report_enqueue(', 'u8 sn)\n{'))

fixture = r'''
#undef __linux__
#define __FreeBSD__ 1
#define __HAIKU__ 1
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint8_t u8;
#define IEEE80211_TX_STAT_ACK 0x1
#define RTW_DBG_TX 0
#define RTW_TX_PROBE_TIMEOUT 500UL
#define time_after_eq(a, b) ((long)((a) - (b)) >= 0)
#define timer_container_of(var, t, field) \
	((struct rtw_dev *)((char *)(t) - offsetof(struct rtw_dev, field)))
struct timer_list { int armed; unsigned long delay; };
struct ieee80211_hw { int unused; };
struct ieee80211_tx_info { uint32_t flags;
	struct { int rates_count; void *status_driver_data[2]; } status; };
struct sk_buff { struct sk_buff *next; struct ieee80211_tx_info info;
	int statuses, acked, freed_silently; };
struct sk_buff_head { struct sk_buff *head, *tail; uint32_t qlen; };
typedef struct { int held; } spinlock_t;
struct rtw_tx_report { spinlock_t q_lock; struct sk_buff_head queue;
	struct timer_list purge_timer; };
struct rtw_dev { struct ieee80211_hw *hw; struct rtw_tx_report tx_report; };

static spinlock_t *q_lock;
/* as on Haiku: jiffies is ((unsigned long)ticks), ticks an int32_t */
static int32_t ticks;
#define jiffies ((unsigned long)ticks)

static void rtw_dbg(struct rtw_dev *d, int mask, const char *fmt, ...) { }

#define spin_lock_irqsave(l, f) do { (f) = 0; assert(!(l)->held); \
	(l)->held = 1; } while (0)
#define spin_unlock_irqrestore(l, f) do { (void)(f); assert((l)->held); \
	(l)->held = 0; } while (0)
#define IEEE80211_SKB_CB(skb) (&(skb)->info)
#define ieee80211_tx_info_clear_status(info) ((info)->status.rates_count = 0)
#define skb_queue_len(q) ((q)->qlen)
#define skb_peek(q) ((q)->head)

static void
mod_timer(struct timer_list *timer, unsigned long expires)
{
	timer->armed++;
	timer->delay = expires - jiffies;
}

static void
__skb_queue_head_init(struct sk_buff_head *q)
{
	q->head = q->tail = NULL;
	q->qlen = 0;
}

static void
__skb_queue_tail(struct sk_buff_head *q, struct sk_buff *skb)
{
	skb->next = NULL;
	if (q->tail != NULL)
		q->tail->next = skb;
	else
		q->head = skb;
	q->tail = skb;
	q->qlen++;
}

static void
__skb_unlink(struct sk_buff *skb, struct sk_buff_head *q)
{
	struct sk_buff **p = &q->head, *prev = NULL;

	while (*p != skb) {
		prev = *p;
		p = &(*p)->next;
	}
	*p = skb->next;
	if (q->tail == skb)
		q->tail = prev;
	q->qlen--;
	skb->next = NULL;
}

static struct sk_buff *
__skb_dequeue(struct sk_buff_head *q)
{
	struct sk_buff *skb = q->head;

	if (skb != NULL)
		__skb_unlink(skb, q);
	return skb;
}

static void
skb_queue_purge(struct sk_buff_head *q)
{
	struct sk_buff *skb;

	/* kfree_skb(): the mbuf and node go, ieee80211_tx_complete() never runs */
	while ((skb = __skb_dequeue(q)) != NULL)
		skb->freed_silently++;
}

static void
ieee80211_tx_status_irqsafe(struct ieee80211_hw *hw, struct sk_buff *skb)
{
	/* runs net80211 callbacks; must not hold up enqueue */
	assert(!q_lock->held);
	skb->statuses++;
	skb->acked = (skb->info.flags & IEEE80211_TX_STAT_ACK) != 0;
}
'''

checks = r'''
static struct rtw_dev rtwdev;
static struct sk_buff skbs[3];

static void
check(int32_t now, int done, int armed, unsigned long delay)
{
	struct timer_list *timer = &rtwdev.tx_report.purge_timer;
	struct sk_buff *skb;
	int i;

	printf("ticks=%d: queued %u, re-armed %d delay %lu", now,
	    skb_queue_len(&rtwdev.tx_report.queue), timer->armed, timer->delay);
	for (i = 0; i < 3; i++)
		printf(", skb %d status %d acked %d silent free %d", i,
		    skbs[i].statuses, skbs[i].acked, skbs[i].freed_silently);
	printf("\n");
	fflush(stdout);
	for (i = 0; i < 3; i++) {
		assert(skbs[i].freed_silently == 0);
		assert(skbs[i].statuses == (i < done));
		assert(skbs[i].acked == 0);
	}
	/* the rest are still queued, in order */
	assert(skb_queue_len(&rtwdev.tx_report.queue) == 3 - done);
	for (i = done, skb = rtwdev.tx_report.queue.head; i < 3; i++,
	    skb = skb->next)
		assert(skb == &skbs[i]);
	assert(skb == NULL);
	assert(timer->armed == armed);
	if (armed)
		assert(timer->delay == delay);
	assert(!q_lock->held);
}

static void
fire(int32_t now, int done, int armed, unsigned long delay)
{
	ticks = now;
	rtwdev.tx_report.purge_timer.armed = 0;
	rtw_tx_report_purge_timer(&rtwdev.tx_report.purge_timer);
	check(now, done, armed, delay);
}

static void
enqueue(const int32_t queued[3])
{
	int i;

	memset(skbs, 0, sizeof(skbs));
	for (i = 0; i < 3; i++) {
		/* a stale ACK flag must not survive the purge */
		skbs[i].info.flags = IEEE80211_TX_STAT_ACK;
		ticks = queued[i];
		rtw_tx_report_enqueue(&rtwdev, &skbs[i], i << 2);
	}
}

/* ticks + n as the int32_t tick counter wraps */
static int32_t
later(int32_t t, uint32_t n)
{
	return (int32_t)((uint32_t)t + n);
}

int
main(void)
{
	static const int32_t queued[3] = { 1000, 1100, 1400 };
	const int32_t old = INT32_MAX - 100;
	const int32_t wrapping[3] = { old, old + 50, old + 90 };

	q_lock = &rtwdev.tx_report.q_lock;
	__skb_queue_head_init(&rtwdev.tx_report.queue);
	enqueue(queued);
	fire(1500, 1, 1, 100);	/* skb 0 has waited 500, skb 1 has not */
	fire(1599, 1, 1, 1);	/* early: nothing due, re-armed again */
	fire(1600, 2, 1, 300);
	fire(1900, 3, 0, 0);	/* empty: not re-armed */
	fire(2400, 3, 0, 0);

	/* across INT32_MAX -> INT32_MIN, where jiffies sign-extends */
	enqueue(wrapping);
	fire(later(old, 499), 0, 1, 1);
	fire(later(old, 500), 1, 1, 50);
	fire(later(old, 590), 3, 0, 0);
	puts("PASS only timed-out TX-report frames complete, once, not acked");
	return 0;
}
'''

with tempfile.TemporaryDirectory() as directory:
    c = Path(directory) / 'tx_report_purge.c'
    c.write_text(fixture + functions + checks)
    exe = c.with_suffix('')
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c99', '-Wall', '-Werror',
        '-Wno-unused-function', '-fsanitize=address,undefined', str(c), '-o',
        str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
