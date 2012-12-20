/* On-demand PLT fixup for shared objects.
   Copyright (C) 1995-2009, 2010, 2011 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#define IN_DL_RUNTIME 1		/* This can be tested in dl-machine.h.  */

#include <alloca.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <ldsodefs.h>
#include <sysdep-cancel.h>
#include "dynamic-link.h"
#include <tls.h>

//////////////////////////////////////////////////////////////////////////////////////// 
#include <sched.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>    /* for PAGESIZE */

// Number of contexts will be allocated
#define CONTEXT_PREALLOCATED_NUMBER 1000

// FIXME doesnt support recoursive calls  because of global stack variables usage
struct WrappingContext{
    // real return address
    void * realReturnAddr; 		// 4bytes
    // content of -4(%%old_ebp)
    void * oldEbpLocVar; 		// 4bytes
    // function return value
    void * eax;			// 4bytes
    double doubleResult;		// 8bytes
    void * functionPointer;		// 4bytes
    struct timespec startTime; 	// function starting time
    struct timespec endTime;	// function ending time
};

void setFunctionPointer(void * pointer);
static struct FunctionStatistic* getFunctionStatistic(void *realFuncAddr);
static struct FunctionStatistic* addNewStat(void *funcAddr, struct timespec diffTime);

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif
#define REDIRECTOR_WORDS_SIZE 4

#define ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE "ELFPERF_PROFILE_FUNCTION"
#define ELFPERF_ENABLE_VARIABLE "ELFPERF_ENABLE"

static const int MAX_SLOTS = 3;

#define STATS_LIMIT 100000
// Global array of functions statistics
static struct FunctionStatistic* s_stats[STATS_LIMIT];

// Number of statistics
static int s_statsCount = 0;

static bool isElfPerfEnabled()
{
    return getenv(ELFPERF_ENABLE_VARIABLE)!=NULL;
}

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
    //sched_setaffinity(0, sizeof(cpuMask), &cpuMask);
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


// Record function start time into context->startTime
void record_start_time_(void * context)
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

// Spinlock for updateStat
static int updateStatSpinlock=0;

static void updateStat(void* funcAddr, struct timespec diffTime)
{

    struct FunctionStatistic* stat = getFunctionStatistic(funcAddr);
    if (stat != NULL) {


        __sync_fetch_and_add(&(stat->totalCallsNumber), 1);

        // Try to lock
        while(  __sync_fetch_and_add(&updateStatSpinlock,1)!=0);
        //_dl_printf("Waiting inside spinlock\n");

        __time_t result_sec = stat->totalDiffTime.tv_sec;
        long int result_nsec = stat->totalDiffTime.tv_nsec;

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

    } else {
        addNewStat(funcAddr, diffTime);
    }
    _dl_error_printf("Updating statistics %p\n", funcAddr);
}


// Record function end time into context->endTime and
// print the duration of function execution
void record_end_time_(void * context)
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
    _dl_error_printf("Function(%p) duration = %ds %dns\n", cont->functionPointer-3, duration.tv_sec, duration.tv_nsec);

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
    static struct FunctionStatistic fake = {0};
    if (s_statsCount == STATS_LIMIT){
        _dl_error_printf("Statistics buffer is full! Exiting\n");
        return &fake;
    }
    _dl_error_printf("addNewStat\n");
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



// Print stats for all functions
static void printFunctionStatistics(){
    int i;
    for (i = 0; i < s_statsCount; i++){
        struct FunctionStatistic *stat = s_stats[i];
        _dl_error_printf("Statistic for function = %p, total time = %ds %dns, number of calls = %lld\n",
                   stat->realFuncAddr, stat->totalDiffTime.tv_sec,
                   stat->totalDiffTime.tv_nsec,
                   stat->totalCallsNumber);
    }
}


// This function returns address of currently free context
struct WrappingContext * getNewContext_(){

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
        return 0;
        // exit(1);
    }

    //pthread_mutex_unlock(&freeContextNumberLock);
    return context;
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
    /*unsigned int i = 0;
    for (i = 0; i<REDIRECTOR_WORDS_SIZE*sizeof(void*) ; i++){
        //_dl_printf("%02hhx ", redirector[i]);
	
    }*/
    
    _dl_error_printf("Created redirector\n");
}


