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

extern void testFunc(char * a);

#define REAL_RETURN_ADDR_STUB 1

// preallocated array of contexts
static struct WrappingContext contextArray[CONTEXT_PREALLOCATED_NUMBER];
// Number of first not used contexts
static int freeContextNumber = 0 ;

/*
 * Returns address of clean context for wrapper.
 */
static int getNewContextSpinlock=0;

// This function returns address of currently free context
struct WrappingContext * getNewContext_()
{
    struct WrappingContext * context;

    unsigned int i = 0;
    bool isFreeContextFound = 0;
    /// Critical section start
    while(  __sync_fetch_and_add(&getNewContextSpinlock,1)!=0);
    // printf("Inside critical section\n");
    // Search for not used Contexts - the ones which has contextArray[i].realReturnAddr == NULL
    for (i = 0; i < CONTEXT_PREALLOCATED_NUMBER; i++) {
        printf("%u : %p\n", i,  contextArray[i].realReturnAddr);
        // Free context found - mark it as used
        // i contains number of free context
        if (contextArray[i].realReturnAddr == NULL) {
            contextArray[i].realReturnAddr = (void*)REAL_RETURN_ADDR_STUB;
            isFreeContextFound = 1;
            break;
        }
    }
    getNewContextSpinlock = 0;
    // Critical section ends
    
    // No free contexts - terminating app
    if (!isFreeContextFound) {
        printf("Context buffer is full!!! Exiting\n");
        exit(1);
    }

    return contextArray + i;

/*    //pthread_mutex_lock(&freeContextNumberLock);
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

    //pthread_mutex_unlock(&freeContextNumberLock);
    return context;*/
}



/* 
 * Code of wrapper for optimized functions which doesn't create new stack frame.
 */
/*void wrapper2()
{
    /* Stack structure
     * /////top/////
     *    old_ebx
     *    old_edx
     *    old_ecx
     *	  old_ebp
     */
    // In this wrapper we don't need to create new stack frame

    // Allocating memory for new context and filling it.
/*    asm volatile (
      	"pushl  %edx\n"    
        "call   getNewContext_\n"       // %eax = getNewContext()
        "popl   %edx\n"
        "movl   %eax, %ebx\n"           // %ebx = &context
        "movl   %edx, 20(%ebx)\n"       // context.functionPointer = wrapperFunction_addr
        "popl   24(%ebx)\n"             // context.old_ebx = old_ebx
        "popl   28(%ebx)\n"             // context.old_edx = old_edx
        "popl   32(%ebx)\n"             // context.old_ecx = old_ecx
        "popl   4(%ebx)\n"              // context.oldEbpLocVar = old_ebp
    );

    // Saving real return address in memory and changing it with fake address inside of wrapper.
    // Address of %ebp will be stored inside of context.
    // And address of context will be inserted instead of %ebp.
    asm volatile (  // Calculate address of WrappingContext
        "movl   %ebp, 4(%ebx)\n"        // context.oldEbpLocVar = %ebp
        "movl   %ebx, %ebp\n"           // %ebp = &context
        "popl   (%ebx)\n"               // pop (return value) >> context.realReturnAddr 

        "pushl  %ebx\n"                 // pushing context address into stack
        "call   record_start_time\n"    // calculating of start time and storing it inside of context
        "popl   %ebx\n"                 // restoring %ebx value
 
        "pushl  $wrapper2_ret_point\n"  // adding new return value for wrapped function 
        "movl   20(%ebx), %edx\n"       // %edx = functionPointer
        "subl   $3, %edx\n"             // %edx = %edx - 3 -- because wrapped function don't create stack frame
                                        // and we don't need pass 3 first bytes of function
        "jmp    %edx\n"                 // jumping to wrapper function
    );

    // Restoring of %ebp state and saving return values inside of context.
    asm volatile (
        "wrapper2_ret_point:\n"
      
        // Restoring %ebp value
        "movl   %ebp, %ebx\n"           // %ebx = %context from %ebp
        "movl   4(%ebx), %ebp\n"        // %ebp = context.oldEbpLocVar
        "movl   %eax, 8(%ebx)\n"        // context->eax = %eax
        "fstpl  0xc(%ebx)\n"            // context->doubleResult = ST0
        
        // Measuring time of function execution
        "pushl  %ebx\n"                 // pushing context address to stack
        "call   record_end_time\n"      // calling record_end_time
        "popl   %ebx\n"                 // restoring %ebx value 
    );

    // Restoring value of %eax and st0.
    // Restoring real return address and returning to caller
    asm volatile (
        "movl   8(%ebx), %eax\n"	// restoring eax
        "fldl   0xc(%ebx)\n"        // restoring ST0
        // Restoring registers
        "movl   28(%ebx), %edx\n"   // restoring edx
        "movl   32(%ebx), %ecx\n"   // restoring ecx    
        "pushl  (%ebx)\n"           // returning to caller - pushing return address to stack
        // Marking context as free and restoring %ebx
        "pushl  24(%ebx)\n" 	    // old_ebx to stack
        "movl   $0, (%ebx)\n"       // context.realReturnAddr = 0
        "popl   %ebx\n"             // %ebx = old_ebx
        "ret\n"
    );
}
*/

