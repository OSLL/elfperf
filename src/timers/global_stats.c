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
//#include "hpet_cntrs.h"
#include "rdtsc_cntrs.h"

#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <errno.h>

// Global array of functions statistics
static struct FunctionStatistic** s_stats=NULL;
// Number of statistics
static int s_statsCount = 0;


// Record function start time into context->startTime
void record_start_time(void * context)
{
    //printf("ELFPERF_DEBUG: record_start_time\n ");
    struct WrappingContext * cont = (struct WrappingContext *)context;
    cont->startTime = getRdtscTicks();
}

// Record function end time into context->endTime and
// print the duration of function execution
void record_end_time(void * context)
{
    struct WrappingContext * cont = (struct WrappingContext *)context;

    cont->endTime = getRdtscTicks();

    uint64_t duration = cont->endTime - cont->startTime;
    //printf("Function duration = %llu ticks\n", duration);

    // Updating statistic for function
    updateStat(cont->functionPointer - 3, duration);
    //errno = 0;
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

// Spinlock for updateStat
static int updateStatSpinlock=0;

void updateStat(void* funcAddr, uint64_t diffTime)
{
    struct FunctionStatistic* stat = getFunctionStatistic(funcAddr);
    if (stat != NULL) {
        __sync_fetch_and_add(&(stat->totalCallsNumber), 1);

        // Try to lock
        while(  __sync_fetch_and_add(&updateStatSpinlock,1)!=0);

        stat->totalDiffTime += diffTime;
	
        // Unlock
        updateStatSpinlock = 0;

	    printf("Current statistic for function = %p, call time %lld, total tick number = %llu, number of calls = %lld\n",
			   stat->realFuncAddr, diffTime, stat->totalDiffTime,
			   stat->totalCallsNumber);
    } else {
        addNewStat(funcAddr, diffTime);
    }

}


static bool isSharedMemoryInited = 0;

// Spinlock for shared memory initialization
static int sharedMemoryInitSpinlock=0;

static void initStats(){
	int count;
	get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);
	s_stats = (struct FunctionStatistic**)malloc(sizeof(struct FunctionStatistic*)*count);
	printf("\tInitializing shared memory %p\n", s_stats);
}

static void initSharedMemory(){

	if (isSharedMemoryInited) return;

	///// Critical section
	// Try to lock
        while(  __sync_fetch_and_add(&sharedMemoryInitSpinlock,1)!=0);
	
	if (isSharedMemoryInited) return;

	initStats();

	// Shared memory variables

	struct FunctionStatistic *** shm = getFunctionStatisticsStorage();
	
	
	// Storing s_stats into shared memory
	*shm = s_stats;
	// Set flag - shared memory is already inited
	isSharedMemoryInited = 1;	
		
	printf("Shared memory inited successfuly: shm = %p ,s_stats = %p\n", *shm,  s_stats );

	// Unlock
	sharedMemoryInitSpinlock = 0;
	///// Critical section ends

	

	
}

struct FunctionStatistic* addNewStat(void *funcAddr, uint64_t diffTime)
{
    // Do s_stats pointers writing into the shared memory if it is not performed before

    if (!isSharedMemoryInited) initSharedMemory();

   //////


    if (s_statsCount >= STATS_LIMIT){
        printf("Statistics buffer is full! Exiting\n");
        exit(1);
    }

    struct FunctionStatistic* stat = (struct FunctionStatistic*)malloc(sizeof(struct FunctionStatistic));

    stat->realFuncAddr = funcAddr;
    stat->totalDiffTime = diffTime;
    stat->totalCallsNumber = 1;
    s_stats[ __sync_fetch_and_add(&s_statsCount, 1)] = stat;

    return stat;
}

// Print stats for all functions
void printFunctionStatistics(){
    int i;
    for (i = 0; i < s_statsCount; i++){
        struct FunctionStatistic *stat = s_stats[i];
        printf("Statistic for function = %p, total tick number = %llu, number of calls = %lld\n",
                   stat->realFuncAddr, stat->totalDiffTime,
                   stat->totalCallsNumber);
    }
}
