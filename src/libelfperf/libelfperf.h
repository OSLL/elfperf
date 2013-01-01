#ifndef LIBELFPERF_H_
#define LIBELFPERF_H_

#include <stdbool.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#define REDIRECTOR_WORDS_SIZE 4
#define REDIRECTOR_SIZE 16

#define ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE "ELFPERF_PROFILE_FUNCTION"
#define ELFPERF_ENABLE_VARIABLE "ELFPERF_ENABLE"

#define CONTEXT_PREALLOCATED_NUMBER 1000

// FIXME doesnt support recoursive calls  because of global stack variables usage
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



struct WrappingContext * getNewContext_();


void wrapper();

void writeRedirectionCode(unsigned char * redirector, void * fcnPtr);

unsigned int getFunctionIndex(char* name, struct RedirectorContext context);

bool isFunctionInFunctionList(char* name, struct RedirectorContext context);

void* getRedirectorAddressForName(char* name, struct RedirectorContext context);

bool isFunctionRedirectorRegistered(char* name, struct RedirectorContext context);

void addNewFunction(char* name, void * functionAddr, struct RedirectorContext context);

void initWrapperRedirectors( struct RedirectorContext *context);





#endif
