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
 * \file ld-routines.h
 * \brief Common functions and data structures for glibc and libelfperf
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#ifndef LD_ROUTINES_H_
#define LD_ROUTINES_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>
#include "config.h"


#define ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE "ELFPERF_PROFILE_FUNCTION"
#define ELFPERF_ENABLE_VARIABLE "ELFPERF_ENABLE"

#define STATS_LIMIT 10000000

#define ELFPERF_SHARED_MEMORY_ID_STAT 2190
#define ELFPERF_SHARED_MEMORY_ID_INFO 2811
#define ELFPERF_SHARED_MEMORY_ID_REDIRECTOR_CONTEXT 3000

#ifdef LIBELFPERF
#include <stdio.h>
#define _dl_debug_printf printf
#endif

#ifdef ELFPERF_ARCH_32

    #define REDIRECTOR_WORDS_SIZE 4
    #define REDIRECTOR_SIZE 16
    #define FCN_PTR_OFFSET 3

#elif defined ELFPERF_ARCH_64  

    #define REDIRECTOR_WORDS_SIZE 3 
    #define REDIRECTOR_SIZE 24 
    #define FCN_PTR_OFFSET 4 

#endif

//Preventing console output from ld.so
#ifdef NO_CONSOLE_OUTPUT_LD_SO

#define _dl_debug_printf debug_print_stub
#define _dl_error_printf debug_print_stub
#define printf debug_print_stub

static int debug_print_stub(const char* fmt, ...) 
{
    return 1;
}

#endif

struct FunctionStatistic
{
    uint64_t totalDiffTime;                   // Total time of function calls
    unsigned long long int totalCallsNumber;  // Total number of functions calls
    void* realFuncAddr;                       // Address of the function
};


struct WrappingContext
{
    // Pointer on previous context in list
    struct WrappingContext* old; 
    // Real return address
    void* realReturnAddr;
    // Pointer on wrapped function
    void* functionPtr;
    // Function start time
    uint64_t startTime;
    // Function end time
    uint64_t endTime; 
};


// Stores all data for redirectors initialization and
// libelfperf functions proper work in ld.so runtime
struct RedirectorContext 
{
    char ** names;
    unsigned int count;
    void * redirectors;
};


// Stores addresses of libelfperf functions
struct ElfperfFunctions 
{
    void (* wrapper)();
    void (* initWrapperRedirectors)(struct RedirectorContext*);
    void (* addNewFunction)(char* , void *, struct RedirectorContext);
    bool (* isFunctionRedirectorRegistered)(char*, struct RedirectorContext);
    bool (* isFunctionInFunctionList)(char*, struct RedirectorContext);
    void * (* getRedirectorAddressForName)(char*, struct RedirectorContext);
};


// Represents all data needed for using libelfperf.so inside libdl.so
struct ElfperfContext
{
    struct ElfperfFunctions addresses;
    struct RedirectorContext context;
    struct FunctionInfo * infos;
};


// Stores function's name-address pair
struct FunctionInfo
{
    char* name;
    void* addr;
};

#ifdef ELFPERF_ARCH_64

extern char **environ;

/*
 * Our realization of getenv. For some reasons getenv doesnt work for x64
 */
static char* our_getenv_(const char* name)
{
    char** env = environ;
    for (; *env; ++env) {
        // Find variable which starts with @name@ str
        if (strstr(*env, name) == *env ) {
            // Locate position of '=' in the string
            char * dataPosition = strchr(*env, '=');
            if ( dataPosition != NULL ) {
                return dataPosition+1;
            } else {
                // No '=' occurs in the *env - return NULL
                return NULL;
            }
        }
    }
        
    return NULL;
}
#define getenv our_getenv_
#endif

/*
 * Returns 1 if evnvironment variables ELFPERF_ENABLE and ELFPERF_PROFILE_FUNCTION are set
 */
static bool isElfPerfEnabled()
{
	//_dl_debug_printf("\t\t ENV %s %s\n", getenv(ELFPERF_ENABLE_VARIABLE), getenv(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE));
    return getenv(ELFPERF_ENABLE_VARIABLE) != NULL 
           && 
           getenv(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE) != NULL;
}


/* 
 * Returns list of function names separted by ":" passed from @env_name@ envoironment variable
 * storing number of them into @count@
 */
