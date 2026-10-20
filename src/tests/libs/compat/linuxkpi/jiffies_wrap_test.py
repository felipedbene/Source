#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Run LinuxKPI's time_after(), jiffies_to_msecs() and
linux_timer_jiffies_until() (linux_compat.c) on Haiku's jiffies across the
int32 roll of freebsd_network's ticks: a timer armed before it is due on
time, not ~24.8 days late. The same checks built with the old
jiffies = (unsigned long)ticks, which sign-extends at the roll, must fail."""
from pathlib import Path
import os
import re
import subprocess
import tempfile

root = (Path(os.environ['SOURCE_ROOT']) if 'SOURCE_ROOT' in os.environ
        else Path(__file__).resolve().parents[5])
compat = root / 'src/libs/compat'


def lines(path, pattern):
    found = re.findall(pattern, (compat / path).read_text(), re.M)
    assert found, (path, pattern)
    return '\n'.join(found) + '\n'


def function(path, head):
    source = (compat / path).read_text()
    start = source.index(head)
    return source[start:source.index('\n}\n', start) + 3]


kernel_h = 'freebsd_network/compat/sys/kernel.h'
clock = (lines(kernel_h, r'^#define hz\s.*$')
    + lines(kernel_h, r'^int(?:32|64)_t _get_ticksl?\(.*\);$')
    + lines(kernel_h, r'^#define ticksl? .*$')
    + lines('freebsd_network/compat/sys/time.h', r'^#define USEC_2_TICKS.*$')
    + function('freebsd_network/clock.c', 'int32_t\n_get_ticks()')
    + function('freebsd_network/clock.c', 'int64_t\n_get_ticksl(void)'))
jiffies = lines('linuxkpi/haiku/compat/linux/jiffies.h',
    r'^#define jiffies\s.*$')
assert 'ticksl' in jiffies, jiffies
linux = (lines('linuxkpi/common/include/linux/jiffies.h',
        r'^#define\t(?:time_(?:after|before)(?:_eq)?|jiffies_to_msecs)\(.*$')
    + function('linuxkpi/common/src/linux_compat.c',
        'static int\nlinux_timer_jiffies_until('))

fixture = r'''
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
typedef int64_t bigtime_t;
static bigtime_t now;

static bigtime_t
system_time(void)
{
	return now;
}
'''
checks = r'''
static int failures;

#define CHECK(cond) do { if (!(cond)) { failures++; \
	printf("FAIL line %d: %s (jiffies %#lx)\n", __LINE__, #cond, \
	    jiffies); } } while (0)

static void
set_ticks(int64_t t)
{
	now = t * 1000000 / hz;
}

int
main(void)
{
	unsigned long expires, start, end;

	/* the 64-bit count and ticks agree on the low 32 bits */
	set_ticks((int64_t)INT32_MAX + 12345);
	CHECK((int32_t)jiffies == ticks);

	/* a timer armed 500 ticks before the roll, due 1000 ticks later */
	set_ticks(INT32_MAX - 500);
	expires = jiffies + 1000;
	set_ticks(INT32_MAX + 100LL);
	CHECK(!time_after(jiffies, expires));
	CHECK(time_before(jiffies, expires));
	CHECK(linux_timer_jiffies_until(expires) == 400);
	set_ticks(INT32_MAX + 500LL);
	CHECK(time_after_eq(jiffies, expires));
	set_ticks(INT32_MAX + 501LL);
	CHECK(time_after(jiffies, expires));
	CHECK(linux_timer_jiffies_until(expires) == 1);

	/* mod_timer(jiffies + d) whose second jiffies read is past the roll */
	set_ticks(INT32_MAX);
	expires = jiffies + 10;
	set_ticks(INT32_MAX + 2LL);
	CHECK(linux_timer_jiffies_until(expires) == 8);

	/* wait_for_completion_timeout(): end computed once, re-armed later */
	set_ticks(INT32_MAX - 50);
	end = jiffies + 100;
	set_ticks(INT32_MAX + 1LL);
	CHECK(linux_timer_jiffies_until(end) == 49);

	/* a small delta across the roll converts to milliseconds */
	set_ticks(INT32_MAX - 5);
	start = jiffies;
	set_ticks(INT32_MAX + 5LL);
	CHECK(jiffies_to_msecs(jiffies - start) == 10 * 1000 / hz);

	/* a far deadline is clamped to the largest callout delay */
	set_ticks(INT32_MAX - 500);
	CHECK(linux_timer_jiffies_until(jiffies + 49 * 86400 * hz) == INT_MAX);

	if (failures != 0)
		return 1;
	puts("PASS jiffies, time_after and timer delays across the ticks roll");
	return 0;
}
'''


def run(directory, name, jiffies):
    c = Path(directory) / (name + '.c')
    c.write_text(fixture + clock + jiffies + linux + checks)
    exe = c.with_suffix('')
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c99', '-Wall', '-Werror',
        '-Wno-unused-function', '-fsanitize=address,undefined', str(c), '-o',
        str(exe)], check=True)
    return subprocess.run([str(exe)]).returncode


with tempfile.TemporaryDirectory() as directory:
    assert run(directory, 'jiffies_wrap', jiffies) == 0
    # negative control: the old definition must fail the same checks
    assert run(directory, 'jiffies_wrap_old',
        '#define jiffies ((unsigned long)ticks)\n') != 0
    print('PASS old jiffies = (unsigned long)ticks fails across the roll')
