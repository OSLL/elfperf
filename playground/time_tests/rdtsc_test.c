#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <stdint.h>


uint64_t rdtsc()
{
    unsigned int hi, lo;
    asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) hi << 32) | lo;
}

#define NANOSECONDS_IN_SEC 1000000000
static double s_ticksPerNanoSec;


int main()
{
    int n = 1000;


    size_t cpuMask = 1;
    sched_setaffinity(0, sizeof(cpuMask), &cpuMask);

    uint64_t start = rdtsc();
    cpuMask = 2;
    sched_setaffinity(0, sizeof(cpuMask), &cpuMask);
    uint64_t end = rdtsc();

    printf("%lld \n", end - start);

    int i;
    return 0;
}
