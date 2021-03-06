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
#include "rdtsc_cntrs.h"
#include "libelfperf.h"

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>

// Global array of functions statistics
static struct FunctionStatistic** s_stats = NULL;
// Number of statistics
static unsigned int s_statsCount = 0;

// Flag of shared memory initialization
static bool isSharedMemoryInited = 0;

// Spinlock for updateStat
static int updateStatSpinlock=0;

// Spinlock for shared memory initialization
static int sharedMemoryInitSpinlock=0;

// Record function start time into context->startTime
void record_start_time(void* context)
{
    struct WrappingContext* cont = (struct WrappingContext*)context;
    printf("LIBELFPERF_LOG: record_start_time for context: %x\n", cont);
    cont->startTime = getRdtscTicks();
}

// Record function end time into context->endTime and
// print the duration of function execution
void record_end_time(void* context)
{
    struct WrappingContext* cont =  (struct WrappingContext*)context;
    printf("LIBELFPERF_LOG: record_end_time for context: %x\n", cont);
 
    cont->endTime = getRdtscTicks();

    uint64_t duration = cont->endTime - cont->startTime;
    printf("LIBELFPERF_LOG: Function duration = %llu ticks\n", duration);

    // Updating statistic for function
    updateStat(cont->functionPtr, duration);
}

// Initialize statistics buffer
static void initStats()
{
    unsigned int count;
    get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);
    s_stats = (struct FunctionStatistic**)mmap(0, sizeof(struct FunctionStatistic*)*count, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    printf("LIBELFPERF_LOG: Initializing shared memory for FunctionStatistic%p\n", s_stats);
}

// Initialize shared memory for statistics storage
static void initSharedMemory()
{
    if (isSharedMemoryInited) {
        return;
    }

    initStats();
    struct FunctionStatistic *** shm = getFunctionStatisticsStorage();
	
    // Storing s_stats into shared memory
    *shm = s_stats;
    isSharedMemoryInited = 1;	
		
    printf("LIBELFPERF_LOG: Shared memory for FunctionStorage inited successfuly: shm = %p ,s_stats = %p\n", *shm,  s_stats );
}

// Add new statistic into buffer
struct FunctionStatistic* addNewStat(void *funcAddr, uint64_t diffTime)
{
    // Do s_stats pointers writing into the shared memory if it is not performed before
    if (!isSharedMemoryInited) initSharedMemory();

    if (s_statsCount >= STATS_LIMIT) {
        printf("LIBELFPERF_LOG: Statistics buffer is full! Exiting\n");
        exit(1);
    }

    struct FunctionStatistic* stat = 
        (struct FunctionStatistic*)mmap(0, sizeof(struct FunctionStatistic), 
                                        PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    stat->realFuncAddr = funcAddr;
    stat->totalDiffTime = diffTime;
    stat->totalCallsNumber = 1;
    s_stats[ __sync_fetch_and_add(&s_statsCount, 1)] = stat;

    printf("LIBELFPERF_LOG: finished addNewStat\n");
    return stat;
}

// Update statistic for given function with new diffTime
void updateStat(void* funcAddr, uint64_t diffTime)
{
    // Try to lock
    while(__sync_fetch_and_add(&updateStatSpinlock, 1)!=0);

    struct FunctionStatistic* stat = getFunctionStatistic(funcAddr);
    if (stat != NULL) {
	stat->totalCallsNumber++;
        stat->totalDiffTime += diffTime;
        printf("LIBELFPERF_LOG: Current statistic for function = %p, call time %lld, total tick number = %llu, number of calls = %lld\n",
               stat->realFuncAddr, diffTime, stat->totalDiffTime,
               stat->totalCallsNumber);
    } else {
        printf("LIBELFPERF_LOG: Statistic is empty. Adding new one.\n");
        addNewStat(funcAddr, diffTime);
    }

    // Unlock
    updateStatSpinlock = 0;
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

// Print statistics for all profiled functions
void printFunctionStatistics()
{
    int i;
    for (i = 0; i < s_statsCount; i++){
        struct FunctionStatistic *stat = s_stats[i];
        printf("LIBELFPERF_LOG: Statistic for function = %p, total tick number = %llu, number of calls = %lld\n",
                   stat->realFuncAddr, stat->totalDiffTime,
                   stat->totalCallsNumber);
    }
}
