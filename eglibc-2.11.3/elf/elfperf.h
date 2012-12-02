#ifndef ELFPERF
#define ELFPERF

#include <sched.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>    /* for PAGESIZE */

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif
#define REDIRECTOR_WORDS_SIZE 4

#define ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE "ELFPERF_PROFILE_FUNCTION"

static const int MAX_SLOTS = 3;


// Return list of function names separted by ":" passed from @env_name@ envoironment variable
// storing number of them into @count@
static char** get_fn_list(const char* env_name, int* count)
{
    char* env_value = getenv(env_name);

    if (env_value == NULL) {
        *count = 0;
        return NULL;
    }

    char** result = NULL;
    int word_count = 0;

    int i;
    int k = 0;
    for (i = 0; i < strlen(env_value); i++) {
        char c = env_value[i + 1];
        if (c == ':' || c == '\0') {
            word_count++;
            result = (char**)realloc(result, sizeof(*result) * word_count);
            char* str = strndup(env_value + k, i - k + 1);
            k = i+2;
            result[word_count - 1] = str;
        }
    }

    *count = word_count;
    return result;
}




static void * functionPointer = NULL;
//static pthread_mutex_t functionPointerLock = PTHREAD_MUTEX_INITIALIZER;

// Workaround - storing context in global variable
//static struct WrappingContext context;

// preallocated array of contexts
static struct WrappingContext contextArray[CONTEXT_PREALLOCATED_NUMBER];
// number of first not used context
static int freeContextNumber = 0 ;
//static pthread_mutex_t freeContextNumberLock = PTHREAD_MUTEX_INITIALIZER;


#define TIMING_WITH_RDTSC

static struct FunctionStatistic
{
    struct timespec totalDiffTime; // Total time of function calls
    unsigned long long int totalCallsNumber;
    void* realFuncAddr;            // Address of the function
};

#define NANOSECONDS_IN_SEC 1000000000
// Calculated ticksPerNanoSec = 10^-9 * F(Hz) = F(GHz)
static double s_ticksPerNanoSec = 2;

