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
#include <limits.h>    /* for getpagesize() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "global_stats.h"
#include "libelfperf.h"
#include "errno.h"

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


// Context address storage variable
static __thread void* contextStorage = 0;

// This function allows to set value of context storage 
void setContextStorage(void* val)
{
    contextStorage = val;
}

// This function allows to access to context storage 
void* getContextStorage()
{
    return contextStorage;
}


// This function returns address of currently free context
static struct WrappingContext * getNewContext()
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
        printf("LIBELFPERF_LOG: Context buffer is full!!! Exiting\n");
        exit(1);
    }

    setContextStorage(contextArray + i);

    return contextArray + i;
}


#ifdef ELFPERF_ARCH_32


// Thread independent context storage
static __thread struct WrappingContext* s_wrappingContext = NULL;


// Contains steps of preprofiling
static void preProfile(void* returnAddr, void* functionPtr)
{
    struct WrappingContext* context = (struct WrappingContext*)mmap(0, sizeof(struct WrappingContext), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    context->old = s_wrappingContext;
    s_wrappingContext = context;
    context->realReturnAddr = returnAddr;
    context->functionPtr = functionPtr;
    record_start_time(context);
}


// Contains steps of postprofiling
static void* postProfile()
{
    struct WrappingContext* context = s_wrappingContext;
    s_wrappingContext = context->old;
    record_end_time(context);

    void* returnAddr = context->realReturnAddr;
    munmap(context, sizeof(struct WrappingContext));

    return returnAddr;
}


/*
 * Code of wrapper-function for x86
 * Wrapper calls selected function and measures its time
 */ 
void wrapper()
{
    /* Stack structure
     * /////top/////
     *     fcnptr
     *     retaddr
     *    args[n-1]
     *      ...
     *     args[0]
     */
    //   push %ebp
    //   movl %esp, %ebp
    asm volatile (
        "popl   %ebp\n"         // Restore old stack frame base
        "pushal\n"              // Keep all registers while preprofiling 
        // Perform preprofile operations
        "pushl  32(%esp)\n"     // arg[0] = retaddr
        "pushl  40(%esp)\n"     // arg[1] = fcnptr
        "call   preProfile\n"
        "addl   $8, %esp\n"     // Remove function arguments from stack 
        // Change return address of function on wrapper return point
        "movl   $wrapper_rp, 36(%esp)\n"
        "popal\n"               // Restore previous register state
        "retl\n"                // <=> pop fcnptr && jmp fcnptr
    );

    asm volatile (
        "wrapper_rp:\n"
        "pushl  $0\n"           // Allocate space for placing return address in future
        "pushal\n"              // Keep all registers while postprofiling
        // Perform postprofile operations
        "call postProfile\n"
        "movl %eax, 32(%esp)\n" // Restore old return address
        "popal\n"               // Restore registers state
        "retl\n"                // Return to caller
    );
}


/*
 * Creates set of machine instructions
 * ///////////////////////////////////
 *  push $fcnPtr
 *  jmp $wrapper
 * ///////////////////////////////////
 * and writes them to @destination@
 */
void writeRedirectionCode(unsigned char * redirector, void * fcnPtr)
{
    printf("LIBELFPERF_LOG: Writing redirection code for function %x: \n", fcnPtr);

    // pushl $fcnPtr
    redirector[0] = 0x68;
    *(uint32_t*)(redirector + 1) = (uint32_t)fcnPtr;

    // jmp $wrapper
    redirector[5] = 0xe9;
    *(uint32_t*)(redirector + 6) = ((uint32_t)wrapper) - (uint32_t)(redirector + 10);
   
    printf("LIBELFPERF_LOG: Created redirector for %p at %p, wrapper =  %x\n", fcnPtr, redirector, wrapper);
}


#elif defined ELFPERF_ARCH_64


// Thread independent context storage
static __thread struct WrappingContext* s_wrappingContext = NULL;


// Contains steps of preprofiling
static void preProfile(void* returnAddr, void* functionPtr)
{
    struct WrappingContext* context = (struct WrappingContext*)mmap(0, sizeof(struct WrappingContext), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    context->old = s_wrappingContext;
    s_wrappingContext = context;
    context->realReturnAddr = returnAddr;
    context->functionPtr = functionPtr;
    record_start_time(context);
}


// Contains steps of postprofiling
static void* postProfile()
{
    struct WrappingContext* context = s_wrappingContext;
    s_wrappingContext = context->old;
    record_end_time(context);

    void* returnAddr = context->realReturnAddr;
    munmap(context, sizeof(struct WrappingContext));

    return returnAddr;
}


/*
 * Code of wrapper-function for x64
 * Wrapper calls selected function and measures its time
 */ 
void wrapper()
{
    /* Stack structure
     * /////top/////
     *     fcnptr
     *      rax
     *     retaddr
     *   arg n - 1
     *      ...
     *     arg 6
     */
    // arguments from 0 to 5 are in %rdi, %rsi, %rdx, %rcx, %r8 and %r9
    //   push %rbp
    //   movl %rsp, %rbp
    asm volatile (
        "popq    %rbp\n"            // Restore old stack frame base
        // Keep all registers before preprofiling   | fcnPtr offset | fcnPtr address
                                                //  | +0            | %(rsp)
        "pushq  %rdi\n"                         //  | +8            | 32(%rsp)
        "pushq  %rsi\n"                         //  | +8            | 40(%rsp)
        // Prepare arguments for preProfile 
        "movq   32(%rsp), %rdi\n"   // arg[0] = retaddr
        "movq   16(%rsp), %rsi\n"   // arg[1] = fcnptr
        "pushq  %rbx\n"                         //  | +8            | 8(%rsp)                      
        "pushq  %rcx\n"                         //  | +8            | 16(%rsp)
        "pushq  %rdx\n"                         //  | +8            | 24(%rsp)
        "pushq  %r8\n"                          //  | +8            | 48(%rsp)
        "pushq  %r9\n"                          //  | +8            | 56(%rsp)
        "pushq  %r10\n"                         //  | +8            | 64(%rsp)
        "pushq  %r11\n"                         //  | +8            | 72(%rsp)
        "pushq  %r12\n"                         //  | +8            | 80(%rsp)
        "pushq  %r13\n"                         //  | +8            | 88(%rsp)
        "pushq  %r14\n"                         //  | +8            | 96(%rsp)
        "pushq  %r15\n"                         //  | +8            | 104(%rsp)
        // Push xmm0-7 on stack                     
        "sub    $16, %rsp\n"                    //  | +16           | 120(%rsp)
        "movdqu %xmm0, (%rsp)\n"                //  |               |
        "sub    $16, %rsp\n"                    //  | +16           | 136(%rsp)
        "movdqu %xmm1, (%rsp)\n"                //  |               |
        "sub    $16, %rsp\n"                    //  | +16           | 152(%rsp)
        "movdqu %xmm2, (%rsp)\n"                //  |               |
        "sub    $16, %rsp\n"                    //  | +16           | 168(%rsp)
        "movdqu %xmm3, (%rsp)\n"                //  |               |
        "sub    $16, %rsp\n"                    //  | +16           | 184(%rsp)
        "movdqu %xmm4, (%rsp)\n"                //  |               |
        "sub    $16, %rsp\n"                    //  | +16           | 200(%rsp)
        "movdqu %xmm5, (%rsp)\n"                //  |               |
        "sub    $16, %rsp\n"                    //  | +16           | 216(%rsp)
        "movdqu %xmm6, (%rsp)\n"                //  |               |
        "sub    $16, %rsp\n"                    //  | +16           | 232(%rsp)
        "movdqu %xmm7, (%rsp)\n"                //  |               |
        // Perform preprofile operations
        "call   preProfile\n"
        // Restore registers state
        // pop xmm7-0 from stack
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
        "add    $16, %rsp\n"
        // Pop general purpose registers 
        "popq   %r15\n"
        "popq   %r14\n"
        "popq   %r13\n"
        "popq   %r12\n"
        "popq   %r11\n"
        "popq   %r10\n"
        "popq   %r9\n"
        "popq   %r8\n"
        "popq   %rdx\n"
        "popq   %rcx\n"
        "popq   %rbx\n"
        "popq   %rsi\n"
        "popq   %rdi\n"
        // Change return address of function on wrapper return point
        "leaq   wrapper_rp(%rip), %rax\n"
        "movq   %rax, 16(%rsp)\n"
        // Restore %rax
        "popq   %rax\n"
        "xchgq  %rax, (%rsp)\n"
        // Jmp to wrapped function
        "retq\n"                    // <=> pop fcnptr && jmp fcnptr
    );

    asm volatile (
        "wrapper_rp:\n"
        "pushq  $0\n"               // Allocate space for placing return address in future
        // Keep all registers before postprofiling
        "pushq  %rax\n"
        "pushq  %rbx\n"                      
        "pushq  %rcx\n"
        "pushq  %rdx\n"
        "pushq  %rdi\n"
        "pushq  %rsi\n"
        "pushq  %r8\n"
        "pushq  %r9\n"
        "pushq  %r10\n"
        "pushq  %r11\n"
        "pushq  %r12\n"
        "pushq  %r13\n"
        "pushq  %r14\n"
        "pushq  %r15\n"
        // Push xmm0-7 on stack                     
        "sub    $16, %rsp\n"
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
        // Perform postprofile operations
        "call postProfile\n"
        // Restore registers state
        // pop xmm7-0 from stack
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
        "add    $16, %rsp\n"
        // Pop general purpose registers 
        "popq   %r15\n"
        "popq   %r14\n"
        "popq   %r13\n"
        "popq   %r12\n"
        "popq   %r11\n"
        "popq   %r10\n"
        "popq   %r9\n"
        "popq   %r8\n"
        "popq   %rsi\n"
        "popq   %rdi\n"
        "popq   %rdx\n"
        "popq   %rcx\n"
        "popq   %rbx\n"
        // Restore old return address
        "movq  %rax, 8(%rsp)\n"
        // Restore %rax
        "popq   %rax\n"
        "retq\n"                    // Return to caller
    );
}


/*
 * Creates set of machine instructions
 * ///////////////////////////////////
 *  push %rax 
 *  mov $fcnPtr, %rax
 *  push %rax
 *  mov $wrapper, %rax
 *  jmp *%rax
 * ///////////////////////////////////
 * and writes them to @destination@
 */
void writeRedirectionCode(unsigned char * redirector, void * fcnPtr)
{
    printf("LIBELFPERF_LOG: Writing redirection code for function %x: \n", fcnPtr);

    // push %rax 
    redirector[0] = 0x50;
    // mov $fcnPtr, %rax
    redirector[1] = 0x48;
    redirector[2] = 0xb8;
    *(uint64_t*)(redirector + 3) = (uint64_t)fcnPtr;
    // push %rax
    redirector[11] = 0x50;
    // mov $wrapper, %rax
    redirector[12] = 0x48;
    redirector[13] = 0xb8;
    *(uint64_t*)(redirector + 14) = (uint64_t)wrapper;
    // jmp *%rax
    redirector[22] = 0xff;
    redirector[23] = 0xe0;
 
    printf("LIBELFPERF_LOG: Created redirector for %p at %p, wrapper = %p,\n", fcnPtr, redirector, wrapper);
}
#endif

/*
 * Returns index of function with name @name@
 */
unsigned int getFunctionIndex(char* name, struct RedirectorContext context)
{
    unsigned int i;
    for (i = 0; i < context.count; i++) {
        printf("LIBELFPERF_LOG: Testing %s for %s\n", context.names[i], name);
        if (!strcmp(name, context.names[i])) {
            printf("LIBELFPERF_LOG: Index of %s is %x\n", name, i);
            return i;
        }
    }
    printf("LIBELFPERF_LOG: Returning invalid index in getFunctionIndex for %s\n", name);
    return context.count + 1;
}


/*
 * Returns true, if function name exists into s_names array
 */
bool isFunctionInFunctionList(char* name, struct RedirectorContext context)
{
    printf("LIBELFPERF_LOG: Doing isFunctionInFunctionList\n");
    unsigned int index = getFunctionIndex(name, context);
    return (index != (context.count + 1));
}


/*
 * Returns redirector address for function with name @name@
 */
void* getRedirectorAddressForName(char* name, struct RedirectorContext context)
{
    unsigned int functionIndex = getFunctionIndex(name, context);
    uint64_t addr = ( uint64_t )context.redirectors + REDIRECTOR_SIZE * functionIndex;

//    #ifdef ELFPERF_ARCH_32     
    

//    printf("LIBELFPERF_LOG: getRedirectorAddressForName\n");
    return (void*)addr;
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
    printf("LIBELFPERF_LOG: isFunctionRedirectorRegistered\n");
    if (*((unsigned int *)redirectorAddress) != 0) {
        printf("LIBELFPERF_LOG: %s is registered\n", name);
    } else {
        printf("LIBELFPERF_LOG: %s is not registered\n", name);
    }

    return ((*((unsigned int *)redirectorAddress)) != 0);
}


/*
 * Adds new function to the redirectors list
 */
void addNewFunction(char* name, void * functionAddr, struct RedirectorContext context){
    printf("LIBELFPERF_LOG: addNewFunction %s\n", name);
    writeRedirectionCode(getRedirectorAddressForName(name, context), functionAddr);
}


/*
 * Initializes s_redirectors and s_names
 */
void initWrapperRedirectors(struct RedirectorContext *context)
{
    initFunctionInfoStorage();

    printf("LIBELFPERF_LOG: Starting initWrapperRedirectors %p, %d \n", context, getpagesize());

    // Memory allocation
    size_t allocSize =  REDIRECTOR_SIZE * context->count + getpagesize() - 1;
    size_t i = 0;
    context->redirectors = 
	mmap(0, allocSize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	//(void *)malloc(allocSize);

    // Aligning by page border
    printf("LIBELFPERF_LOG: Before aligment %x, %x\n", context->redirectors, REDIRECTOR_SIZE*context->count + getpagesize()-1);
    context->redirectors = (void *)(((uint64_t) context->redirectors + getpagesize()-1) & ~(getpagesize()-1));
    printf("LIBELFPERF_LOG: After aligment %x\n", context->redirectors);
    int pagesNum = allocSize/getpagesize() ;
    printf("LIBELFPERF_LOG: Number of memory pages %x\n", pagesNum);

    for (i = 0; i < pagesNum; i++) {
        printf("LIBELFPERF_LOG: Going to do mprotect\n");
        if (mprotect(context->redirectors + getpagesize()*i, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC)) {
            printf("LIBELFPERF_LOG: Couldn't mprotect, errno = %d\n", errno);
            //exit(errno);
            return;
        }
    }

    // Set 0 into each redirector first byte
    // This will allow to determine wich one is inited
    printf("LIBELFPERF_LOG: Zeroing\n");
    for (i = 0 ; i < REDIRECTOR_SIZE * context->count; i += REDIRECTOR_SIZE) {
        *((unsigned int*)(context->redirectors+i)) = 0;
    }
}