static char** get_fn_list(const char* env_name, unsigned int* count)
{
    char* env_value = getenv(env_name);

    if (env_value == NULL) {
        *count = 0;
        return NULL;
    }

    char** result = NULL;
    unsigned int word_count = 0;

    unsigned int i;
    int k = 0;
    for (i = 0; i < strlen(env_value); i++) {
        char c = env_value[i + 1];
        if (c == ':' || c == '\0') {
            word_count++;
            result = (char**)realloc(result, sizeof(*result) * word_count);
            char* str = strndup(env_value + k, i - k + 1);
            k = i + 2;
            result[word_count - 1] = str;
        }
    }

    *count = word_count;
    _dl_debug_printf("LD_ROUTINES_LOG: Got %u functions at get_fn_list\n", word_count);

    return result;
}


/*
 * Checks should function be profiled or not
 */ 
static bool isFunctionProfiled(char * name)
{
    static unsigned int count = 0;
    static char** functions = NULL;
    if (functions == NULL) {
        functions = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);
    }
	
    unsigned int i;
    for (i = 0; i < count; i++) {
        if (strcmp(functions[i],name) == 0) 
            return true;
    }
	
    return false;
}


/*
 * Allocates shared memory for array of struct FunctionInfo
 * and fill it with stubs 
 */
static struct FunctionInfo* initFunctionInfoStorage()
{
	// Get function list from env variables	
    char ** names;
    unsigned int count;
    names = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);

	// Shared memory variables
    int shmid;
    struct FunctionInfo** shm;

    shmid = shmget(ELFPERF_SHARED_MEMORY_ID_INFO, sizeof(struct FunctionInfo**), IPC_CREAT | 0666);
    if (shmid < 0) {
        _dl_debug_printf("LD_ROUTINES_LOG: Failed to create shared memory segment for FunctionInfo!(%u)\n", errno);
        return NULL;
    }

    shm = (struct FunctionInfo**)shmat(shmid, NULL, 0);
    if (shm == (struct FunctionInfo**) -1) {
        _dl_debug_printf("LD_ROUTINES_LOG: Failed to attach at shared memory segment for FunctionInfo!(%u)\n", errno);
        return NULL;
    }

    // Shared memory successfully allocated
    // Allocating memory on heap
    struct FunctionInfo* infos = 
	(struct FunctionInfo*)mmap(0, sizeof(struct FunctionInfo)*count, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    unsigned int i = 0;
    for (i = 0; i < count; i++) {
        (infos[i]).name = names[i];
    }

    *shm = infos;
    _dl_debug_printf("LD_ROUTINES_LOG: Success creation of FunctionInfo shared memory segment!\n");	
	
    return *shm;
}


/*
 * Returns pointer to array of struct FunctionInfo
 * from shared memory
 */
static struct FunctionInfo* getFunctionInfoStorage()
{
    // Shared memory variables
    int shmid;
    struct FunctionInfo** shm;

    shmid = shmget(ELFPERF_SHARED_MEMORY_ID_INFO, sizeof(struct FunctionInfo**), IPC_CREAT | 0666);
    if (shmid < 0) {
        _dl_debug_printf("LD_ROUTINES_LOG: Failed to get shared memory segment for FunctionInfo!(%u)\n", errno);
        return NULL;
    }

    shm = (struct FunctionInfo**)shmat(shmid, NULL, 0);
    if (shm == (struct FunctionInfo**) -1) {
        _dl_debug_printf("LD_ROUTINES_LOG: Failed to attach at shared memory segment for FunctionInfo!(%u)\n", errno);
        return NULL;
    }
    _dl_debug_printf("LD_ROUTINES_LOG: Successfuly got address of info storage %x \n", shm);	

    return *shm;
}


/*
 * Returns pointer to info for name @name@ at storage
 */
static struct FunctionInfo* getInfoByName(char* name, struct FunctionInfo* storage, int count)
{
    if (storage == NULL) return NULL;
    int i;

    for (i = 0; i < count ; i++) {
        if ( storage+i != NULL &&  strcmp(storage[i].name,name) == 0) {
            return storage+i;
        }
    }

    return NULL;
}


/* 
 * Returns pointer to info for addr @addr@ at storage
 */
static struct FunctionInfo* getInfoByAddr(void* addr, struct FunctionInfo* storage)
{
    if (storage == NULL) {
        _dl_debug_printf("LD_ROUTINES_LOG: Storage is NULL. Returning NULL info.\n"); 
        return NULL;
    }

    static unsigned int count = 0;
    static bool isFunctionListFetched = 0;

