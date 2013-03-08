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

#ifdef ELFPERF_ARCH_64 

/*
 * Wrapper for functions which creates stack frame.
 */
void wrapper_cdecl()
{
    /* Stack structure
     * /////top/////
     *      rax
     *      rbx
     *    old_rbp
     *  return_addr
     *   arg n - 1
     *      ...
     *     arg 6
     */
    // arguments from 0 to 5 are in %rdi, %rsi, %rdx, %rcx, %r8 and %r9
    asm volatile (
        // TODO support two different redirector types - one for cdecl (do  "push   %rbp\n" inside redirector)
        // one for nocdecl

        // Create new stack frame
        "pop    %r11\n"         // r11 = rax
        "pop    %rbx\n"         // restoring rbx value
        "push   %rbp\n"
        "mov    %rsp, %rbp\n"
        "push   %rbx\n"         // saving to the stack - rbx
        "push   %r11\n"         // saving to the stack - rax value
        // Save values of needed registers
        "push   %r14\n"
        "push   %r13\n"
        "push   %r12\n"
        "push   %r10\n"
        "push   %rax\n"         // function address
        "push   %rsi\n"
        "push   %rdi\n"
        "push   %rcx\n"
        "push   %rdx\n"
        "push   %r8\n"
        "push   %r9\n"
        // push xmm0-7 on stack
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
        // Get new context for call
        "push   %r15\n"
        "call   getNewContext_\n"   // rax = getNewContext
        "pop    280(%rax)\n"        // context->r15 = %r15
        "mov    %rax, %r15\n"       // r15 = &context
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
        // Restore registers state
        "pop    %r9\n"
        "pop    %r8\n"
        "pop    %rdx\n"
        "pop    %rcx\n"
        "pop    %rdi\n"
        "pop    %rsi\n"
        "pop    %rax\n"
        "pop    288(%r15)\n"        // context->r10 = r10
        "pop    256(%r15)\n"        // context->r12 = r12 
        "pop    264(%r15)\n"        // context->r13 = r13
        "pop    272(%r15)\n"        // context->r14 = r14
        "pop    48(%r15)\n"         // context->rax = rax_old_value 
        "pop    56(%r15)\n"         // context->rbx = rbx
        // Extract context content from stack and registers
        "mov    8(%rbp), %r11\n"    // r11 = return address
        "mov    %r11, (%r15)\n"     // context->realReturnAddress = r11
        "mov    %rax, 8(%r15)\n"    // context->functionPointer = rbx
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
        "leaq   wrapper_ret_point(%rip), %rax\n"
        "mov    %rax, 0x8(%rbp)\n"       
    );

    asm volatile (
        // Save registers before record_start_time call
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
        // Restoring previous value of rax
        "mov    48(%r15), %rax\n"
        "mov    288(%r15), %r10\n"
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
//        "mov    %rbx, %r15\n"   // %r15 = context_addr
//        "mov    -8(%rbp), %r15\n"   // %r15 = 1st local var of caller (&context)
//        "mov    32(%r15), %rbx\n"   // %rbx = saved value of 1st local var of caller
//        "mov    %rbx, -8(%rbp)\n"   // 1st local var of caller = old value
//        "push   %rax\n"

        // Save return values of wrapped function
        "mov    %rax, 16(%r15)\n"   // context->integerResult = %rax
        "mov    %rdx, 72(%r15)\n"   // context->rdx = %rdx
        "movsd  %xmm0, 24(%r15)\n"  // context->floatingPointResult = %xmm0
        // Save registers before record_end_time call
        "push   %rdx\n"
        "push   %r15\n"
        // Call record_end_time
        "call   record_end_time\n"
        // Restore registers state
        "pop    %r15\n"
        "pop    %rdx\n"
        // Restoring of non-argument registers
        "mov    56(%r15), %rbx\n"
        "mov    256(%r15), %r12\n"
        "mov    264(%r15), %r13\n"
        "mov    272(%r15), %r14\n"
        // Preparing to exit from wrapper
        "mov    16(%r15), %rax\n"   // %rax = context->integerResult
        "mov    72(%r15), %rdx\n"   // %rdx = context->rdx
        "push   (%r15)\n"           // restore real return address, i.e. push context->realReturnAddr
        "movsd  24(%r15), %xmm0\n"  // %xmm0 = context->floatingPointResult
        "push   280(%r15)\n"        // push old r15 value to stack
        "movq   $0, (%r15)\n"       // context->realReturnAddr = 0
        "pop    %r15\n"             // restore old r15 value
        "ret\n"
    );
}


/*
 * Code of wrapper for optimized function which doesn't create stack frame
 */
