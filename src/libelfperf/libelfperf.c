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
 * \file libelfperf.c
 * \brief Implementation of functions for working with wrappers and redirectors
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include <sched.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>    /* for PAGESIZE */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "../timers/global_stats.h"

// Preallocated array of contexts
static struct WrappingContext contextArray[CONTEXT_PREALLOCATED_NUMBER];
// Number of first not used contexts
static int freeContextNumber = 0 ;

/*
 * Returns address of clean context for wrapper.
 */
struct WrappingContext * getNewContext_()
{
    struct WrappingContext * context;

    // Check freeContextNumber
    // atomicly increment the freeContextNumber
    // check if it is greater than CONTEXT_PREALLOCATED_NUMBER
    unsigned int number = __sync_fetch_and_add(&freeContextNumber, 1);
    if (number < CONTEXT_PREALLOCATED_NUMBER) {
        context = &contextArray[number];
    } else {
        printf("Context buffer is full!!! Exiting\n");
        return 0;
    }

    return context;
}


/* 
 * Code of wrapper for functions.
 * Wrapper calls selected function and measures its time.
 */
void wrapper()
{
    /* Stack structure
     * /////top/////
     *    old_edx
     *    old_ebx
     *    old_ecx
     *	  old_ebp
     */
    asm volatile (
        //// Special creation of a stack frame ebp 
        // Because we placed old registers on the ebp we need to start stack frame from this place
        // not from current esp
        "movl %esp, %eax\n"
        "addl $12, %eax\n"          // %ebp = %esp + 12
        "movl %eax, %ebp\n"         // because 12 is a size of saved old_{ecx,ebx,edx}
        ///////////////////
        // By the start of wrapper edx contains jump addres of function, which is wrapped
        "pushl %edx\n"              // Storing wrappedFunction_addr into stack
        "movl (%ebp), %ebx\n"       // ebx = old_ebp
        "subl $0x4, %ebx\n"         // ebx = old_ebp - 4
        "movl (%ebx), %edx\n"       // edx = -4(%old_ebp) = (%ebp)
        // Storing context pointer into freed space (-4(%old_ebp))
	    "call getNewContext_\n"     // eax = getNewContext()
        "movl %eax, (%ebx)\n"       // -4(%old_ebp) = eax
        "movl %eax, %ebx\n"         // %ebx = &context
        "movl %edx, 4(%ebx)\n"      //context->oldEbpLocVar = edx
        // Extracting wrappedFunction_addr from stack and placing it to context
        "popl 20(%ebx)\n"           // context->functionPointer = wrappedFunction_addr
        // Memorization of registers states
        "pop 24(%ebx)\n"            // context.old_ebx = old_ebx
        "pop 28(%ebx)\n"            // context.old_edx = old_edx
        "pop 32(%ebx)\n"            // context.old_ecx = old_ecx

        // Changing return address to wrapper_return_point
        "movl 4(%ebp), %ecx\n"			// Storing real return_addres
        "movl %ecx, (%ebx) \n"
        "movl $wrapper_return_point, 4(%ebp)\n" // changing return address for $wrapper_return_point
    );

    // memorize old return addres and change it for returning in wrapper()
    // stack variables will be damaged, so i use global variable
    asm volatile (  // Calculate address of WrappingContext
        "movl (%ebp), %ecx\n"           //  %ecx = old_ebp
        "movl -4(%ecx), %ebx\n"         //  %ebx = context address
        // WrapperContext struct layout
        /*
		    (%ebx)      realReturnAddress
			4(%ebx)     oldEbpLocVar    // -4(%old_ebp)
			8(%ebx)     eax
			12(%ebx)    floatFunctionReturnValue
        */
        // Start time recording
	    // record_start_time(%ebx)
        /* Commented call of record_start_time_*/
        "pushl %ebx\n"                  // pushing parameter(context address into stack)
        "call record_start_time\n"
        "add $4, %esp\n"                // cleaning stack
        // Going to wrapped function (context->functionPointer)
        "jmp 20(%ebx)\n"
    );

    asm volatile (
        // Calculating address of WrappingContext and memorizing return values
        "wrapper_return_point: movl -4(%ebp), %ebx\n"   // %ebx = & context
        "movl %eax, 8(%ebx)\n"                          // context->eax = %eax
        "fstpl 0xc(%ebx)\n"                             // context->doubleResult = ST0
        // Measuring time of function execution
        /* Commented call of record_end_time_ */
        "pushl %ebx\n"                                  // pushing context address to stack
        "call record_end_time\n"                        // calling record_end_time
        "add $4, %esp\n"                                // cleaning allocated memory
    );

    // Getting context address
    // restoring value of eax and st0
    // returning to caller
    asm volatile (
        "movl -4(%ebp), %ebx\n" // get context address
        "movl 4(%ebx), %edx\n"  // restoring -4(old_ebp)
        "movl %edx, -4(%ebp)\n" // edx = context->oldEbpLocVar ; -4(%ebp) = edx
        "movl 8(%ebx), %eax\n"  // restoring eax
        "fldl 0xc(%ebx)\n"      // restoring ST0
        // Restoring registers
        "movl 28(%ebx), %edx\n" // restoring edx
        "movl 32(%ebx), %ecx\n" // restoring ecx
        "pushl (%ebx)\n"        // returning to caller - pushing return address to stack
        // Restoring %ebx
        "movl 24(%ebx), %ebx\n" // %ebx = old_ebx
        "ret\n"
    );
}


