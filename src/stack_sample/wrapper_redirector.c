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
 * \file wrapper_redirector.h
 * \brief Implementation of functions for array-of-redirectors generation
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "wrapper_redirector.h"
#include "cdecl_wrapper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
//static void * s_functionPointers;
static int s_count;
static void * s_wrapperAddress;

/*void * getFunctionPointer(unsigned int number){
	if (number < s_count) return s_functionPointers[number];
	return NULL;
}*/

// Create set of machine instructions
/*
        mov fcnPtr+3, %eax
	mov wrapper_addr, %ebx
        jmp *(%ebx)
*/
// and write them to @destination@
void writeRedirectionCode(unsigned char * redirector, void * fcnPtr){
//	 unsigned char* redirector[REDIRECTOR_WORDS_SIZE*sizeof(void*)];
	// mov $number, %%eax
	// ret = 0xc3
	unsigned int functionPointer = (unsigned int )(fcnPtr)+3;
	redirector[0]=0xb8;
	// reversing byte order
	redirector[4] = (functionPointer >> 24) & 0xFF;
	redirector[3] = (functionPointer >> 16) & 0xFF;
	redirector[2] = (functionPointer >>  8) & 0xFF;
	redirector[1] =  functionPointer        & 0xFF;
	//memcpy(redirector+1, &number, 4);
	
	// mov $wrapper, %ebx
	//wrapperAddress = wrapperAddr;
    unsigned int wrapper = (unsigned int)&s_wrapperAddress;
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
	//printf("Written gateaway to %x, size %d\n", redirector, REDIRECTOR_WORDS_SIZE*sizeof(void*));
    /*unsigned int i = 0;
    for (i = 0; i<REDIRECTOR_WORDS_SIZE*sizeof(void*) ; i++){
		printf("%02hhx ", redirector[i]);
    }*/
    //printf("\nfcn_ptr = %x, destination = %x\n", fcnPtr, redirector);
}


// Return index of function with name @name@
static unsigned int getFunctionIndex(char* name)
{
	unsigned int i;
	for (i=0; i<s_count; i++){

//		printf("%s, %s\n", s_names[i], name);
        if ( !strcmp(name, s_names[i]) )
            return i;
	}
	return 0;
}

// Return redirector address for function with name @name@
void* getRedirectorAddressForName(char* name)
{
    int functionIndex = getFunctionIndex(name);
    void * result = s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
    //printf("getRedirectorAddressForName = %x\n", (unsigned int)result);
    return result;
}


// Add new function to the list of redirectors
void addNewFunction(char* name, void * functionAddr)
{
    writeRedirectionCode(getRedirectorAddressForName(name), functionAddr);
}

// Initialize s_redirectors and s_names
void initWrapperRedirectors(char** names,unsigned int count, void * wrapperAddr)
{
	s_wrapperAddress = wrapperAddr;

	// Memory allocation
    size_t allocSize = sizeof(void*)*REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1;
    s_redirectors = (void *)malloc(allocSize);
	// Aligning by page border
    printf("Before alignment %x, %d\n", (unsigned int)s_redirectors, allocSize);
    s_redirectors = (void *)(((int) s_redirectors + PAGESIZE-1) & ~(PAGESIZE-1));
    printf("After alignment %x\n", (unsigned int)s_redirectors);


    int pagesNum = allocSize/PAGESIZE + 1;
    printf("Number of memory pages %d\n", pagesNum);

    int i = 0;
    for (i = 0; i < pagesNum; i++) {
        if (mprotect(s_redirectors + PAGESIZE*i, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            perror("Couldn't mprotect");
            exit(errno);
        }
    }

	// Dealing with function names
    unsigned int sumSize = 0;
	s_count = count;
	for (i = 0 ; i < count ; i++){
		sumSize += strlen(names[i])+1;
	}

	s_names = (char**) malloc(sizeof(char*)*count);
	for (i = 0 ; i < count ; i++){
		s_names[i] = (char*)malloc(sizeof(char)*(strlen(names[i])+1));
		memcpy(s_names[i], names[i], sizeof(char)*(strlen(names[i])+1));
        //printf("%s, %s\n",s_names[i], names[i]);
	}

}