void wrapper_no_cdecl()
{
    /* Stack structure
     * /////top/////
     *     %rbx
     *     %rax
     *  return_addr
     *   arg n - 1
     *      ...
     *     arg 6
     */
    // arguments from 0 to 5 are in %rdi, %rsi, %rdx, %rcx, %r8 and %r9
    
    // In this wrapper we don't need to create new stack fram
    asm volatile (
        // Subtract 4 from %rax (functionPointer+4) because 
        // we don't need to skip stack frame creation
        //"sub $4, %rax\n"
        // Save values of needed registers
        "push   %r12\n"
        "push   %r13\n"
        "push   %r14\n"
        "push   %r10\n"
        "push   %rbp\n"
        "push   %rax\n"
        //"push   %rbx\n" //<- was saved at redirector
        "push   %rsi\n"
        "push   %rdi\n"
        "push   %rcx\n"
        "push   %rdx\n"
        "push   %r8\n"
        "push   %r9\n"
        // Need to save r15 also
        "push   %r15\n"
        // push xmm0-7 on stack
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
        // Get new context for call
        "call   getNewContext_\n"    // rax = getNewContext
        "mov    %rax, %r15\n"        // r15 = &context
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
        // Restore registers state
        "pop    280(%rax)\n"        // context->r15 = r15
        "pop    %r9\n"
        "pop    %r8\n"
        "pop    %rdx\n"
        "pop    %rcx\n"
        "pop    %rdi\n"
        "pop    %rsi\n"
        "pop    8(%r15)\n"          // context->functionPointer = function address
        "pop    %rbp\n"
        "pop    %r10\n"
        "pop    %r14\n"
        "pop    %r13\n"
        "pop    %r12\n"
        "pop    %rax\n"
        "pop    %rbx\n"
        // Extract context content from stack and registers
        "mov    (%rsp), %r11\n"     // r11 = return address
        "mov    %r11, (%r15)\n"     // context->realReturnAddress = r11
        //"mov    %rax, 8(%r15)\n"    // context->functionPointer = rax
        // Saving registers to context
        "mov    %rbx, 56(%r15)\n"   // context->rbx = rbx
        "mov    %rbp, 40(%r15)\n"   // context->rbp = rbp
        "mov    %r12, 256(%r15)\n"  // context->r12 = r12
        "mov    %r13, 264(%r15)\n"  // context->r13 = r13
        "mov    %r14, 272(%r15)\n"  // context->r14 = r14
        "mov    %r10, 288(%r15)\n"  // context->r10 = r10
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
        "leaq   wrapper_no_frame_ret_point(%rip), %r11\n"
        "mov   %r11, (%rsp)\n"
    );

    asm volatile (
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
        // Restore XMM registers
        "movdqu 112(%r15), %xmm0\n"
        "movdqu 128(%r15), %xmm1\n"
        "movdqu 144(%r15), %xmm2\n"
        "movdqu 160(%r15), %xmm3\n"
        "movdqu 176(%r15), %xmm4\n"
        "movdqu 192(%r15), %xmm5\n"
        "movdqu 208(%r15), %xmm6\n"
        "movdqu 224(%r15), %xmm7\n"
        // Save context address in %rbp and %rbp value inside of context
        //"mov    %rbp, 40(%r15)\n"   // context->rbp = %rbp
        "mov    %r15, %rbp\n"       // %rbp = %r15
        "mov    288(%r15), %r10\n"  // %r10 = context->r10
        // Jump to wrapped function
        "mov    8(%r15), %r15\n"
        "sub    $4, %r15\n"
        "jmp    %r15\n"          // jmp context->functionPointer
    );

    asm volatile (
        "wrapper_no_frame_ret_point:\n"
        // Here we return after execution of wrapping function.
        // Restore context address from %rbp value
        "mov    %rbp, %r15\n"       // %r15 = %rbp (&context)
        // Save return values of wrapped function
        "mov    %rax, 16(%r15)\n"   // context->integerResult = %rax
        "mov    %rdx, 72(%r15)\n"   // context->rdx = %rdx
        "movsd  %xmm0, 24(%r15)\n"  // context->floatingPointResult = %xmm0
        // Save registers before record_end_time call
        "push   %r15\n"
        // Call record_end_time
        "mov    %r15, %rdi\n"       // %rdi = &context (arg0 for record_end_time)
        "call   record_end_time\n"
        // Restore registers state
        "pop    %r15\n"
        // Restoring registers which are not used for arguments passing
        "mov    56(%r15), %rbx\n"
        "mov    40(%r15), %rbp\n"
        "mov    256(%r15), %r12\n"
        "mov    264(%r15), %r13\n"
        "mov    272(%r15), %r14\n"
        //"mov    280(%r15), %r15\n"
        // Preparing to exit from wrapper
        "mov    16(%r15), %rax\n"   // %rax = context->integerResult
        "mov    72(%r15), %rdx\n"
        "push   (%r15)\n"           // restore real return address, i.e. push context->realReturnAddr
        "movsd  24(%r15), %xmm0\n"  // %xmm0 = context->floatingPointResult
        // Restoring r15
        "push   280(%r15)\n"
        // After THIS instruction should not be any other context usages
        "movq   $0, (%r15)\n"       // context->realReturnAddr = 0
        "pop    %r15\n"
        "ret\n"
    );
}


