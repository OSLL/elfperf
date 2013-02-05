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
 * \file global_stats.h
 * \brief Global statistics for functions
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#ifndef _ELFPERF_GLOBAL_STATS_H_
#define _ELFPERF_GLOBAL_STATS_H_

#include <stdint.h>
#include "../libelfperf/ld-routines.h"

struct FunctionStatistic* getFunctionStatistic(void* realFuncAddr);
void updateStat(void* funcAddr, uint64_t diffTime);
struct FunctionStatistic* addNewStat(void *funcAddr, uint64_t diffTime);

// Record function start time into context->startTime
//void record_start_time(void * context);
// Record function end time into context->endTime and
// print the duration of function execution
//void record_end_time(void * context);

void record_start_time(void*) __attribute__((visibility("hidden")));
void record_end_time(void*) __attribute__((visibility("hidden")));

#endif // _ELFPERF_GLOBAL_STATS_H_
