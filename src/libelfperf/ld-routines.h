#ifndef LD_ROUTINES_H_
#define LD_ROUTINES_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


#define ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE "ELFPERF_PROFILE_FUNCTION"
#define ELFPERF_ENABLE_VARIABLE "ELFPERF_ENABLE"

#define STATS_LIMIT 100000

#define ELFPERF_SHARED_MEMORY_ID_STAT 2190
#define ELFPERF_SHARED_MEMORY_ID_INFO 2811
#define ELFPERF_SHARED_MEMORY_ID_REDIRECTOR_CONTEXT 3000

#ifdef LIBELFPERF
#include <stdio.h>
#define _dl_debug_printf printf
#endif

#include "config.h"

//Preventing console output from ld.so
#ifdef NO_CONSOLE_OUTPUT_LD_SO

#define _dl_debug_printf debug_print_stub
#define _dl_error_printf debug_print_stub
#define printf debug_print_stub

static int debug_print_stub(const char* fmt, ...){
        return 1;
}

#endif

struct FunctionStatistic
{
    uint64_t totalDiffTime;			// Total time of function calls
    unsigned long long int totalCallsNumber;	// Total number of functions calls
    void* realFuncAddr;            		// Address of the function
};


struct WrappingContext{
    // real return address
    void * realReturnAddr; 		// 4bytes	0
    // content of -4(%%old_ebp)
    void * oldEbpLocVar; 		// 4bytes	4
    // function return value
    void * eax;				// 4bytes	8
    double doubleResult;		// 8bytes	12
    void * functionPointer;		// 4bytes	20
    int old_ebx;			// 4bytes	24	
    int old_edx;			// 4bytes	28
    int old_ecx;			// 4bytes	32
    uint64_t startTime; 		// function starting time
    uint64_t endTime;			// function ending time
};

// Store all data for redirectors initialization and
// libelfperf functions proper work in ld.so runtime
struct RedirectorContext{
	char ** names;
	unsigned int count;
	void * redirectors;
};

// Store addresses of libelfperf functions
struct ElfperfFunctions {

	void (* wrapper)();
	void (* initWrapperRedirectors)(struct RedirectorContext*);
	void (* addNewFunction)(char* , void *, struct RedirectorContext);
	bool (* isFunctionRedirectorRegistered)(char*, struct RedirectorContext);
	bool (* isFunctionInFunctionList)(char*, struct RedirectorContext);
	void * (* getRedirectorAddressForName)(char*, struct RedirectorContext);
	void * storage;
};

// Represent all data needed for using libelfperf.so inside libdl.so
struct ElfperfContext{
	struct ElfperfFunctions addresses;
	struct RedirectorContext context;
	struct FunctionInfo * infos;
};


static bool isElfPerfEnabled()
{
    return getenv(ELFPERF_ENABLE_VARIABLE)!=NULL && 
		getenv(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE)!=NULL;
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
    _dl_debug_printf("Got %u functions at get_fn_list\n", word_count);

    return result;
}

// Check should function be profiled or not
static bool isFunctionProfiled(char * name){

	static unsigned int count =0;
	static char** functions = NULL;
	if (functions == NULL) 
		functions = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);
	
	unsigned int i;

	for (i = 0; i < count; i++){
		if (strcmp(functions[i],name) == 0) 
			return true;
	}
	
	return false;
}

////// Function info - store function name-addr pairs

// Store information about function
struct FunctionInfo{
	char* name;
	void* addr;
};

// Allocates shared memory for array of struct FunctionInfo
// and fill it with stubs 
static struct FunctionInfo* initFunctionInfoStorage(){
	
	// Get function list from env variables	
	char ** names;
	int count;
	names = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);

	// Shared memory variables

	int shmid;
	struct FunctionInfo * shm;
	
	if ( (shmid = shmget(ELFPERF_SHARED_MEMORY_ID_INFO, sizeof(struct FunctionInfo )*count , IPC_CREAT | 0666) ) < 0) {
		_dl_debug_printf("Failed to create shared memory segment for FunctionInfo!(%u)\n", errno);
		return NULL;
	}

	if ((shm = shmat(shmid, NULL, 0)) == (struct FunctionInfo *) -1) {
		_dl_debug_printf("Failed to attach at shared memory segment for FunctionInfo!(%u)\n", errno);
		return NULL;	
	}

	/// Shared memory successfuly allocated
	// Allocating memory on heap
//	struct FunctionInfo * infos = (struct FunctionInfo *)malloc(sizeof(struct FunctionInfo )*count); 
	int i;
	// Creating stubs 
	for (i = 0 ; i < count; i++)
	{
		(shm[i]).name = names[i];
	}

