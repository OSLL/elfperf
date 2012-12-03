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
 * \file global_stats.c
 * \brief Global statistics for functions
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "global_stats.h"
#include "../wrappers/cdecl_wrapper.h"
#include "hpet_cntrs.h"
#include "rdtsc_cntrs.h"

#include <stdlib.h>
#include <stdio.h>

#define STATS_LIMIT 100000
// Global array of functions statistics
static struct FunctionStatistic* s_stats[STATS_LIMIT];
// Number of statistics
static int s_statsCount = 0;

// Record function start time into context->startTime
void record_start_time(void * context)
{
    struct WrappingContext * cont = (struct WrappingContext *)context;

#ifdef TIMING_WITH_HPET
    printf("LOG: get start time with HPET\n");
    cont->startTime = get_accurate_time();
#endif

#ifdef TIMING_WITH_RDTSC
    printf("LOG: get start time with RDTSC\n");
    initRdtsc();
    getRdtscTime(&cont->startTime);
#endif

}

// Record function end time into context->endTime and
// print the duration of function execution
void record_end_time(void * context)
{
    struct WrappingContext * cont = (struct WrappingContext *)context;

#ifdef TIMING_WITH_HPET
    printf("LOG: get end time with HPET\n");
    cont->endTime = get_accurate_time();
#endif

#ifdef TIMING_WITH_RDTSC
    printf("LOG: get end time with RDTSC\n");
    getRdtscTime(&cont->endTime);
#endif

    struct timespec duration = diff(cont->startTime, cont->endTime);
    printf("Function duration = %ds %dns\n", duration.tv_sec, duration.tv_nsec);

    // Updating statistic for function
    updateStat(cont->functionPointer - 3, duration);
}

// Get statistic for given function
struct FunctionStatistic* getFunctionStatistic(void *realFuncAddr)
{
    int i;
    for (i = 0; i < s_statsCount; i++) {
        if (s_stats[i]->realFuncAddr == realFuncAddr)
            return s_stats[i];
    }

    return NULL;
}

void updateStat(void* funcAddr, struct timespec diffTime)
{
    struct FunctionStatistic* stat = getFunctionStatistic(funcAddr);
    if (stat != NULL) {
        __time_t result_sec = stat->totalDiffTime.tv_sec;
        long int result_nsec = stat->totalDiffTime.tv_nsec;

        result_sec += diffTime.tv_sec;
        result_nsec += diffTime.tv_nsec;

        if (result_nsec >= 1000000000) {
            result_sec += 1;
            result_nsec = result_nsec - 1000000000;
        }

        stat->totalDiffTime.tv_sec = result_sec;
        stat->totalDiffTime.tv_nsec = (long int)result_nsec;
    } else {
        addNewStat(funcAddr, diffTime);
    }
}

struct FunctionStatistic* addNewStat(void *funcAddr, struct timespec diffTime)
{
    if (s_statsCount == STATS_LIMIT){
        printf("Statistics buffer is full! Exiting\n");
        exit(1);
    }

    s_statsCount += 1;
    struct FunctionStatistic* stat = (struct FunctionStatistic*)malloc(sizeof(struct FunctionStatistic));

    stat->realFuncAddr = funcAddr;
    stat->totalDiffTime.tv_sec = diffTime.tv_sec;
    stat->totalDiffTime.tv_nsec = diffTime.tv_nsec;
    s_stats[s_statsCount - 1] = stat;

    return stat;
}