// Return index of function with name @name@
static unsigned int getFunctionIndex(char* name){
    unsigned int i;
    for (i=0; i<s_count; i++){

        //		_dl_printf("%s, %s\n", s_names[i], name);
        if ( !strcmp(name, s_names[i]) ){
	    _dl_error_printf("return normal index in getFunctionIndex for %s\n", name);
            return i;
	}
    }
    //_dl_error_printf("Returning invalid index in getFunctionIndex for %s\n", name);
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
    _dl_error_printf("getRedirectorAddressForName\n");
    //_dl_printf("getRedirectorAddressForName = %x\n", s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*));
    return s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
}

// Return true, if redirector for function with name = @name@ is already registered
static bool isFunctionRedirectorRegistered(char* name){
    // Gets first byte of redirector
    // check is it 0xba or not
    // 0xba - code for the first byte of each registerred redirector
    void * redirectorAddress = getRedirectorAddressForName(name);
    _dl_error_printf("isFunctionRedirectorRegistered\n");
    if (*((unsigned int *)redirectorAddress) != 0){
	_dl_error_printf("%s is registered\n", name);
    }else
    {
	_dl_error_printf("%s is not registered\n", name);
    }

    return (*((unsigned int *)redirectorAddress) != 0);
}

// Add new function to the list of redirectors
static void addNewFunction(char* name, void * functionAddr){
    _dl_error_printf("addNewFunction\n");
    writeRedirectionCode( getRedirectorAddressForName(name), functionAddr);
}

