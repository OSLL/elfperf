#include <stdio.h>
#include <dlfcn.h>
#include <time.h>
#include <stdint.h>

#include "testlib.c"

uint64_t rdtsc()
{
    unsigned int hi, lo;
    asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) hi << 32) | lo;
}

#define NANOSECONDS_IN_SEC 1000000000
static double s_ticksPerNanoSec;

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


void calibrateTicks()
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

struct timespec get_accurate_time()
{
    struct timespec time;
    clockid_t clockType = CLOCK_MONOTONIC;
    clock_gettime(clockType, &time);
    return time;
}

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec res;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        res.tv_sec = end.tv_sec - start.tv_sec - 1;
        res.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        res.tv_sec = end.tv_sec - start.tv_sec;
        res.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return res;
}


int main()
{
    printf("===== Test of measurement error =====\n");	

    int n = 1000;

    void* p = dlopen("./libtest.so", RTLD_LAZY);

    int i;
    // Calculate error of measurement method
    /*uint64_t sum = 0;
    for (i = 0; i < n; i++) {
        uint64_t start, end;
        start = rdtsc();
        slowFunction();
        end = rdtsc();
        uint64_t duration = end - start;
        //printf("%d: duration %llu\n", i, duration);
        sum += duration;
    }

    void (*testFn)() = dlsym(p, "slowFunction");
    for (i = 0; i < n; i++) {
        testFn();
    }
    printf("Sample average value %f\n", (double) sum / n);*/

    // Calculate profiler time measurement error

    calibrateTicks();    
    printf("Ticks per ns: %f\n", s_ticksPerNanoSec);

    uint64_t sum1 = 0;
    for (i = 0; i < n; i++) {
        uint64_t start, end;
        struct timespec startts, endts;
        calibrateTicks();
        startts = get_accurate_time();
        slowFunction();
        endts = get_accurate_time();
        //printf("%d: Start TS: %dsec %dnsec\n", i, startts.tv_sec, startts.tv_nsec);
        //printf("%d: End TS: %dsec %dnsec\n", i, endts.tv_sec, endts.tv_nsec);
        //printf("%d: Duration: %dsec %dnsec\n", i, diff(startts, endts).tv_sec, diff(startts, endts).tv_nsec);

        start = (uint64_t)((startts.tv_sec * NANOSECONDS_IN_SEC + startts.tv_nsec) * s_ticksPerNanoSec);
        end = (uint64_t)((endts.tv_sec * NANOSECONDS_IN_SEC + endts.tv_nsec) * s_ticksPerNanoSec);
        uint64_t duration = end - start;
        //printf("%d: duration %llu\n", i, duration);
        sum1 += duration;
    }

    uint64_t sum2 = 0;
    for (i = 0; i < n; i++) {
        uint64_t start, end;
        start = rdtsc();
        slowFunction();
        end = rdtsc();
        uint64_t duration = end - start;

        sum2 += duration;
    }
    
    printf("Sample average value %f\n", (double) sum1 / n);
    printf("Measured average value %f\n", (double) sum2 / n);

    // Test duration of wrapper mechanism 
    /*void (*f1)() = (void(*)())dlsym(p, "emptyFunction");
    
    for (i = 0; i < n; i++) {
       printf("%d: ", i); f1(i);
    }*/

    dlclose(p);
    return 0;
}
