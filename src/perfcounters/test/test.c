/*
 * Copyright © 2012 OSLL osll@osll.spb.ru
 *
     * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * The advertising clause requiring mention in adverts must never be included.
 */
/*! ---------------------------------------------------------------
 * \file test.c
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "../time.h"

void loop(int n)
{
    int i;
    for (i = 0; i < n; i++);
}

void test_sleep()
{
    printf("Test of sleep function:\n");

    struct timespec start, end;
    int expected = 1;
    start = get_accurate_time();
    sleep(expected);
    end = get_accurate_time();
    int elapsed_sec = diff(start, end).tv_sec;
    int elapsed_ns = diff(start, end).tv_nsec;
    printf("1: expected: %ds; elapsed: %ds %dns\n", expected, elapsed_sec, elapsed_ns);
    assert(expected == elapsed_sec);

    expected = 2;
    start = get_accurate_time();
    sleep(expected);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("2: expected: %ds; elapsed: %ds %dns\n", expected, elapsed_sec, elapsed_ns);
    assert(expected == elapsed_sec);

    expected = 5;
    start = get_accurate_time();
    sleep(expected);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("3: expected: %ds; elapsed: %ds %dns\n", expected, elapsed_sec, elapsed_ns);
    assert(expected == elapsed_sec);
}

void test_loop()
{
    printf("Test of loops:\n");

    struct timespec start, end;
    int elapsed_sec, elapsed_ns;

    printf("loop(10000)\n");
    start = get_accurate_time();
    loop(10000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t1: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    start = get_accurate_time();
    loop(10000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t2: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    start = get_accurate_time();
    loop(10000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t3: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    printf("loop(1000000)\n");
    start = get_accurate_time();
    loop(1000000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t1: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    start = get_accurate_time();
    loop(1000000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t2: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    start = get_accurate_time();
    loop(1000000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t3: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    printf("loop(100000000)\n");
    start = get_accurate_time();
    loop(100000000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t1: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    start = get_accurate_time();
    loop(1000000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t2: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);

    start = get_accurate_time();
    loop(1000000);
    end = get_accurate_time();
    elapsed_sec = diff(start, end).tv_sec;
    elapsed_ns = diff(start, end).tv_nsec;
    printf("\t3: elapsed: %ds %dns\n", elapsed_sec, elapsed_ns);
}

int main(int argc, char **argv)
{
    //test_sleep();
    test_loop();
    return 0;
}