// Initialize s_redirectors and s_names
static void initWrapperRedirectors(char** names,unsigned int count, void * wrapperAddr){

    _dl_error_printf("Starting initWrapperRedirectors\n");

    s_wrapperAddress = wrapperAddr;

    // Memory allocation
    size_t allocSize = sizeof(void*)*REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1;

    s_redirectors = (void *)malloc(allocSize);
    // Aligning by page border
    _dl_error_printf("Before aligment %x, %x\n", s_redirectors, sizeof(void*) * REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1);
    //_dl_error_printf("Before aligment \n");
    s_redirectors = (void *)(((int) s_redirectors + PAGESIZE-1) & ~(PAGESIZE-1));
    _dl_error_printf("After aligment %x\n", s_redirectors);

    int pagesNum = allocSize/PAGESIZE ;
    _dl_error_printf("Number of memory pages %x\n", pagesNum);

    size_t i = 0;
    for (i = 0; i < pagesNum; i++) {
	_dl_error_printf("Going to do mprotect\n");
        if (mprotect(s_redirectors + PAGESIZE*i, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            _dl_error_printf("Couldn't mprotect");
            //exit(errno);
            return;
        }
    }

    // Set 0 into each redirector first byte
    // This will allow to determine wich one is inited
    _dl_error_printf("Zeroing\n");
    for (i = 0 ; i < sizeof(void*)*REDIRECTOR_WORDS_SIZE*count ; i+=REDIRECTOR_SIZE){
        *((unsigned int*)(s_redirectors+i)) = 0;
    }

    // Dealing with function names
    //
    size_t * sizes = (size_t*) malloc(sizeof(size_t)*count);
    s_count = count;
    for (i = 0 ; i < count ; i++){
	_dl_error_printf("Calling strlen\n");
        sizes[i] = strlen(names[i]);
    }

    s_names = (char**) malloc(sizeof(char*)*count);
    size_t size;
    for (i = 0 ; i < count ; i++){
	_dl_error_printf("Calling malloc\n");
	//size = sizeof(char)*(strlen(names[i])+1);
        s_names[i] = (char*)malloc(sizes[i]+1);
	_dl_error_printf("Calling memcpy\n");
        memcpy(s_names[i], names[i], sizes[i]+1);
	//strcpy(s_names[i], names[i]);
        _dl_error_printf("memcpy result %s, %s\n",s_names[i], names[i]);
    }

}

//////////////////////////////////////////////////////////////////////////////////

#if (!defined ELF_MACHINE_NO_RELA && !defined ELF_MACHINE_PLT_REL) \
    || ELF_MACHINE_NO_REL
# define PLTREL  ElfW(Rela)
#else
# define PLTREL  ElfW(Rel)
#endif

#ifndef VERSYMIDX
# define VERSYMIDX(sym)	(DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGIDX (sym))
#endif

/* The fixup functions might have need special attributes.  If none
   are provided define the macro as empty.  */
#ifndef ARCH_FIXUP_ATTRIBUTE
# define ARCH_FIXUP_ATTRIBUTE
#endif

#ifndef reloc_offset
# define reloc_offset reloc_arg
# define reloc_index  reloc_arg / sizeof (PLTREL)
#endif

#define ELFPERF_LIB_NAME "libhello.so"

/*
  Search for library @libname@'s link_map, and if its exists try to get from it
  symbol with name @symbol_name@.
  @flags@ are domestic var resieved from dl_fixup
  @l@ is link_map from which we start search.

  If nothing found or something went wrong 0 is returned. 
*/
static DL_FIXUP_VALUE_TYPE getSymbolAddrFromLibrary(char * libname, char * symbol_name, 
	struct link_map *l, int flags){

	ElfW(Sym) * sym1 = (ElfW(Sym) *)malloc(sizeof(ElfW(Sym) ));
	lookup_t result1;
	DL_FIXUP_VALUE_TYPE value1 = 0;
//	char* name1 = "hello";
//	char* libname = "libhello.so";
	struct link_map* libhello_map = NULL;
	struct link_map* tmp_map = NULL;

	_dl_error_printf("Doing search for libhello linkmap\n");
	// Finding libhello link_map by string name
	if (strstr(l->l_name,libname) == NULL){
		// If l is not the needed lib search in the previous and next libs 

		// Analyze previous libs
		if (l->l_prev != NULL){
			_dl_error_printf("Doing search for libhello linkmap in PREVIOUS libs\n");

			tmp_map = l;

			while (tmp_map->l_prev != NULL) {
				
				if (strstr(tmp_map->l_name,libname) != NULL){
					_dl_error_printf("Map for libhello found: %s\n", tmp_map->l_name);
					libhello_map = tmp_map;
					break;
				}

				tmp_map = tmp_map->l_prev;
			}

		// Analyze next libs if they exists and we didnt found anything else
		}else if (l->l_next != NULL && libhello_map == NULL){
			_dl_error_printf("Doing search for libhello linkmap in NEXT libs\n");

			tmp_map = l;

			while (tmp_map->l_next != NULL) {
				
				if (strstr(tmp_map->l_name,libname) != NULL){
					_dl_error_printf("Map for libhello found: %s\n", tmp_map->l_name);
					libhello_map = tmp_map;
					break;
				}

				tmp_map = tmp_map->l_next;
			}

		}
		

	}else {
		// l is needed lib
		_dl_error_printf("libhello found. l is libhello.so\n");
		libhello_map = l;
	}

	// If we found something
	if (libhello_map != NULL){
	

		_dl_error_printf("Doing _dl_lookup_symbol_x for hello symbol\n");
		result1 = _dl_lookup_symbol_x (symbol_name, libhello_map, &sym1, libhello_map->l_scope,
					    NULL, ELF_RTYPE_CLASS_PLT, flags, NULL);
		
		if (sym1 !=NULL){
		
			_dl_error_printf("Found not NULL symbol for \"hello\"!\n");
			value1 = DL_FIXUP_MAKE_VALUE (result1,
						   sym1 ? (LOOKUP_VALUE_ADDRESS (result1)
							  + sym1->st_value) : 0);

			if (sym1 != NULL
			    && __builtin_expect (ELFW(ST_TYPE) (sym1->st_info) == STT_GNU_IFUNC, 0))
			  value1 = ((DL_FIXUP_VALUE_TYPE (*) (void)) DL_FIXUP_VALUE_ADDR (value1)) ();
				
		}else{
			
			_dl_error_printf("Error! Found NULL symbol for \"hello\"!\n");
		}
	}else {
		_dl_error_printf("Library not found!\n");
	}

	//free(sym1);
	return value1;
}

/* This function is called through a special trampoline from the PLT the
   first time each PLT entry is called.  We must perform the relocation
   specified in the PLT of the given shared object, and return the resolved
   function address to the trampoline, which will restart the original call
   to that address.  Future calls will bounce directly from the PLT to the
   function.  */

static DL_FIXUP_VALUE_TYPE hella_addr = 0;




#ifndef ELF_MACHINE_NO_PLT
DL_FIXUP_VALUE_TYPE
__attribute ((noinline)) ARCH_FIXUP_ATTRIBUTE
_dl_fixup (
# ifdef ELF_MACHINE_RUNTIME_FIXUP_ARGS
	   ELF_MACHINE_RUNTIME_FIXUP_ARGS,
# endif
	   /* GKM FIXME: Fix trampoline to pass bounds so we can do
	      without the `__unbounded' qualifier.  */
	   struct link_map *__unbounded l, ElfW(Word) reloc_arg)
{
  const ElfW(Sym) *const symtab
    = (const void *) D_PTR (l, l_info[DT_SYMTAB]);
  const char *strtab = (const void *) D_PTR (l, l_info[DT_STRTAB]);

  const PLTREL *const reloc
    = (const void *) (D_PTR (l, l_info[DT_JMPREL]) + reloc_offset);
  const ElfW(Sym) *sym = &symtab[ELFW(R_SYM) (reloc->r_info)];
  void *const rel_addr = (void *)(l->l_addr + reloc->r_offset);
  lookup_t result;
  DL_FIXUP_VALUE_TYPE value;

  char* name = 0;
  int flags;

  /* Sanity check that we're really looking at a PLT relocation.  */
  assert (ELFW(R_TYPE)(reloc->r_info) == ELF_MACHINE_JMP_SLOT);

   /* Look up the target symbol.  If the normal lookup rules are not
      used don't look in the global scope.  */
  if (__builtin_expect (ELFW(ST_VISIBILITY) (sym->st_other), 0) == 0)
    {
      const struct r_found_version *version = NULL;

      if (l->l_info[VERSYMIDX (DT_VERSYM)] != NULL)
	{
	  const ElfW(Half) *vernum =
	    (const void *) D_PTR (l, l_info[VERSYMIDX (DT_VERSYM)]);
	  ElfW(Half) ndx = vernum[ELFW(R_SYM) (reloc->r_info)] & 0x7fff;
	  version = &l->l_versions[ndx];
	  if (version->hash == 0)
	    version = NULL;
	}

      /* We need to keep the scope around so do some locking.  This is
	 not necessary for objects which cannot be unloaded or when
	 we are not using any threads (yet).  */
      flags = DL_LOOKUP_ADD_DEPENDENCY;
      if (!RTLD_SINGLE_THREAD_P)
	{
	  THREAD_GSCOPE_SET_FLAG ();
	  flags |= DL_LOOKUP_GSCOPE_LOCK;
	}

#ifdef RTLD_ENABLE_FOREIGN_CALL
      RTLD_ENABLE_FOREIGN_CALL;
#endif
//	_dl_error_printf("dl-runtime1\n");
      name = strtab + sym->st_name;

//      _dl_error_printf("Doing _dl_lookup_symbol_x for %s\n",name);

      result = _dl_lookup_symbol_x (strtab + sym->st_name, l, &sym, l->l_scope,
				    version, ELF_RTYPE_CLASS_PLT, flags, NULL);

      /* We are done with the global scope.  */
      if (!RTLD_SINGLE_THREAD_P)
	THREAD_GSCOPE_RESET_FLAG ();

#ifdef RTLD_FINALIZE_FOREIGN_CALL
      RTLD_FINALIZE_FOREIGN_CALL;
#endif

	_dl_error_printf("dl-runtime2\n");
      /* Currently result contains the base load address (or link map)
	 of the object that defines sym.  Now add in the symbol
	 offset.  */
      value = DL_FIXUP_MAKE_VALUE (result,
				   sym ? (LOOKUP_VALUE_ADDRESS (result)
					  + sym->st_value) : 0);
//	_dl_error_printf("dl-runtime3\n");
    }
  else
    {
      /* We already found the symbol.  The module (and therefore its load
	 address) is also known.  */
//	_dl_error_printf("dl-runtime4\n");
      value = DL_FIXUP_MAKE_VALUE (l, l->l_addr + sym->st_value);
	_dl_error_printf("dl-runtime5\n");
      result = l;
    }

//	_dl_error_printf("dl-runtime6\n");
  /* And now perhaps the relocation addend.  */
  value = elf_machine_plt_value (l, reloc, value);

//	_dl_error_printf("dl-runtime7\n");
  if (sym != NULL
      && __builtin_expect (ELFW(ST_TYPE) (sym->st_info) == STT_GNU_IFUNC, 0))
    value = ((DL_FIXUP_VALUE_TYPE (*) (void)) DL_FIXUP_VALUE_ADDR (value)) ();

//	_dl_error_printf("dl-runtime8\n");
  /* Finally, fix up the plt itself.  */
  if (__builtin_expect (GLRO(dl_bind_not), 0))
    return value;

            if(!isElfPerfEnabled())
            {
               goto skip_elfperf;
            }


            //            // ELFPERF
            static int initialized = 0;
            goto run;

            wrapper_code:
            asm volatile(
			// Building stack frame
			"push %ebp\n"
			"movl %esp,%ebp"
                        // By the start of wrapper edx contains jump addres of function, which is wrapped
                        "pushl %edx\n"				// Storing wrappedFunction_addr into stack
                        "movl (%ebp), %ebx\n"			// ebx = old_ebp
                        "subl $0x4, %ebx\n"			// ebx = old_ebp - 4
                        "movl (%ebx), %edx\n"			// edx = -4(%old_ebp) = (%ebp)
                        // Storing context pointer into freed space (-4(%old_ebp))
                        "call getNewContext_\n"			// eax = getNewContext()
                        "movl %eax, (%ebx)\n" 			// -4(%old_ebp) = eax
                        "movl %eax, %ebx\n"			// %ebx = &context
                        "movl %edx, 4(%ebx)\n"			//context->oldEbpLocVar = edx
                        // Extracting wrappedFunction_addr from stack and placing it to context
                        "popl 20(%ebx)\n"			// context->functionPointer = wrappedFunction_addr
                        // Changing return address to wrapper_return_point
                        "movl 4(%ebp), %ecx\n"			// Storing real return_addres
                        "movl %ecx, (%ebx) \n"
                        "movl $wrapper_return_point, 4(%ebp)\n" // changing return address for $wrapper_return_point
                        //: : :		// (%ebx) = %eax

                        );

            //elfperf_log("WRAPPED!");

            // memorize old return addres and change it for returning in wrapper()
            // stack variables will be damaged, so i use global variable

            asm volatile(	// Calculate address of WrappingContext
                            "movl (%ebp), %ecx\n"			//  %ecx = old_ebp
                            "movl -4(%ecx), %ebx\n"			//  %ebx = context address
                            // WrapperContext struct layout
                            /*
                                    (%ebx)		realReturnAddress
                                    4(%ebx)	oldEbpLocVar // -4(%old_ebp)
                                    8(%ebx)	eax
                                    12(%ebx) floatFunctionReturnValue

                                */
                            // Start time recording
                            // record_start_time(%ebx)
                            "pushl %ebx\n"				// pushing parameter(context address into stack)
                            "call record_start_time_\n"		//
                            "add $4, %esp\n"			// cleaning stack
                            // Going to wrapped function (context->functionPointer)
                            "jmp 20(%ebx)\n"
                            //: : :
                            );


            // going to wrapped function
            //	asm("jmp %0" : :"r"(getFunctionJmpAddress()));

            // returning back into wrapper()
            // 	memorizing eax value
            asm volatile(
                        // Calculating address of WrappingContext and memorizing return values
                        "wrapper_return_point: movl -4(%ebp), %ebx\n"	// %ebx = & context
                        "movl %eax, 8(%ebx)\n"			// context->eax = %eax
                        "fstpl 0xc(%ebx)\n"			// context->doubleResult = ST0
                        // Measuring time of function execution
                        "pushl %ebx\n"				// pushing context address to stack
                        "call record_end_time_\n"		// calling record_end_time
                        "add $4, %esp\n"			// cleaning allocated memory
                        //	: : :
                        );


            /*	asm(	"wrapper_return_point: movl 0(%ebp), %0\n"
                "movl %2, 0(%ebp)\n"
                "movl %eax, %1 ": "=r"(context_), "=r"(context_->eax):"r"(context_->oldEbp):"%eax");*/

            // Change this call to any needed routine
            //elfperf_log("back to wrapper!");

            // restoring real return_address and eax and return
            /*	asm(	"movl %1, %eax\n"
                "jmp %0" : :"r"(context_->realReturnAddr), "r"(context_->eax) : "%eax");*/

            // Getting context address
            // restoring value of eax and st0
            // returning to caller
            asm volatile(
                        "movl -4(%ebp), %ebx\n"	// get context address
                        "movl 4(%ebx), %edx\n"	// restoring -4(old_ebp)
                        "movl %edx, -4(%ebp)\n"	// edx = context->oldEbpLocVar ; -4(%ebp) = edx
                        "movl 8(%ebx), %eax\n"	// restoring eax
                        "fldl 0xc(%ebx)\n"		// restoring ST0
                        "pushl (%ebx)\n"		// returning to caller - pushing return address to stack
                        "ret\n"// : : :
                        );

            //////////


run:
    if(0==initialized)
    {
	_dl_error_printf("Hello from ELFPERF!\n");
	char *names[]={"hello"};
	unsigned int count = 1;

	//names=get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);
	void *wr = &&wrapper_code;
	//_dl_error_printf("Going to initWrapperRedirectors\n");
	initWrapperRedirectors(names, count, wr);
	initialized = 1;

    }
	  if( isFunctionInFunctionList(name)){
	    _dl_error_printf("Doing routines for ELFPERF\n");
	    if (!isFunctionRedirectorRegistered(name)){
			_dl_error_printf("function %s not registered, adding\n", name);
			addNewFunction(name,(void*) value);

	    }

	    value = (DL_FIXUP_VALUE_TYPE) getRedirectorAddressForName(name);
	  }
      



skip_elfperf:
  if (strcmp(name, "puts") == 0){
	char * c ="PUTS CALLLED\n" ;
	_dl_error_printf("Calling puts with special arg\n");

	((void (*)(const char*))value)(c);

	// Studying how to use _dl_lookup_symbol_x
	
	DL_FIXUP_VALUE_TYPE value1;
	value1 = getSymbolAddrFromLibrary(ELFPERF_LIB_NAME, "hello", l, flags);

	if (value1) ((void (*)(const char*))value1)("calling hello function from dl_fixup");

  } 
//  _dl_error_printf("sizeof(DL_FIXUP_VALUE_TYPE) = %u, sizeof(void*) = %u, sizeof(DL_FIXUP_VALUE_TYPE) = %u \n", sizeof(Elf32_Word), sizeof(void*), sizeof(Elf32_Addr));
  return elf_machine_fixup_plt (l, result, reloc, rel_addr, value);
}
#endif

#if !defined PROF && !defined ELF_MACHINE_NO_PLT && !__BOUNDED_POINTERS__
DL_FIXUP_VALUE_TYPE
__attribute ((noinline)) ARCH_FIXUP_ATTRIBUTE
_dl_profile_fixup (
#ifdef ELF_MACHINE_RUNTIME_FIXUP_ARGS
		   ELF_MACHINE_RUNTIME_FIXUP_ARGS,
#endif
		   struct link_map *l, ElfW(Word) reloc_arg,
		   ElfW(Addr) retaddr, void *regs, long int *framesizep)
{
  void (*mcount_fct) (ElfW(Addr), ElfW(Addr)) = INTUSE(_dl_mcount);

  /* This is the address in the array where we store the result of previous
     relocations.  */
  struct reloc_result *reloc_result = &l->l_reloc_result[reloc_index];
  DL_FIXUP_VALUE_TYPE *resultp = &reloc_result->addr;

  DL_FIXUP_VALUE_TYPE value = *resultp;
  if (DL_FIXUP_VALUE_CODE_ADDR (value) == 0)
    {
      /* This is the first time we have to relocate this object.  */
      const ElfW(Sym) *const symtab
	= (const void *) D_PTR (l, l_info[DT_SYMTAB]);
      const char *strtab = (const char *) D_PTR (l, l_info[DT_STRTAB]);

      const PLTREL *const reloc
	= (const void *) (D_PTR (l, l_info[DT_JMPREL]) + reloc_offset);
      const ElfW(Sym) *refsym = &symtab[ELFW(R_SYM) (reloc->r_info)];
      const ElfW(Sym) *defsym = refsym;
      lookup_t result;

      /* Sanity check that we're really looking at a PLT relocation.  */
      assert (ELFW(R_TYPE)(reloc->r_info) == ELF_MACHINE_JMP_SLOT);

      /* Look up the target symbol.  If the symbol is marked STV_PROTECTED
	 don't look in the global scope.  */
      if (__builtin_expect (ELFW(ST_VISIBILITY) (refsym->st_other), 0) == 0)
	{
	  const struct r_found_version *version = NULL;

	  if (l->l_info[VERSYMIDX (DT_VERSYM)] != NULL)
	    {
	      const ElfW(Half) *vernum =
		(const void *) D_PTR (l, l_info[VERSYMIDX (DT_VERSYM)]);
	      ElfW(Half) ndx = vernum[ELFW(R_SYM) (reloc->r_info)] & 0x7fff;
	      version = &l->l_versions[ndx];
	      if (version->hash == 0)
		version = NULL;
	    }

	  /* We need to keep the scope around so do some locking.  This is
	     not necessary for objects which cannot be unloaded or when
	     we are not using any threads (yet).  */
	  int flags = DL_LOOKUP_ADD_DEPENDENCY;
	  if (!RTLD_SINGLE_THREAD_P)
	    {
	      THREAD_GSCOPE_SET_FLAG ();
	      flags |= DL_LOOKUP_GSCOPE_LOCK;
	    }
	  _dl_error_printf("dl-runtime2\n");
	  result = _dl_lookup_symbol_x (strtab + refsym->st_name, l,
					&defsym, l->l_scope, version,
					ELF_RTYPE_CLASS_PLT, flags, NULL);

	  /* We are done with the global scope.  */
	  if (!RTLD_SINGLE_THREAD_P)
	    THREAD_GSCOPE_RESET_FLAG ();

	  /* Currently result contains the base load address (or link map)
	     of the object that defines sym.  Now add in the symbol
	     offset.  */
	  value = DL_FIXUP_MAKE_VALUE (result,
				       defsym != NULL
				       ? LOOKUP_VALUE_ADDRESS (result)
					 + defsym->st_value : 0);

	  if (defsym != NULL
	      && __builtin_expect (ELFW(ST_TYPE) (defsym->st_info)
				   == STT_GNU_IFUNC, 0))
	    value = ((DL_FIXUP_VALUE_TYPE (*) (void))
		     DL_FIXUP_VALUE_ADDR (value)) ();
	}
      else
	{
	  /* We already found the symbol.  The module (and therefore its load
	     address) is also known.  */
	  value = DL_FIXUP_MAKE_VALUE (l, l->l_addr + refsym->st_value);

	  if (__builtin_expect (ELFW(ST_TYPE) (refsym->st_info)
				== STT_GNU_IFUNC, 0))
	    value = ((DL_FIXUP_VALUE_TYPE (*) (void))
		     DL_FIXUP_VALUE_ADDR (value)) ();

	  result = l;
	}
      /* And now perhaps the relocation addend.  */
      value = elf_machine_plt_value (l, reloc, value);

#ifdef SHARED
      /* Auditing checkpoint: we have a new binding.  Provide the
	 auditing libraries the possibility to change the value and
	 tell us whether further auditing is wanted.  */
      if (defsym != NULL && GLRO(dl_naudit) > 0)
	{
	  reloc_result->bound = result;
	  /* Compute index of the symbol entry in the symbol table of
	     the DSO with the definition.  */
	  reloc_result->boundndx = (defsym
				    - (ElfW(Sym) *) D_PTR (result,
							   l_info[DT_SYMTAB]));

	  /* Determine whether any of the two participating DSOs is
	     interested in auditing.  */
	  if ((l->l_audit_any_plt | result->l_audit_any_plt) != 0)
	    {
	      unsigned int altvalue = 0;
	      struct audit_ifaces *afct = GLRO(dl_audit);
	      /* Synthesize a symbol record where the st_value field is
		 the result.  */
	      ElfW(Sym) sym = *defsym;
	      sym.st_value = DL_FIXUP_VALUE_ADDR (value);

	      /* Keep track whether there is any interest in tracing
		 the call in the lower two bits.  */
	      assert (DL_NNS * 2 <= sizeof (reloc_result->flags) * 8);
	      assert ((LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT) == 3);
	      reloc_result->enterexit = LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT;

	      const char *strtab2 = (const void *) D_PTR (result,
							  l_info[DT_STRTAB]);

	      for (unsigned int cnt = 0; cnt < GLRO(dl_naudit); ++cnt)
		{
		  /* XXX Check whether both DSOs must request action or
		     only one */
		  if ((l->l_audit[cnt].bindflags & LA_FLG_BINDFROM) != 0
		      && (result->l_audit[cnt].bindflags & LA_FLG_BINDTO) != 0)
		    {
		      unsigned int flags = altvalue;
		      if (afct->symbind != NULL)
			{
			  uintptr_t new_value
			    = afct->symbind (&sym, reloc_result->boundndx,
					     &l->l_audit[cnt].cookie,
					     &result->l_audit[cnt].cookie,
					     &flags,
					     strtab2 + defsym->st_name);
			  if (new_value != (uintptr_t) sym.st_value)
			    {
			      altvalue = LA_SYMB_ALTVALUE;
			      sym.st_value = new_value;
			    }
			}

		      /* Remember the results for every audit library and
			 store a summary in the first two bits.  */
		      reloc_result->enterexit
			&= flags & (LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT);
		      reloc_result->enterexit
			|= ((flags & (LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT))
			    << ((cnt + 1) * 2));
		    }
		  else
		    /* If the bind flags say this auditor is not interested,
		       set the bits manually.  */
		    reloc_result->enterexit
		      |= ((LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT)
			  << ((cnt + 1) * 2));

		  afct = afct->next;
		}

	      reloc_result->flags = altvalue;
	      value = DL_FIXUP_ADDR_VALUE (sym.st_value);
	    }
	  else
	    /* Set all bits since this symbol binding is not interesting.  */
	    reloc_result->enterexit = (1u << DL_NNS) - 1;
	}
#endif

      /* Store the result for later runs.  */
      if (__builtin_expect (! GLRO(dl_bind_not), 1))
	*resultp = value;
    }

  /* By default we do not call the pltexit function.  */
  long int framesize = -1;

#ifdef SHARED
  /* Auditing checkpoint: report the PLT entering and allow the
     auditors to change the value.  */
  if (DL_FIXUP_VALUE_CODE_ADDR (value) != 0 && GLRO(dl_naudit) > 0
      /* Don't do anything if no auditor wants to intercept this call.  */
      && (reloc_result->enterexit & LA_SYMB_NOPLTENTER) == 0)
    {
      ElfW(Sym) *defsym = ((ElfW(Sym) *) D_PTR (reloc_result->bound,
						l_info[DT_SYMTAB])
			   + reloc_result->boundndx);

      /* Set up the sym parameter.  */
      ElfW(Sym) sym = *defsym;
      sym.st_value = DL_FIXUP_VALUE_ADDR (value);

      /* Get the symbol name.  */
      const char *strtab = (const void *) D_PTR (reloc_result->bound,
						 l_info[DT_STRTAB]);
      const char *symname = strtab + sym.st_name;

      /* Keep track of overwritten addresses.  */
      unsigned int altvalue = reloc_result->flags;

      struct audit_ifaces *afct = GLRO(dl_audit);
      for (unsigned int cnt = 0; cnt < GLRO(dl_naudit); ++cnt)
	{
	  if (afct->ARCH_LA_PLTENTER != NULL
	      && (reloc_result->enterexit
		  & (LA_SYMB_NOPLTENTER << (2 * (cnt + 1)))) == 0)
	    {
	      unsigned int flags = altvalue;
	      long int new_framesize = -1;
	      uintptr_t new_value
		= afct->ARCH_LA_PLTENTER (&sym, reloc_result->boundndx,
					  &l->l_audit[cnt].cookie,
					  &reloc_result->bound->l_audit[cnt].cookie,
					  regs, &flags, symname,
					  &new_framesize);
	      if (new_value != (uintptr_t) sym.st_value)
		{
		  altvalue = LA_SYMB_ALTVALUE;
		  sym.st_value = new_value;
		}

	      /* Remember the results for every audit library and
		 store a summary in the first two bits.  */
	      reloc_result->enterexit
		|= ((flags & (LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT))
		    << (2 * (cnt + 1)));

	      if ((reloc_result->enterexit & (LA_SYMB_NOPLTEXIT
					      << (2 * (cnt + 1))))
		  == 0 && new_framesize != -1 && framesize != -2)
		{
		  /* If this is the first call providing information,
		     use it.  */
		  if (framesize == -1)
		    framesize = new_framesize;
		  /* If two pltenter calls provide conflicting information,
		     use the larger value.  */
		  else if (new_framesize != framesize)
		    framesize = MAX (new_framesize, framesize);
		}
	    }

	  afct = afct->next;
	}

      value = DL_FIXUP_ADDR_VALUE (sym.st_value);
    }
#endif

  /* Store the frame size information.  */
  *framesizep = framesize;

  (*mcount_fct) (retaddr, DL_FIXUP_VALUE_CODE_ADDR (value));

  return value;
}

#endif /* PROF && ELF_MACHINE_NO_PLT */


#include <stdio.h>
void
ARCH_FIXUP_ATTRIBUTE
_dl_call_pltexit (struct link_map *l, ElfW(Word) reloc_arg,
		  const void *inregs, void *outregs)
{
#ifdef SHARED
  /* This is the address in the array where we store the result of previous
     relocations.  */
  // XXX Maybe the bound information must be stored on the stack since
  // XXX with bind_not a new value could have been stored in the meantime.
  struct reloc_result *reloc_result = &l->l_reloc_result[reloc_index];
  ElfW(Sym) *defsym = ((ElfW(Sym) *) D_PTR (reloc_result->bound,
					    l_info[DT_SYMTAB])
		       + reloc_result->boundndx);

  /* Set up the sym parameter.  */
  ElfW(Sym) sym = *defsym;
  sym.st_value = DL_FIXUP_VALUE_ADDR (reloc_result->addr);

  /* Get the symbol name.  */
  const char *strtab = (const void *) D_PTR (reloc_result->bound,
					     l_info[DT_STRTAB]);
  const char *symname = strtab + sym.st_name;

  struct audit_ifaces *afct = GLRO(dl_audit);
  for (unsigned int cnt = 0; cnt < GLRO(dl_naudit); ++cnt)
    {
      if (afct->ARCH_LA_PLTEXIT != NULL
	  && (reloc_result->enterexit
	      & (LA_SYMB_NOPLTEXIT >> (2 * cnt))) == 0)
	{
	  afct->ARCH_LA_PLTEXIT (&sym, reloc_result->boundndx,
				 &l->l_audit[cnt].cookie,
				 &reloc_result->bound->l_audit[cnt].cookie,
				 inregs, outregs, symname);
	}

      afct = afct->next;
    }
#endif
}
