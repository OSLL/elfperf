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

// Global array of functions statistics
static struct FunctionStatistic* s_stats;
// Number of statistics
static int s_statsCount;

// Record function start time into context->startTime
void record_start_time(void * context)
{
    struct WrappingContext * cont = (struct WrappingContext *)context;
    cont->startTime = get_accurate_time();
}

// Record function end time into context->endTime and
// print the duration of function execution
void record_end_time(void * context)
{
    struct WrappingContext * cont = (struct WrappingContext *)context;
    cont->endTime = get_accurate_time();
    struct timespec duration = diff(cont->startTime, cont->endTime);
    printf("Function duration = %ds %dns\n", duration.tv_sec, duration.tv_nsec);

    printf("Function address = %d\n", cont->functionPointer);
}

// Get statistic for given function
struct FunctionStatistic* getFunctionStatistic(void *realFuncAddr)
{
    int i;
    for (i = 0; i < s_statsCount; i++) {
        struct FunctionStatistic* stat = s_stats + i;
        if (stat->realFuncAddr == realFuncAddr)
            return stat;
    }

    return NULL;
}