/*
 * Wrapper for functions which creates stack frame.
 */
void wrapper()
{
    /* Stack structure
     * /////top/////
     *	  old_rbp
     *  return_addr
     *   arg n - 1
     *      ...
     *     arg 6
     */
    // arguments from 0 to 5 are in %rdi, %rsi, %rdx, %rcx, %r8 and %r9
    asm volatile (
        // Save value of old_ebp
        "mov    (%rbp), %rbx\n"
        // Save values of needed registers
        "push   %rax\n"
        "push   %rbx\n"
        "push   %rsi\n"
        "push   %rdi\n"
        "push   %rcx\n"
        "push   %rdx\n"
        "push   %r8\n"
        "push   %r9\n"
        // push xmm0-7 on stack
        "movdqu %xmm0, (%rsp)\n"
        "sub    $16, %rsp\n"
        "movdqu %xmm1, (%rsp)\n"
        "sub    $16, %rsp\n"
        "movdqu %xmm2, (%rsp)\n"
        "sub    $16, %rsp\n"
        "movdqu %xmm3, (%rsp)\n"
        "sub    $16, %rsp\n"
        "movdqu %xmm4, (%rsp)\n"
        "sub    $16, %rsp\n"
        "movdqu %xmm5, (%rsp)\n"
        "sub    $16, %rsp\n"
        "movdqu %xmm6, (%rsp)\n"
        "sub    $16, %rsp\n"
        "movdqu %xmm7, (%rsp)\n"
        "sub    $16, %rsp\n"
        // Get new context for call
        "call   getNewContext_\n"   // rax = getNewContext
        "mov    %rax, %r15\n"        // r15 = &context
        // pop xmm7-0 from stack
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm7\n"
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm6\n"
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm5\n"
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm4\n"
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm3\n"
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm2\n"
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm1\n"
        "add    $16, %rsp\n"
        "movdqu (%rsp), %xmm0\n"
        // Restore registers state
        "pop    %r9\n"
        "pop    %r8\n"
        "pop    %rdx\n"
        "pop    %rcx\n"
        "pop    %rdi\n"
        "pop    %rsi\n"
        "pop    %rbx\n"
        "pop    %rax\n" 
        // Extract context content from stack and registers
        "mov    8(%rbp), %r11\n"    // r11 = return address
        "mov    %r11, (%r15)\n"     // context->realReturnAddress = r11
        "mov    %rax, 8(%r15)\n"    // context->functionPointer = rbx
        "mov    %rbx, 32(%r15)\n"   // context->callerLocVar = rbx
        // Save address of context as local variable in caller
        "mov    -8(%rbx), %rax\n"   // caller 1st local var = rax
        "mov    %rax, 32(%r15)\n"   // context->callerLocalVar = rax
        "mov    %r15, -8(%rbx)\n"   // caller 1st local var = &context
        // Save XMM registers
        "movdqu %xmm0, 112(%r15)\n" // context->xmm0 = xmm0;
        "movdqu %xmm1, 128(%r15)\n" // context->xmm0 = xmm0;
        "movdqu %xmm2, 144(%r15)\n" // context->xmm0 = xmm0;
        "movdqu %xmm3, 160(%r15)\n" // context->xmm0 = xmm0;
        "movdqu %xmm4, 176(%r15)\n" // context->xmm0 = xmm0;
        "movdqu %xmm5, 192(%r15)\n" // context->xmm0 = xmm0;
        "movdqu %xmm6, 208(%r15)\n" // context->xmm0 = xmm0;
        "movdqu %xmm7, 224(%r15)\n" // context->xmm0 = xmm0;
        // Change real return address on label inside of wrapper
        "movq   $wrapper_ret_point, 0x8(%rbp)\n"
    );

    asm volatile (
        // Save registers before record_start_time call
        "push   %rax\n"   
        "push   %rsi\n"
        "push   %rdi\n"
        "push   %rcx\n"
        "push   %rdx\n"
        "push   %r8\n"
        "push   %r9\n"
        "push   %r15\n"
        // Call record_start_time
        "mov    %r15, %rdi\n"       // arg0 for record_start_time in rdi
        "call   record_start_time\n"
        // Restore registers state
        "pop    %r15\n"
        "pop    %r9\n"
        "pop    %r8\n"
        "pop    %rdx\n"
        "pop    %rcx\n"
        "pop    %rdi\n"
        "pop    %rsi\n"
        "pop    %rax\n"
        // Save registers inside of context
        //"mov    %rbp, 40(%r15)\n"   // context->rbp = %rbp
        //"mov    %rax, 48(%r15)\n"   // context->rax = %rax
        "mov    %rbx, 56(%r15)\n"   // context->rbx = %rbx
        "mov    %rcx, 64(%r15)\n"   // context->rcx = %rcx
        "mov    %rdx, 72(%r15)\n"   // context->rdx = %rdx
        "mov    %rdi, 80(%r15)\n"   // context->rdi = %rdi
        "mov    %rsi, 88(%r15)\n"   // context->rsi = %rsi
        "mov    %r8, 96(%r15)\n"    // context->r8 = %r8
        "mov    %r9, 104(%r15)\n"   // context->r9 = %r9
        // Restore XMM registers
        "movdqu 112(%r15), %xmm0\n"
        "movdqu 128(%r15), %xmm1\n"
        "movdqu 144(%r15), %xmm2\n"
        "movdqu 160(%r15), %xmm3\n"
        "movdqu 176(%r15), %xmm4\n"
        "movdqu 192(%r15), %xmm5\n"
        "movdqu 208(%r15), %xmm6\n"
        "movdqu 224(%r15), %xmm7\n"
        // Jump to wrapped function        
        "jmp    8(%r15)\n"          // jmp context->functionPointer
    );

    asm volatile (
        "wrapper_ret_point:\n"  
        // Here we return after execution of wrapping function.
        // After returning at this point %rbp contains value of old_rbp.
        // Restore context address from caller local variable        
        "mov    -8(%rbp), %r15\n"   // %r15 = 1st local var of caller (&context)
        "mov    32(%r15), %rbx\n"   // %rbx = saved value of 1st local var of caller
        "mov    %rbx, -8(%rbp)\n"   // 1st local var of caller = old value
        // Save return values of wrapped function
        "mov    %rax, 16(%r15)\n"   // context->integerResult = %rax
        "movsd  %xmm0, 24(%r15)\n"

        // Restore registers' state from context
        //"mov    48(%r15), %rax\n"   // %rax = context->rax
        "mov    56(%r15), %rbx\n"   // %rbx = context->rbx
        "mov    64(%r15), %rcx\n"   // %rcx = context->rcx
        "mov    72(%r15), %rdx\n"   // %rdx = context->rdx
        "mov    80(%r15), %rdi\n"   // %rdi = context->rdi
        "mov    88(%r15), %rsi\n"   // %rsi = context->rsi
        "mov    96(%r15), %r8\n"    // %r8 = context->r8
        "mov    104(%r15), %r9\n"   // %r9 = context->r9
        // Save registers before record_end_time call
        "push   %r15\n"
        // Call record_end_time
        "mov    %r15, %rdi\n"       // %rdi = &context (arg0 for record_end_time)
        "call   record_end_time\n"
        // Restore registers state
        "pop    %r15\n"
        // Preparing to exit from wrapper
        "mov    16(%r15), %rax\n"   // %rax = context->integerResult
        "push   (%r15)\n"           // restore real return address, i.e. push context->realReturnAddr
        "movq   $0, (%r15)\n"       // context->realReturnAddr = 0
        "movsd  24(%r15), %xmm0\n"  // %xmm0 = context->floatingPointResult
        "ret\n"
    );
}


