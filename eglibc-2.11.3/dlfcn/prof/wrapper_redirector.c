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



// Create set of machine instructions
/*
        pushl <function_addr+3>
        jmp *<wrapper_addr>
*/
// and write them to @destination@
void writeRedirectionCode(unsigned char * redirector, void * functionAddr, void * wrapperAddr, unsigned int number){
//	 unsigned char* redirector[REDIRECTOR_WORDS_SIZE*sizeof(void*)];
	// mov $number, %%eax
	// ret = 0xc3
	redirector[0]=0xb8;
	// reversing byte order
	redirector[4] = (number >> 24) & 0xFF;
	redirector[3] = (number >> 16) & 0xFF;
	redirector[2] = (number >>  8) & 0xFF;
	redirector[1] =  number        & 0xFF;
	//memcpy(redirector+1, &number, 4);
	
	// mov $wrapper, %ebx
	unsigned int wrapper = (unsigned int)wrapperAddr;
	redirector[5]=0xbb;
	redirector[9] = (wrapper >> 24) & 0xFF;
	redirector[8] = (wrapper >> 16) & 0xFF;
	redirector[7] = (wrapper >>  8) & 0xFF;
	redirector[6] =  wrapper        & 0xFF;

	// jmp *(%ebx)
	redirector[10] = 0x67;
	redirector[11] = 0xFF;
	redirector[12] = 0x23;
	
	// nop nop
	redirector[13] = 0x90;
	redirector[14] = 0x90;
	redirector[15] = 0x90;

//	memcpy(destination, redirector, REDIRECTOR_WORDS_SIZE*sizeof(void*));
	//printf("Written gateaway to %x, size %d\n", redirector, REDIRECTOR_WORDS_SIZE*sizeof(void*));
	unsigned int i = 0;
	for (i = 0; i<REDIRECTOR_WORDS_SIZE*sizeof(void*) ; i++){
		printf("%02hhx ", redirector[i]);
	}
	printf("\nwrapper = %x , number = %x,\n", wrapper, number);
}


// Return index of function with name @name@
static unsigned int getFunctionIndex(char* name){
	unsigned int i;
	for (i=0; i<s_count; i++){

//		printf("%s, %s\n", s_names[i], name);
		if ( !strcmp(name, s_names[i]) )
			return i;
	}
	return 0;
}

// Return redirector address for function with name @name@
void* getRedirectorAddressForName(char* name){
	int functionIndex = getFunctionIndex(name);
	void * result = s_redirectors +  functionIndex;
	return s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
}


// Add new function to the list of redirectors
void addNewFunction(char* name, void * functionAddr, void * wrapperAddr){
	writeRedirectionCode( getRedirectorAddressForName(name), functionAddr, wrapperAddr, getFunctionIndex(name));
}

// Initialize s_redirectors and s_names
void initWrapperRedirectors(char** names,unsigned int count){
	// Memory allocation
	s_redirectors = (void *)malloc(sizeof(void*)*REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1);
	// Aligning by page border
	printf("Before aligment %x, %x\n", s_redirectors, sizeof(void*) * REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1);
	s_redirectors = (void *)(((int) s_redirectors + PAGESIZE-1) & ~(PAGESIZE-1));
	printf("After aligment %x\n", s_redirectors);
	
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

	s_names = (char**) malloc(sizeof(char*)*count);
	for (i = 0 ; i < count ; i++){
		s_names[i] = (char*)malloc(sizeof(char)*(strlen(names[i])+1));
		memcpy(s_names[i], names[i], sizeof(char)*(strlen(names[i])+1));
		printf("%s, %s\n",s_names[i], names[i]);
	}

}


