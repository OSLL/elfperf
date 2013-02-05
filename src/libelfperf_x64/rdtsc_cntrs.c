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
 * \file rdtsc_cntrs.c
 * \brief Implementation of functions for time measurement based on processor-specific registers
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "rdtsc_cntrs.h"
#include <sched.h>

#define NANOSECONDS_IN_SEC 1000000000
static double s_ticksPerNanoSec;

// Assembly code for reading number of CPU ticks from TSC (Time Stamp Counter) register
static inline uint64_t rdtsc()
{
    unsigned int high, low;
    asm volatile("rdtsc" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}

// This function is used for calibrating the number of CPU cycles per nanosecond
/*void calibrateTicks()
{
    struct timespec start_ts, end_ts;
    uint64_t start = 0, end = 0;
    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    start = rdtsc();
    uint64_t i;
    for (i = 0; i < 1000000; i++);
    end = rdtsc();
    clock_gettime(CLOCK_MONOTONIC, &end_ts);
    struct timespec elapsed_ts = diffTimeSpec(start_ts, end_ts);
    uint64_t elapsed_nsec = elapsed_ts.tv_sec * 1000000000LL + elapsed_ts.tv_nsec;
    s_ticksPerNanoSec = (double)(end - start) / (double)elapsed_nsec;
}

// This function should be called before using rdtsc(),
// has side effect of binding to CPU0.
void initRdtsc()
{
    size_t cpuMask = 1;
    sched_setaffinity(0, sizeof(cpuMask), &cpuMask);
    calibrateTicks();
}

// Calculate difference between two timespecs
struct timespec diffTimeSpec(struct timespec start, struct timespec end)
{
    struct timespec res;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        res.tv_sec = end.tv_sec - start.tv_sec - 1;
        res.tv_nsec = NANOSECONDS_IN_SEC + end.tv_nsec - start.tv_nsec;
    } else {
        res.tv_sec = end.tv_sec - start.tv_sec;
        res.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return res;
}


// Get elapsed time in timespecs for given time in nanoseconds
void getTimeSpec(struct timespec *ts, uint64_t nsecs)
{
    ts->tv_sec = nsecs / NANOSECONDS_IN_SEC;
    ts->tv_nsec = nsecs % NANOSECONDS_IN_SEC;
}

// Get elapsed time in timespecs using time converted from TSC reading
void getRdtscTime(struct timespec* ts)
{
    getTimeSpec(ts, rdtsc()/s_ticksPerNanoSec);
}*/

uint64_t getRdtscTicks()
{
    return rdtsc();
}
/*
double getCpuFrequency()
{

    return s_ticksPerNanoSec;
}*/
