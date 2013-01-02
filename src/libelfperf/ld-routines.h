#ifndef LD_ROUTINES_H_
#define LD_ROUTINES_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE "ELFPERF_PROFILE_FUNCTION"
#define ELFPERF_ENABLE_VARIABLE "ELFPERF_ENABLE"

#define STATS_LIMIT 100000
#define ELFPERF_SHARED_MEMORY_ID 2701

struct FunctionStatistic
{
    uint64_t totalDiffTime;			// Total time of function calls
    unsigned long long int totalCallsNumber;	// Total number of functions calls
    void* realFuncAddr;            		// Address of the function
};


struct WrappingContext{
    // real return address
    void * realReturnAddr; 		// 4bytes
    // content of -4(%%old_ebp)
    void * oldEbpLocVar; 		// 4bytes
    // function return value
    void * eax;				// 4bytes
    double doubleResult;		// 8bytes
    void * functionPointer;		// 4bytes
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

#endif