//	*shm = infos;
	_dl_debug_printf("Success creation of FunctionInfo shared memory segment!\n");	
	
	return shm;
}
// Return pointer to array of struct FunctionInfo
// from shared memory
static struct FunctionInfo* getFunctionInfoStorage(){
	// Get function list from env variables	
	char ** names;
	int count;
	names = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);

	int shmid;
	struct FunctionInfo * shm;
	
	if ((shmid = shmget(ELFPERF_SHARED_MEMORY_ID_INFO, sizeof(struct FunctionInfo )*count,  0666)) < 0) {
		_dl_debug_printf("Failed to get shared memory segment for FunctionInfo at getFunctionInfoStorage(%u)!\n", errno);
		return NULL;
	}

	if ((shm = shmat(shmid, NULL, 0)) == (struct FunctionInfo *) -1) {
		_dl_debug_printf("Failed to attach at shared memory segment for FunctionInfo at getFunctionInfoStorage(%u)!\n", errno);
		return NULL;	
	}
	_dl_debug_printf("Successfuly got addres of info storage %x \n", shm);	

	return shm;
}

// Return pointer to info for name @name@ at storage
static struct FunctionInfo* getInfoByName(char* name, struct FunctionInfo* storage, int count){

	if (storage == NULL) return NULL;
	int i;

	for (i = 0; i < count ; i++)
	{
		if ( storage+i != NULL &&  strcmp(storage[i].name,name) == 0)
			return storage+i;

	}

	return NULL;
}

// Return pointer to info for addr @addr@ at storage
static struct FunctionInfo* getInfoByAddr(void* addr, struct FunctionInfo* storage){

	if (storage == NULL) return NULL;

	static int count = -1;

	if (count < 0){
		get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &count);
	}

	int i;


	for (i = 0; i < count ; i++)
	{
//		_dl_debug_printf("\tChecking match for pair %u %u\n",storage[i].addr, addr);
		if (  storage[i].addr == addr ){
			
			return storage+i;
		}
	}

	return NULL;

}


static struct FunctionStatistic*** initFunctionStatisticsStorage(){


	// Shared memory variables

	int shmid;
	struct FunctionStatistic *** shm;
	
	if ((shmid = shmget(ELFPERF_SHARED_MEMORY_ID_STAT, sizeof(struct FunctionStatistic *** ), IPC_CREAT | 0666)) < 0) {
		_dl_debug_printf("initFunctionStatisticsStorage: Erorr during shmget");
		return NULL;
	}

	if ((shm = shmat(shmid, NULL, 0)) == (struct FunctionStatistic ***) -1) {
		_dl_debug_printf("initFunctionStatisticsStorage: Error during shmat\n");
		return NULL;
	}
	
	*shm = NULL;
		
	_dl_debug_printf("Shared memory inited successfuly: shm = %x\n", *shm );

	
}


static struct FunctionStatistic*** getFunctionStatisticsStorage(){

	int shmid;
	struct FunctionStatistic*** shm;

	if ((shmid = shmget( ELFPERF_SHARED_MEMORY_ID_STAT, sizeof(struct FunctionStatistic***), 0666)) < 0 ) {
		_dl_debug_printf("getFunctionStatisticsStorage: Erorr during shmget");
		return NULL;
	}  else if ((shm = shmat(shmid, NULL, 0)) == (struct FunctionStatistic*** ) -1) {

		_dl_debug_printf("getFunctionStatisticsStorage: Error during shmat\n");
		return NULL;
	}
	return shm;
}

//////

static bool initElfperfContextStorage(struct ElfperfContext context){


	// Shared memory variables

	int shmid;
	struct ElfperfContext* shm;
	
	if ((shmid = shmget(ELFPERF_SHARED_MEMORY_ID_REDIRECTOR_CONTEXT, sizeof(struct ElfperfContext* ), IPC_CREAT | 0666)) < 0) {
		_dl_debug_printf("initElfperfContextStorage: Erorr during shmget");
		return false;
	}

	if ((shm = shmat(shmid, NULL, 0)) == (struct ElfperfContext*) -1) {
		_dl_debug_printf("initElfperfContextStorage: Error during shmat\n");
		return false;
	}
	
	*shm = context;
		
	_dl_debug_printf("initElfperfContextStorage: Shared memory inited successfuly: shm = %x\n", *shm );

	return true;
}

static struct ElfperfContext* getElfperfContextStorage(){

	int shmid;
	struct ElfperfContext* shm;

	if ((shmid = shmget( ELFPERF_SHARED_MEMORY_ID_REDIRECTOR_CONTEXT, sizeof(struct ElfperfContext*), 0666)) < 0 ) {
		_dl_debug_printf("getElfperfContextStorage: Erorr during shmget");
		return NULL;
	}  else if ((shm = shmat(shmid, NULL, 0)) == (struct ElfperfContext* ) -1) {
		_dl_debug_printf("getElfperfContextStorage: Error during shmat\n");
		return NULL;
	}
	return shm;
}

#endif
