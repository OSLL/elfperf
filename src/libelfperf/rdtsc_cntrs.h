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
 * \file rdtsc_cntrs.h
 * \brief Declaration of functions for time measurement based on processor-specific registers
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#ifndef _ELFPERF_RDTSC_CNTRS_H_
#define _ELFPERF_RDTSC_CNTRS_H_

#include <time.h>
#include <stdint.h>

//Calibrate the number of CPU cycles per nanosecond
//void calibrateTicks();

// Initialize environment for correct using of rdtsc()
//void initRdtsc();

// Calculate difference between two timespecs
//struct timespec diffTimeSpec(struct timespec start, struct timespec end);

// Get elapsed time in timespecs for given time in nanoseconds
//void getTimeSpec(struct timespec* ts, uint64_t nsecs);

// Get elapsed time in timespecs using time converted from TSC reading
//void getRdtscTime(struct timespec* ts);

// Get number of CPU ticks by the moment
uint64_t getRdtscTicks();

//double getCpuFrequency();

#endif // _ELFPERF_RDTSC_CNTRS_H_