// Assembly code for reading number of CPU ticks from TSC (Time Stamp Counter) register
static inline uint64_t rdtsc()
{
    unsigned int high, low;
    asm volatile("rdtsc" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}

// Calculate difference between two timespecs
static struct timespec diffTimeSpec(struct timespec start, struct timespec end)
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

// This function is used for calibrating the number of CPU cycles per nanosecond
static void calibrateTicks()
{
    /*struct timespec start_ts, end_ts;
    uint64_t start = 0, end = 0;
    clock_gettime(CLOCK_MONOTONIC, &start_ts);
    start = rdtsc();
    uint64_t i;
    for (i = 0; i < 1000000; i++);
    end = rdtsc();
    clock_gettime(CLOCK_MONOTONIC, &end_ts);
    struct timespec elapsed_ts = diffTimeSpec(start_ts, end_ts);
    uint64_t elapsed_nsec = elapsed_ts.tv_sec * 1000000000LL + elapsed_ts.tv_nsec;
    s_ticksPerNanoSec = (double)(end - start) / (double)elapsed_nsec;*/
}

// This function should be called before using rdtsc(),
// has side effect of binding to CPU0.
static void initRdtsc()
{
    size_t cpuMask = 1;
    sched_setaffinity(0, sizeof(cpuMask), &cpuMask);
    calibrateTicks();
}



// Get elapsed time in timespecs for given time in nanoseconds
static void getTimeSpec(struct timespec *ts, uint64_t nsecs)
{
    ts->tv_sec = nsecs / NANOSECONDS_IN_SEC;
    ts->tv_nsec = nsecs % NANOSECONDS_IN_SEC;
}

// Get elapsed time in timespecs using time converted from TSC reading
static void getRdtscTime(struct timespec* ts)
{
    getTimeSpec(ts, rdtsc()/s_ticksPerNanoSec);
}


static struct timespec diff(struct timespec start, struct timespec end)
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

#define STATS_LIMIT 100000
// Global array of functions statistics
static struct FunctionStatistic* s_stats[STATS_LIMIT];
// Number of statistics
static int s_statsCount = 0;

// Record function start time into context->startTime
static void record_start_time(void * context)
{
    struct WrappingContext * cont = (struct WrappingContext *)context;

#ifdef TIMING_WITH_HPET
    _dl_printf("LOG: get start time with HPET\n");
    cont->startTime = get_accurate_time();
#endif

#ifdef TIMING_WITH_RDTSC
    //    _dl_printf("LOG: get start time with RDTSC\n");
    initRdtsc();
    getRdtscTime(&cont->startTime);
#endif

}

// Record function end time into context->endTime and
// print the duration of function execution
static void record_end_time(void * context)
{
    struct WrappingContext * cont = (struct WrappingContext *)context;

#ifdef TIMING_WITH_HPET
    //    _dl_printf("LOG: get end time with HPET\n");
    cont->endTime = get_accurate_time();
#endif

#ifdef TIMING_WITH_RDTSC
    //_dl_printf("LOG: get end time with RDTSC\n");
    getRdtscTime(&cont->endTime);
#endif

    struct timespec duration = diff(cont->startTime, cont->endTime);
    //    _dl_printf("Function(%p) duration = %ds %dns\n", cont->functionPointer-3, duration.tv_sec, duration.tv_nsec);

    // Updating statistic for function
    updateStat(cont->functionPointer - 3, duration);
}

// Get statistic for given function
static struct FunctionStatistic* getFunctionStatistic(void *realFuncAddr)
{
    int i;
    for (i = 0; i < s_statsCount; i++) {
        if (s_stats[i]->realFuncAddr == realFuncAddr)
            return s_stats[i];
    }

    return NULL;
}


static struct FunctionStatistic* addNewStat(void *funcAddr, struct timespec diffTime)
{
    if (s_statsCount == STATS_LIMIT){
        _dl_printf("Statistics buffer is full! Exiting\n");
        exit(1);
    }

    // atomicly increment s_statsCount
    //  unsigned int number = __sync_fetch_and_add(&s_statsCount, 1);

    struct FunctionStatistic* stat = (struct FunctionStatistic*)malloc(sizeof(struct FunctionStatistic));

    stat->realFuncAddr = funcAddr;
    stat->totalCallsNumber = 1;
    stat->totalDiffTime.tv_sec = diffTime.tv_sec;
    stat->totalDiffTime.tv_nsec = diffTime.tv_nsec;
    s_stats[__sync_fetch_and_add(&s_statsCount, 1)] = stat;

    return stat;
}

// Spinlock for updateStat
static int updateStatSpinlock=0;

void __updateStat(void* funcAddr, struct timespec diffTime)
{

    struct FunctionStatistic* stat = getFunctionStatistic(funcAddr);
    if (stat != NULL) {


//        __sync_fetch_and_add(&(stat->totalCallsNumber), 1);

//        // Try to lock
//        while(  __sync_fetch_and_add(&updateStatSpinlock,1)!=0);
//        //_dl_printf("Waiting inside spinlock\n");

//        __time_t result_sec = stat->totalDiffTime.tv_sec;
//        long int result_nsec = stat->totalDiffTime.tv_nsec;
#if 0
        // Atomicly add diffTime.tv_sec to stat->totalDiffTime.tv_sec
        //__sync_fetch_and_add(&(stat->totalDiffTime.tv_sec), diffTime.tv_sec);
        result_sec += diffTime.tv_sec;
        result_nsec += diffTime.tv_nsec;

        if (result_nsec >= 1000000000) {
            result_sec += 1;
            result_nsec = result_nsec - 1000000000;
        }

        //	_dl_printf("Going to update stat: old %ds %dns, addition %ds %dns \n",
        //		stat->totalDiffTime.tv_sec, stat->totalDiffTime.tv_nsec, result_sec, result_nsec);
        // Atomicly increment the stat->totalCallsNumber
        stat->totalDiffTime.tv_nsec = (long int)result_nsec;
        stat->totalDiffTime.tv_sec = result_sec;

        // Unlock
        updateStatSpinlock = 0;
#endif
    } else {
      //  addNewStat(funcAddr, diffTime);
    }

}


// Print stats for all functions
static void _dl_printfunctionStatistics(){
    int i;
    for (i = 0; i < s_statsCount; i++){
        struct FunctionStatistic *stat = s_stats[i];
        _dl_printf("Statistic for function = %p, total time = %ds %dns, number of calls = %lld\n",
               stat->realFuncAddr, stat->totalDiffTime.tv_sec,
               stat->totalDiffTime.tv_nsec,
               stat->totalCallsNumber);
    }
}


// This function returns address of currently free context
static struct WrappingContext * getNewContext(){

    struct WrappingContext * context;


    //pthread_mutex_lock(&freeContextNumberLock);
    // Check freeContextNumber
    // atomicly increment the freeContextNumber
    // check if it is greater than CONTEXT_PREALLOCATED_NUMBER

    unsigned int number = __sync_fetch_and_add(&freeContextNumber, 1);
    if (number < CONTEXT_PREALLOCATED_NUMBER){
        context = &contextArray[number];
    } else {
        _dl_printf("Context buffer is full!!! Exiting\n");
        exit(1);
    }

    //pthread_mutex_unlock(&freeContextNumberLock);
    return context;
}




static void elfperf_log(const char *msg)
{
    static int c=0;
    _dl_printf("[ELFPERF:%d] %s\n",c++,msg);
}


// Array which contains redirectors
static void * s_redirectors;
// Array of function names, the same indexing as in redirectors
static char** s_names;
//static void * s_functionPointers;
static int s_count;
static void * s_wrapperAddress;

#define REDIRECTOR_SIZE 16

// Create set of machine instructions
/*
        mov fcnPtr+3, %eax
    mov wrapper_addr, %ebx
        jmp *(%ebx)
*/
// and write them to @destination@
static void writeRedirectionCode(unsigned char * redirector, void * fcnPtr){
    //	 unsigned char* redirector[REDIRECTOR_WORDS_SIZE*sizeof(void*)];
    // mov $number, %%edx
    // ret = 0xc3
    unsigned int functionPointer = (unsigned int )(fcnPtr)+3;
    redirector[0]=0xba;//0xb8;
    // reversing byte order
    redirector[4] = (functionPointer >> 24) & 0xFF;
    redirector[3] = (functionPointer >> 16) & 0xFF;
    redirector[2] = (functionPointer >>  8) & 0xFF;
    redirector[1] =  functionPointer        & 0xFF;
    //memcpy(redirector+1, &number, 4);

    // mov $wrapper, %ebx
    //wrapperAddress = wrapperAddr;
    unsigned int wrapper = &s_wrapperAddress;
    redirector[5]=0xbb;
    redirector[9] = (wrapper >> 24) & 0xFF;
    redirector[8] = (wrapper >> 16) & 0xFF;
    redirector[7] = (wrapper >>  8) & 0xFF;
    redirector[6] =  wrapper        & 0xFF;

    // jmp *(%ebx)
    redirector[10] = 0xFF;
    redirector[11] = 0x23;
    redirector[12] = 0x90;

    // nop nop
    redirector[13] = 0x90;
    redirector[14] = 0x90;
    redirector[15] = 0x90;

    //	memcpy(destination, redirector, REDIRECTOR_WORDS_SIZE*sizeof(void*));
    //_dl_printf("Written gateaway to %x, size %d\n", redirector, REDIRECTOR_WORDS_SIZE*sizeof(void*));
    unsigned int i = 0;
    for (i = 0; i<REDIRECTOR_WORDS_SIZE*sizeof(void*) ; i++){
        _dl_printf("%02hhx ", redirector[i]);
    }
    _dl_printf("\nfcn_ptr = %x, destination = %x\n", fcnPtr, redirector);
}


// Return index of function with name @name@
static unsigned int getFunctionIndex(char* name){
    unsigned int i;
    for (i=0; i<s_count; i++){

        //		_dl_printf("%s, %s\n", s_names[i], name);
        if ( !strcmp(name, s_names[i]) )
            return i;
    }
    return s_count+1;
}

// Return true, if function name exists into s_names array
static bool isFunctionInFunctionList(char* name){
    return getFunctionIndex(name) != s_count+1;
}



// Return redirector address for function with name @name@
static void* getRedirectorAddressForName(char* name){
    int functionIndex = getFunctionIndex(name);
    void * result = s_redirectors +  functionIndex;
    _dl_printf("getRedirectorAddressForName = %x\n", s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*));
    return s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
}