/*
 * Creates set of machine instructions
 * ///////////////////////////////////
 *  mov fcnPtr+4, %rax
 *  mov wrapper_addr, %ebx
 *  jmp %rbx
 * ///////////////////////////////////
 * and writes them to @destination@
 */
void writeRedirectionCode(unsigned char * redirector, void * fcnPtr)
{
    
    uint64_t functionPointer = (uint64_t)(fcnPtr)+4;

    // mov fcnPtr+3, %eax     48 b8 ...
    redirector[0] = 0x48;
    redirector[1] = 0xb8;
    
    // writing function pointer bytes in reversed order
    redirector[9] = (functionPointer >> 56) & 0xFF;
    redirector[8] = (functionPointer >> 48) & 0xFF;
    redirector[7] = (functionPointer >> 40) & 0xFF;
    redirector[6] = (functionPointer >> 32) & 0xFF;
    redirector[5] = (functionPointer >> 24) & 0xFF;
    redirector[4] = (functionPointer >> 16) & 0xFF;
    redirector[3] = (functionPointer >>  8) & 0xFF;
    redirector[2] =  functionPointer        & 0xFF;

    // mov $wrapper+4, %rbx
    // Skip stack frame construction - because we pass some extra params throw stack
    // and esp will not be good addr for new stack frame
/*    unsigned int wrapper_;

    // Check first 3 bytes of function and choose appropriate wrapper.
    printf("First 3 bytes of function: \n");
    printf("\t%x %x %x\n",
        ((unsigned int)((void**)fcnPtr)[0]) & 0xFF,
        (((unsigned int)((void**)fcnPtr)[0]) >> 8) & 0xFF,
        (((unsigned int)((void**)fcnPtr)[0]) >> 16) & 0xFF
    );

    if ( (((unsigned int)((void**)fcnPtr)[0]) & 0xFF) == 0x55           // 1st byte
         &&
         ((((unsigned int)((void**)fcnPtr)[0]) >> 8) & 0xFF) == 0x89    // 2nd byte
         &&
         ((((unsigned int)((void**)fcnPtr)[0]) >> 16) & 0xFF) == 0xe5 ) // 3rd byte
    {
        printf("ELFPERF_DEBUG: Chosen 1st wrapper for redirector\n");
        wrapper_ = (unsigned int)&wrapper + 3;
    } else {
        wrapper_ = (unsigned int)&wrapper2 + 3;
        printf("ELFPERF_DEBUG: Chosen 2nd wrapper for redirector\n");
    }
*/
    /////
    uint64_t wrapper_ = (uint64_t)&wrapper;
    /////

    // mov wrapper, %edx     48 bb ...
    redirector[10] = 0x48;
    redirector[11] = 0xbb;

    redirector[19] = (wrapper_ >> 56) & 0xFF;
    redirector[18] = (wrapper_ >> 48) & 0xFF;
    redirector[17] = (wrapper_ >> 40) & 0xFF;
    redirector[16] = (wrapper_ >> 32) & 0xFF;
    redirector[15] = (wrapper_ >> 24) & 0xFF;
    redirector[14] = (wrapper_ >> 16) & 0xFF;
    redirector[13] = (wrapper_ >>  8) & 0xFF;
    redirector[12] =  wrapper_        & 0xFF;

    // jmp %rbx     ff e3
    redirector[20] = 0xFF;
    redirector[21] = 0xe3;
    
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