#elif defined ELFPERF_ARCH_32

// Thread independent context storage
static __thread struct WrappingContext* s_wrappingContext = NULL;


// Contains steps of preprofiling
static void preProfile(void* returnAddr, void* functionPtr)
{
    struct WrappingContext* context = getNewContext();
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
    return context->realReturnAddr;
}


/*
 * Code of wrapper-function
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
     *    args[0]
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

#endif


#ifdef ELFPERF_ARCH_32

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

/*
 * Creates set of machine instructions
 * ///////////////////////////////////
 *  push rbx                    ; If not memorize it register content will be lost
 *  push rax
 *  mov fcnPtr+4, %rax
 *  mov wrapper_addr+4, %rbx
 *  jmp %rbx
 * ///////////////////////////////////
 * and writes them to @destination@
 */
void writeRedirectionCode(unsigned char * redirector, void * fcnPtr)
{
    printf("LIBELFPERF_LOG: Writing redirection code for function %x: \n", fcnPtr);
  
    uint64_t functionPointer = (uint64_t)(fcnPtr)+4;

    // push %rbx
    redirector[0] = 0x53;
    // push %rax
    redirector[1] = 0x50;

    // mov fcnPtr+4, %rax     48 b8 ...
    redirector[2] = 0x48;
    redirector[3] = 0xb8;
    
    // writing function pointer bytes in reversed order
    redirector[11] = (functionPointer >> 56) & 0xFF;
    redirector[10] = (functionPointer >> 48) & 0xFF;
    redirector[9] = (functionPointer >> 40) & 0xFF;
    redirector[8] = (functionPointer >> 32) & 0xFF;
    redirector[7] = (functionPointer >> 24) & 0xFF;
    redirector[6] = (functionPointer >> 16) & 0xFF;
    redirector[5] = (functionPointer >>  8) & 0xFF;
    redirector[4] =  functionPointer        & 0xFF;

    // mov $wrapper+4, %rbx
    // Skip stack frame construction - because we pass some extra params throw stack
    // and esp will not be good addr for new stack frame
    uint64_t wrapper_;

    // Check first 3 bytes of function and choose appropriate wrapper.
    printf("LIBELFPERF_LOG: First 3 bytes of function: \n");
    printf("\t%x %x %x %x\n",
        ((uint64_t)((void**)fcnPtr)[0]) & 0xFF,
        (((uint64_t)((void**)fcnPtr)[0]) >> 8) & 0xFF,
        (((uint64_t)((void**)fcnPtr)[0]) >> 16) & 0xFF,
        (((uint64_t)((void**)fcnPtr)[0]) >> 24) & 0xFF       
    );

    if ( (((uint64_t)((void**)fcnPtr)[0]) & 0xFF) == 0x55           // 1st byte
         &&
         ((((uint64_t)((void**)fcnPtr)[0]) >> 8) & 0xFF) == 0x48    // 2nd byte
         &&
         ((((uint64_t)((void**)fcnPtr)[0]) >> 16) & 0xFF) == 0x89   // 3rd byte
          &&
         ((((uint64_t)((void**)fcnPtr)[0]) >> 24) & 0xFF) == 0xe5 ) // 4th byte
    {
        printf("LIBELFPERF_LOG: Chosen normal wrapper for redirector\n");
        wrapper_ = (uint64_t)&wrapper_cdecl + 4;
    } else {
        wrapper_ = (uint64_t)&wrapper_no_cdecl + 4;
        printf("LIBELFPERF_LOG: Chosen wrapper_no_stack_frame for redirector\n");
    }

    // mov wrapper, %edx     48 bb ...
    redirector[12] = 0x48;
    redirector[13] = 0xbb;

    redirector[21] = (wrapper_ >> 56) & 0xFF;
    redirector[20] = (wrapper_ >> 48) & 0xFF;
    redirector[19] = (wrapper_ >> 40) & 0xFF;
    redirector[18] = (wrapper_ >> 32) & 0xFF;
    redirector[17] = (wrapper_ >> 24) & 0xFF;
    redirector[16] = (wrapper_ >> 16) & 0xFF;
    redirector[15] = (wrapper_ >>  8) & 0xFF;
    redirector[14] =  wrapper_        & 0xFF;

    // jmp %rbx     ff e3
    redirector[22] = 0xFF;
    redirector[23] = 0xe3;
    
    printf("LIBELFPERF_LOG: Created redirector for %p at %p, wrapper = %p,\n", fcnPtr, redirector, wrapper_);
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
