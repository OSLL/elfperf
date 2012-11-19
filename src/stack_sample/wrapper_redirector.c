#include "cdecl_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>    /* for PAGESIZE */
#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#define REDIRECTOR_WORDS_SIZE 4

// This file contains code for array-of-redirectors generation
// Redirector structure
/*
	pushl <function_addr+3>
	jmp *<wrapper>
*/
// After reaching wrapper, address will be poped from stack

// Array which contains redirectors
static void * s_redirectors;
// Array of function names, the same indexing as in redirectors
static char** s_names;
static int s_count;
// Create set of machine instructions
/*
        pushl <function_addr+3>
        jmp *<wrapper_addr>
*/
// and write them to @destination@
void writeRedirectionCode(void * destination, void * functionAddr, void * wrapperAddr, int number){
	void * redirector[REDIRECTOR_WORDS_SIZE];
	// push $number
	redirector[0]=0x68;
	// reversing byte order
	redirector[1] = (number >> 24) & 0xFF;
	redirector[2] = (number >> 16) & 0xFF;
	redirector[3] = (number >>  8) & 0xFF;
	redirector[4] =  number        & 0xFF;
	
	

}

// Return redirector address for function with name @name@
void* getRedirectorAddressForName(char* name){
	int functionIndex = getFunctionIndex(name);
	return s_redirectors[REDIRECTOR_WORDS_SIZE*functionIndex];
}

// Return index of function with name @name@
static int getFunctionIndex(char* name){
	int i;
	for (int i=0; i<s_count; i++){
		if ( !strcmp(name, s_name[i]) )
			return i;
	}
	return -1;
}

// Add new function to the list of redirectors
void addNewFunction(char* name, void * functionAddr, void * wrapperAddr){
	writeRedirectionCode( getRedirectorAddressForName(name), functionAddr, wrapperAddr);
}

// Initialize s_redirectors and s_names
void initWrapperRedirectors(char** names, int count){
	// Memory allocation
	s_redirectors = (void *)malloc(sizeof(void*) * REDIRECTOR_WORDS_SIZE + PAGESIZE-1);
	// Aligning by page border
	s_redirectors = (void *)(((int) s_redirectors + PAGESIZE-1) & ~(PAGESIZE-1));

	if (mprotect(s_redirectors, 1024, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        	perror("Couldn't mprotect");
        	exit(errno);
    	}

	// Dealing with function names
	int sumSize = 0, i = 0;
	s_count = count;
	for (i = 0 ; i < count ; i++){
		sumSize += strlen(names[i])+1;
	}

	s_names = (char**) malloc(sizeof(char*)*sumSize);
	for (i = 0 ; i < count ; i++){
		s_names[i] = (char*)malloc(sizeof(char)*(strlen(names[i])+1));
		memcpy(s_names[i], names[i], sizeof(char)*(strlen(names[i])+1));
	}

	
	

}


