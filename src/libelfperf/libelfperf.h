#ifndef LIBELFPERF_H_
#define LIBELFPERF_H_

#include <stdbool.h>
#include "ld-routines.h"

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#define REDIRECTOR_WORDS_SIZE 4
#define REDIRECTOR_SIZE 16

#define ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE "ELFPERF_PROFILE_FUNCTION"
#define ELFPERF_ENABLE_VARIABLE "ELFPERF_ENABLE"

#define CONTEXT_PREALLOCATED_NUMBER 1000


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