    if (count == 0 || isFunctionListFetched == 0) {
        get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);
    }

    unsigned int i;
	for (i = 0; i < count ; i++) {
    	_dl_debug_printf("LD_ROUTINES_LOG: Checking match for pair %x %x\n",storage[i].addr, addr);
     	_dl_debug_printf("LD_ROUTINES_LOG:                         %s\n",storage[i].name);
        if (  storage[i].addr == addr ) {
            _dl_debug_printf("LD_ROUTINES_LOG: Found match\n");
            return storage+i;
        }
    }

    _dl_debug_printf("LD_ROUTINES_LOG: No matches. Returning NULL.\n");
    return NULL;
}


/*
 * Allocates shared memory for storing of Function Statistics
 * and fill it with stubs 
 */
static struct FunctionStatistic*** initFunctionStatisticsStorage()
{
    // Shared memory variables
    int shmid;
    struct FunctionStatistic*** shm;

    shmid = shmget(ELFPERF_SHARED_MEMORY_ID_STAT, sizeof(struct FunctionStatistic***), IPC_CREAT | 0666);	
    if (shmid < 0) {
        _dl_debug_printf("LD_ROUTINES_LOG: initFunctionStatisticsStorage: Error during shmget");
        return NULL;
    }

    shm = (struct FunctionStatistic***)shmat(shmid, NULL, 0);
    if (shm == (struct FunctionStatistic ***) -1) {
        _dl_debug_printf("LD_ROUTINES_LOG: initFunctionStatisticsStorage: Error during shmat\n");
        return NULL;
    }
	
    *shm = NULL;
		
    _dl_debug_printf("LD_ROUTINES_LOG: Shared memory inited successfuly: shm = %x\n", *shm );
    return shm;
}

/*
 * Returns pointer to array of struct FunctionStatistic
 * from shared memory
 */
static struct FunctionStatistic*** getFunctionStatisticsStorage() 
{
    int shmid;
    struct FunctionStatistic*** shm;

    shmid = shmget(ELFPERF_SHARED_MEMORY_ID_STAT, sizeof(struct FunctionStatistic***), 0666);
    if (shmid < 0 ) {
        _dl_debug_printf("LD_ROUTINES_LOG: getFunctionStatisticsStorage: Erorr during shmget");
        return NULL;
	} 

    shm = (struct FunctionStatistic***)shmat(shmid, NULL, 0);
    if (shm == (struct FunctionStatistic*** ) -1) {
        _dl_debug_printf("LD_ROUTINES_LOG: getFunctionStatisticsStorage: Error during shmat\n");
        return NULL;
    }
    return shm;
}


/*
 * Allocates shared memory for ElfperfContext
 */
static bool initElfperfContextStorage(struct ElfperfContext context)
{
    // Shared memory variables
    int shmid;
    struct ElfperfContext* shm;

    shmid = shmget(ELFPERF_SHARED_MEMORY_ID_REDIRECTOR_CONTEXT, sizeof(struct ElfperfContext* ), IPC_CREAT | 0666);	
    if (shmid < 0) {
        _dl_debug_printf("LD_ROUTINES_LOG: initElfperfContextStorage: Erorr during shmget");
        return false;
    }

    shm = (struct ElfperfContext*)shmat(shmid, NULL, 0);
    if (shm == (struct ElfperfContext*) -1) {
        _dl_debug_printf("LD_ROUTINES_LOG: initElfperfContextStorage: Error during shmat\n");
        return false;
	}
	
    *shm = context;
		
    _dl_debug_printf("LD_ROUTINES_LOG: initElfperfContextStorage: Shared memory inited successfuly: shm = %x\n", *shm );

    return true;
}

/*
 * Returns pointer on ElfperfContext
 * from shared memory
 */
static struct ElfperfContext* getElfperfContextStorage()
{
    int shmid;
    struct ElfperfContext* shm;
    
    shmid = shmget( ELFPERF_SHARED_MEMORY_ID_REDIRECTOR_CONTEXT, sizeof(struct ElfperfContext*), 0666);
    if (shmid < 0) {
        _dl_debug_printf("LD_ROUTINES_LOG: getElfperfContextStorage: Error during shmget");
        return NULL;
    } 

    shm = (struct ElfperfContext*)shmat(shmid, NULL, 0);
    if (shm == (struct ElfperfContext* ) -1) {
        _dl_debug_printf("LD_ROUTINES_LOG: getElfperfContextStorage: Error during shmat\n");
        return NULL;
    }
    return shm;
}

#endif //  LD_ROUTINES_H_