/*
 * Creates set of machine instructions
 * ///////////////////////////////////
 *  push %ebp
 *  push %ecx
 *  push %edx
 *  push %ebx
 *  mov fcnPtr+3, %edx
 *  mov wrapper_addr+3, %ebx
 *  jmp *(%ebx)
 * ///////////////////////////////////
 * and writes them to @destination@
 */
void writeRedirectionCode(unsigned char * redirector, void * fcnPtr)
{
    unsigned int functionPointer = (unsigned int )(fcnPtr)+3;

    // push %ebp
    // push %ecx
    // push %edx
    // push %ebx
    redirector[0] = 0x55;
    redirector[1] = 0x51; 
    redirector[2] = 0x52; 
    redirector[3] = 0x53; 

    // mov fcnPtr+3, %edx
    redirector[4]=0xba; //0xb8;
    
    // reversing byte order
    redirector[8] = (functionPointer >> 24) & 0xFF;
    redirector[7] = (functionPointer >> 16) & 0xFF;
    redirector[6] = (functionPointer >>  8) & 0xFF;
    redirector[5] =  functionPointer        & 0xFF;

    // mov $wrapper+3, %ebx
    // Skip stack frame construction - because we pass some extra params throw stack
    // and esp will not be good addr for new stack frame
    unsigned int wrapper_ = (unsigned int)&wrapper + 3;
    redirector[9] = 0xbb;
    redirector[13] = (wrapper_ >> 24) & 0xFF;
    redirector[12] = (wrapper_ >> 16) & 0xFF;
    redirector[11] = (wrapper_ >>  8) & 0xFF;
    redirector[10] =  wrapper_        & 0xFF;

    // jmp *(%ebx)
    redirector[14] = 0xFF;
    redirector[15] = 0xE3; //0x23;
    
    printf("Created redirector for %p at %p, wrapper = %p, %x\n", fcnPtr, redirector, wrapper, wrapper_);
}


/*
 * Returns index of function with name @name@
 */
unsigned int getFunctionIndex(char* name, struct RedirectorContext context)
{
    unsigned int i;
    for (i = 0; i < context.count; i++) {
        printf("Testing %s for %s\n", context.names[i], name);
        if (!strcmp(name, context.names[i])) {
            printf("Index of %s is %x\n", name, i);
            return i;
        }
    }
    printf("Returning invalid index in getFunctionIndex for %s\n", name);
    return context.count + 1;
}


/*
 * Returns true, if function name exists into s_names array
 */
bool isFunctionInFunctionList(char* name, struct RedirectorContext context)
{
    printf("Doing isFunctionInFunctionList\n");
    unsigned int index = getFunctionIndex(name, context);
    return (index != (context.count + 1));
}


/*
 * Returns redirector address for function with name @name@
 */
void* getRedirectorAddressForName(char* name, struct RedirectorContext context)
{
    int functionIndex = getFunctionIndex(name, context);
    printf("getRedirectorAddressForName\n");
    return context.redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
}


/*
 * Return true, if redirector for function with name = @name@ is already registered
 */
bool isFunctionRedirectorRegistered(char* name, struct RedirectorContext context)
{
    // Gets first byte of redirector
    // check is it 0 or not
    // 0 - code for the first byte of each unregisterred redirector
    void * redirectorAddress = getRedirectorAddressForName(name, context);
    printf("isFunctionRedirectorRegistered\n");
    if (*((unsigned int *)redirectorAddress) != 0) {
        printf("%s is registered\n", name);
    } else {
        printf("%s is not registered\n", name);
    }

    return ((*((unsigned int *)redirectorAddress)) != 0);
}


/*
 * Adds new function to the redirectors list
 */
void addNewFunction(char* name, void * functionAddr, struct RedirectorContext context){
    printf("addNewFunction %s\n", name);
    writeRedirectionCode(getRedirectorAddressForName(name, context), functionAddr);
}


/*
 * Initializes s_redirectors and s_names
 */
void initWrapperRedirectors(struct RedirectorContext *context)
{
    initFunctionInfoStorage();

    printf("Starting initWrapperRedirectors %p\n", context);

    // Memory allocation
    size_t allocSize = sizeof(void*) * REDIRECTOR_WORDS_SIZE * context->count + PAGESIZE - 1;
    size_t i = 0;
    context->redirectors = (void *)malloc(allocSize);

    // Aligning by page border
    printf("Before aligment %x, %x\n", context->redirectors, sizeof(void*) * REDIRECTOR_WORDS_SIZE*context->count + PAGESIZE-1);
    context->redirectors = (void *)(((int) context->redirectors + PAGESIZE-1) & ~(PAGESIZE-1));
    int pagesNum = allocSize/PAGESIZE ;
    printf("Number of memory pages %x\n", pagesNum);

    for (i = 0; i < pagesNum; i++) {
        printf("Going to do mprotect\n");
        if (mprotect(context->redirectors + PAGESIZE*i, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            printf("Couldn't mprotect");
            //exit(errno);
            return;
        }
    }

    // Set 0 into each redirector first byte
    // This will allow to determine wich one is inited
    printf("Zeroing\n");
    for (i = 0 ; i < sizeof(void*) * REDIRECTOR_WORDS_SIZE * context->count; i += REDIRECTOR_SIZE) {
        *((unsigned int*)(context->redirectors+i)) = 0;
    }
}
