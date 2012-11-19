#include "cdecl_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

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


void reorderBytes(unsigned int number, unsigned char* destination){
	destination[0] = (number >> 24) & 0xFF;
	destination[1] = (number >> 16) & 0xFF;
	destination[2] = (number >>  8) & 0xFF;
	destination[3] =  number        & 0xFF;
}

// Create set of machine instructions
/*
        pushl <function_addr+3>
        jmp *<wrapper_addr>
*/
// and write them to @destination@
void writeRedirectionCode(void * destination, void * functionAddr, void * wrapperAddr, unsigned int number){
	 unsigned char* redirector[REDIRECTOR_WORDS_SIZE*sizeof(void*)];
	// mov $number, %%eax
	redirector[0]=0xb8;
	// reversing byte order
	reorderBytes(number, redirector+1);
/*	redirector[1] = (number >> 24) & 0xFF;
	redirector[2] = (number >> 16) & 0xFF;
	redirector[3] = (number >>  8) & 0xFF;
	redirector[4] =  number        & 0xFF;*/
	
	// mov $wrapper, %ebx
	unsigned int wrapper = (unsigned int)wrapperAddr;
	redirector[5]=0xbb;
	reorderBytes(wrapper, redirector+6);
/*	redirector[6] = (number >> 24) & 0xFF;
        redirector[7] = (number >> 16) & 0xFF;
        redirector[8] = (number >>  8) & 0xFF;
        redirector[9] =  number        & 0xFF;*/

	// jmp *(%ebx)
	redirector[10] = 0x67;
	redirector[11] = 0xFF;
	redirector[12] = 0x23;
	
	// nop nop
	redirector[13] = 0x90;
	redirector[14] = 0x90;
	redirector[15] = 0x90;

	memcpy(destination, (void*)redirector, REDIRECTOR_WORDS_SIZE*sizeof(void*));

}


// Return index of function with name @name@
static unsigned int getFunctionIndex(char* name){
	unsigned int i;
	for (i=0; i<s_count; i++){
		if ( !strcmp(name, s_names[i]) )
			return i;
	}
	return -1;
}

// Return redirector address for function with name @name@
void* getRedirectorAddressForName(char* name){
	int functionIndex = getFunctionIndex(name);
	return &s_redirectors[REDIRECTOR_WORDS_SIZE*functionIndex];
}


// Add new function to the list of redirectors
void addNewFunction(char* name, void * functionAddr, void * wrapperAddr){
	writeRedirectionCode( getRedirectorAddressForName(name), functionAddr, wrapperAddr, getFunctionIndex(name));
}

// Initialize s_redirectors and s_names
void initWrapperRedirectors(char** names,unsigned int count){
	// Memory allocation
	s_redirectors = (void *)malloc(sizeof(void*) * REDIRECTOR_WORDS_SIZE + PAGESIZE-1);
	// Aligning by page border
	s_redirectors = (void *)(((int) s_redirectors + PAGESIZE-1) & ~(PAGESIZE-1));

	if (mprotect(s_redirectors, 1024, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        	perror("Couldn't mprotect");
        	exit(errno);
    	}

	// Dealing with function names
	unsigned int sumSize = 0, i = 0;
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


