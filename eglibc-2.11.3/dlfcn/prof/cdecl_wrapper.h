#ifndef _CDECL_WRAPPER_H_
#define _CDECL_WRAPPER_H_
#include <time.h>
#include <stdint.h>
// Number of contexts will be allocated
#define CONTEXT_PREALLOCATED_NUMBER 100000

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
	uint64_t startTime; 	// function starting time
	uint64_t endTime;	// function ending time
};


void setFunctionPointer(void * pointer);

void wrapper();

#endif