// Return true, if redirector for function with name = @name@ is already registered
static bool isFunctionRedirectorRegistered(char* name){
    // Gets first byte of redirector
    // check is it 0xba or not
    // 0xba - code for the first byte of each registerred redirector
    void * redirectorAddress = getRedirectorAddressForName(name);
    return (*((unsigned int *)redirectorAddress) != 0);
}

// Add new function to the list of redirectors
static void addNewFunction(char* name, void * functionAddr){
    writeRedirectionCode( getRedirectorAddressForName(name), functionAddr);
}

// Initialize s_redirectors and s_names
static void initWrapperRedirectors(char** names,unsigned int count, void * wrapperAddr){
    s_wrapperAddress = wrapperAddr;

    // Memory allocation
    size_t allocSize = sizeof(void*)*REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1;

    s_redirectors = (void *)malloc(allocSize);
    // Aligning by page border
    _dl_printf("Before aligment %x, %x\n", s_redirectors, sizeof(void*) * REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1);
    s_redirectors = (void *)(((int) s_redirectors + PAGESIZE-1) & ~(PAGESIZE-1));
    _dl_printf("After aligment %x\n", s_redirectors);

    int pagesNum = allocSize/PAGESIZE + 1;
    _dl_printf("Number of memory pages %d\n", pagesNum);

    unsigned int i = 0;
    for (i = 0; i < pagesNum; i++) {
        if (mprotect(s_redirectors + PAGESIZE*i, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            perror("Couldn't mprotect");
            exit(errno);
        }
    }

    // Set 0 into each redirector first byte
    // This will allow to determine wich one is inited
    for (i = 0 ; i < sizeof(void*)*REDIRECTOR_WORDS_SIZE*count ; i+=REDIRECTOR_SIZE){
        *((unsigned int*)(s_redirectors+i)) = 0;
    }

    // Dealing with function names
    unsigned int sumSize = 0;
    s_count = count;
    for (i = 0 ; i < count ; i++){
        sumSize += strlen(names[i])+1;
    }

    s_names = (char**) malloc(sizeof(char*)*count);
    for (i = 0 ; i < count ; i++){
        s_names[i] = (char*)malloc(sizeof(char)*(strlen(names[i])+1));
        memcpy(s_names[i], names[i], sizeof(char)*(strlen(names[i])+1));
        _dl_printf("%s, %s\n",s_names[i], names[i]);
    }

}

//#################################################
#endif
