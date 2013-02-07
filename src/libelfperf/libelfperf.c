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

// This function returns address of currently free context
static struct WrappingContext * getNewContext_()
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
     *  return_addr
     *   arg n - 1
     *      ...
     *     arg 6
     */
    // arguments from 0 to 5 are in %rdi, %rsi, %rdx, %rcx, %r8 and %r9
    asm volatile (
        // Create new stack frame
        "pop    %rbx\n"
        "push   %rbp\n"
        "mov    %rsp, %rbp\n"
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
        "call   getNewContext_\n"   // rax = getNewContext
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
        "leaq   wrapper_ret_point(%rip), %rax\n"
        "mov   %rax, 0x8(%rbp)\n"       
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
        "movsd  %xmm0, 24(%r15)\n"  // context->floatingPointResult = %xmm0
        // Save registers before record_end_time call
        "push   %rdx\n"
        "push   %r15\n"
        // Call record_end_time
        "mov    %r15, %rdi\n"       // %rdi = &context (arg0 for record_end_time)
        "call   record_end_time\n"
        // Restore registers state
        "pop    %r15\n"
        "pop    %rdx\n"
        // Preparing to exit from wrapper
        "mov    16(%r15), %rax\n"   // %rax = context->integerResult
        "push   (%r15)\n"           // restore real return address, i.e. push context->realReturnAddr
        "movq   $0, (%r15)\n"       // context->realReturnAddr = 0
        "movsd  24(%r15), %xmm0\n"  // %xmm0 = context->floatingPointResult
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
     *    old_rbp
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
        "sub $4, %rax\n"
        // Save values of needed registers
        "push   %r12\n"
        "push   %r13\n"
        "push   %r14\n"
        "push   %rbp\n"
        "push   %rax\n"
        //"push   %rbx\n" //<- was saved at redirector
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
        // Need to save r15 also
        "push   %r15\n"
        // Get new context for call
        "call   getNewContext_\n"    // rax = getNewContext
        "pop    280(%rax)\n"         // context->r15 = r15
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
        "pop    %r9\n"
        "pop    %r8\n"
        "pop    %rdx\n"
        "pop    %rcx\n"
        "pop    %rdi\n"
        "pop    %rsi\n"
        "pop    %rax\n"
        "pop    %rbp\n"
        "pop    %r14\n"
        "pop    %r13\n"
        "pop    %r12\n"
        "pop    %rbx\n"
        // Extract context content from stack and registers
        "mov    (%rsp), %r11\n"     // r11 = return address
        "mov    %r11, (%r15)\n"     // context->realReturnAddress = r11
        "mov    %rax, 8(%r15)\n"    // context->functionPointer = rax
        // Saving registers to context
        "mov    %rbx, 56(%r15)\n"   // context->rbx = rbx
        "mov    %rbp, 40(%r15)\n"   // context->rbp = rbp
        "mov    %r12, 256(%r15)\n"   // context->r12 = r12
        "mov    %r13, 264(%r15)\n"   // context->r13 = r13
        "mov    %r14, 272(%r15)\n"   // context->r14 = r14
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
        "leaq   wrapper_no_frame_ret_point(%rip), %rax\n"
        "mov   %rax, (%rsp)\n"
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
        // Jump to wrapped function
        "jmp    8(%r15)\n"          // jmp context->functionPointer
    );

    asm volatile (
        "wrapper_no_frame_ret_point:\n"
        // Here we return after execution of wrapping function.
        // Restore context address from %rbp value
        "mov    %rbp, %r15\n"       // %r15 = %rbp (&context)
        // Save return values of wrapped function
        "mov    %rax, 16(%r15)\n"   // context->integerResult = %rax
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

/* 
 * Code of wrapper for functions.
 * Wrapper calls selected function and measures its time.
 */
void wrapper_cdecl()
{
    /* Stack structure
     * /////top/////
     *    old_ebx
     *    old_edx
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
        "movl -4(%ebp), %ebx\n"	// get context address
        "movl 4(%ebx), %edx\n"	// restoring -4(old_ebp)
        "movl %edx, -4(%ebp)\n"	// edx = context->oldEbpLocVar ; -4(%ebp) = edx
        "movl 8(%ebx), %eax\n"	// restoring eax
        "fldl 0xc(%ebx)\n"	// restoring ST0
        // Restoring registers
        "movl 28(%ebx), %edx\n" // restoring edx
        "movl 32(%ebx), %ecx\n" // restoring ecx
        //
        "pushl (%ebx)\n"        // returning to caller - pushing return address to stack
        // Marking context as free and Restoring %ebx
        "pushl 24(%ebx)\n" 	    // old_ebx to stack
        "movl $0, (%ebx)\n"     // context.realReturnAddr = 0
    	"popl %ebx\n"           // %ebx = old_ebx
        "ret\n"
    );
}

/* 
 * Code of wrapper for optimized functions which doesn't create new stack frame.
 */
void wrapper_no_cdecl()
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
    asm volatile (
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
#endif


#ifdef ELFPERF_ARCH_32

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
    printf("LIBELFPERF_LOG: Writing redirection code for function %x: \n", fcnPtr);

    unsigned int functionPointer = (unsigned int )(fcnPtr)+FCN_PTR_OFFSET;
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
    unsigned int wrapper_;

    // Check first 3 bytes of function and choose appropriate wrapper.
    printf("LIBELFPERF_LOG: First 3 bytes of function: \n");
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
        printf("LIBELFPERF_LOG: Chosen 1st wrapper for redirector\n");
        wrapper_ = (unsigned int)&wrapper_cdecl + FCN_PTR_OFFSET;
    } else {
        wrapper_ = (unsigned int)&wrapper_no_cdecl + FCN_PTR_OFFSET;
        printf("LIBELFPERF_LOG: Chosen 2nd wrapper for redirector\n");
    }

    redirector[9] = 0xbb;
    redirector[13] = (wrapper_ >> 24) & 0xFF;
    redirector[12] = (wrapper_ >> 16) & 0xFF;
    redirector[11] = (wrapper_ >>  8) & 0xFF;
    redirector[10] =  wrapper_        & 0xFF;

    // jmp *(%ebx)
    redirector[14] = 0xFF;
    redirector[15] = 0xE3; //0x23;
    
    printf("LIBELFPERF_LOG: Created redirector for %p at %p, wrapper =  %x\n", fcnPtr, redirector, wrapper_);
}

#elif defined ELFPERF_ARCH_64

/*
 * Creates set of machine instructions
 * ///////////////////////////////////
 *  push rbx                    ; If not memorize it register content will be lost
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

    // mov fcnPtr+4, %rax     48 b8 ...
    redirector[1] = 0x48;
    redirector[2] = 0xb8;
    
    // writing function pointer bytes in reversed order
    redirector[10] = (functionPointer >> 56) & 0xFF;
    redirector[9] = (functionPointer >> 48) & 0xFF;
    redirector[8] = (functionPointer >> 40) & 0xFF;
    redirector[7] = (functionPointer >> 32) & 0xFF;
    redirector[6] = (functionPointer >> 24) & 0xFF;
    redirector[5] = (functionPointer >> 16) & 0xFF;
    redirector[4] = (functionPointer >>  8) & 0xFF;
    redirector[3] =  functionPointer        & 0xFF;

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
    redirector[11] = 0x48;
    redirector[12] = 0xbb;

    redirector[20] = (wrapper_ >> 56) & 0xFF;
    redirector[19] = (wrapper_ >> 48) & 0xFF;
    redirector[18] = (wrapper_ >> 40) & 0xFF;
    redirector[17] = (wrapper_ >> 32) & 0xFF;
    redirector[16] = (wrapper_ >> 24) & 0xFF;
    redirector[15] = (wrapper_ >> 16) & 0xFF;
    redirector[14] = (wrapper_ >>  8) & 0xFF;
    redirector[13] =  wrapper_        & 0xFF;

    // jmp %rbx     ff e3
    redirector[21] = 0xFF;
    redirector[22] = 0xe3;
    
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
