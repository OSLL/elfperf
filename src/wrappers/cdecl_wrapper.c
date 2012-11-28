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
 * \file cdecl_wrapper.c
 * \brief Implementation of wrapper for cdecl function
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */


#include <stdio.h>
#include <pthread.h>
#include "cdecl_wrapper.h"
#include "../timers/global_stats.h"


// Global function pointer - stub for us, until we will get better way to 
// understand which function should be wrapped
static void * functionPointer = NULL;
static pthread_mutex_t functionPointerLock = PTHREAD_MUTEX_INITIALIZER;

// Workaround - storing context in global variable
//static struct WrappingContext context;

// preallocated array of contexts 
static struct WrappingContext contextArray[CONTEXT_PREALLOCATED_NUMBER];
// number of first not used context
static int freeContextNumber = 0 ;
static pthread_mutex_t freeContextNumberLock = PTHREAD_MUTEX_INITIALIZER;


// This function returns address of currently free context
static struct WrappingContext * getNewContext()
{
	struct WrappingContext * context;

	pthread_mutex_lock(&freeContextNumberLock);
	// Check freeContextNumber
	if (freeContextNumber < CONTEXT_PREALLOCATED_NUMBER){
		context = &contextArray[freeContextNumber++];
	} else {
		printf("Context buffer is full!!! Exiting\n");
		exit(1);
	}
	
	pthread_mutex_unlock(&freeContextNumberLock);
	return context;
}



// This global variable is storing real return address
//static void * realReturnAddr = NULL;
//static void * eax = NULL;

static void * getFunctionPointer()
{
	return functionPointer;
}

// Return actual address for jmp
static void * getFunctionJmpAddress()
{
	/* Adding 3 to addr because each cdecl function contains such code at the start:
	55 	push   %ebp
	89 e5	mov    %esp,%ebp

	This code is changing stack (we dont want to) so we need to skip it.
	*/
	int ptr = functionPointer;
	pthread_mutex_unlock(&functionPointerLock);
	return ptr+3;
}

// Set an pointer to wrapped function
void setFunctionPointer(void * pointer)
{
	pthread_mutex_lock(&functionPointerLock);

	functionPointer = pointer;
}

// Wrapper code. 
// We doesnt touch stack and variables, just print something and jmp to wrapped function.
void wrapper()
{
	asm volatile(				
		// by the start eax contains address of the wrapped function
		"pushl %%eax\n"				//storing wrappedFunction_addr into stack
		"movl (%%ebp), %%ebx\n"			// Start of the moved area 
		"subl $0x4, %%ebx\n"			// ebx = old_ebp - 4
		"movl (%%ebx), %%edx\n"			// edx = -4(%%old_ebp) = (%%ebp)
		// Storing context pointer into freed space
		"call getNewContext\n"			// eax = getNewContext() 
		"movl %%eax, (%%ebx)\n" 		// -4(%%old_ebp) = eax
		"movl %%eax, %%ebx\n"			// %ebx = &context
		"movl %%edx, 4(%%ebx)\n"		//context->oldEbpLocVar = edx
		// Extracting wrappedFunction_addr from stack and placing it to context
		"popl 20(%%ebx)\n"			// context->functionPointer = wrappedFunction_addr 
		// Changing return address to wrapper_return_point 
		"movl 4(%%ebp), %%ecx\n"		// Storing real return_addres
		"movl %%ecx, (%%ebx) \n"
//		"movl %%ebp, %%ecx\n"                	// Storing ebp for this frame 
//                "movl %%ecx, 4(%%ebx) \n"		// needed for backshofting stackframe after $wrapper_return_point$
		"movl $wrapper_return_point, 4(%%ebp)\n" // changing return address for $wrapper_return_point
		: : :		// (%%ebx) = %%eax
		 
	);

	puts("WRAPPED!");

	// memorize old return addres and change it for returning in wrapper()
	// stack variables will be damaged, so i use global variable

	asm volatile(	// Calculate address of WrappingContext
		"movl (%%ebp), %%ecx\n"			//  old_ebp -> %%ecx
		"movl -4(%%ecx), %%ebx\n"		//  %%ebx = context address 
		// WrapperContext struct layout
		/*
			(%ebx)		realReturnAddress 
			4(%%ebx)	oldEbpLocVar // -4(%%old_ebp)
			8(%%ebx)	eax
			
		*/
		"pushl %%ebx\n"				// start recodring function time using record_start_time	
		"call record_start_time\n"
		// Going to wrapped function (context->functionPointer)
		"jmp 20(%%ebx)\n"	
		: : :
    );


	// going to wrapped function
//	asm("jmp %0" : :"r"(getFunctionJmpAddress()));

	// returning back into wrapper()
	// 	memorizing eax value
	asm volatile(
		// Calculating address of WrappingContext and memorizing return values
		"wrapper_return_point: movl -4(%%ebp), %%ebx\n"	// %%ebx = & context  
		"movl %%eax, 8(%%ebx)\n"			// context->eax = %%eax
		"fstpl 0xc(%%ebx)\n"				// context->doubleResult = ST0
		"pushl %%ebx\n"
		"call record_end_time\n"
			: : :				// push &context
	);


/*	asm(	"wrapper_return_point: movl 0(%%ebp), %0\n"
		"movl %2, 0(%%ebp)\n"
		"movl %%eax, %1 ": "=r"(context_), "=r"(context_->eax):"r"(context_->oldEbp):"%eax");*/

	// Change this call to any needed routine
	puts("back to wrapper!");	

	// restoring real return_address and eax and return
/*	asm(	"movl %1, %%eax\n"
		"jmp %0" : :"r"(context_->realReturnAddr), "r"(context_->eax) : "%eax");*/

	// Getting context address 
	// restoring value of eax and st0
	// returning to caller
	asm volatile(
		"movl -4(%%ebp), %%ebx\n"	// get context address
		"movl 4(%%ebx), %%edx\n"	// restoring -4(old_ebp)
		"movl %%edx, -4(%%ebp)\n"	// edx = context->oldEbpLocVar ; -4(%%ebp) = edx
		"movl 8(%%ebx), %%eax\n"	// restoring eax
		"fldl 0xc(%%ebx)\n"		// restoring ST0
		"pushl (%%ebx)\n"		// returning to caller - pushing return address to stack
		"ret\n" : : :
	);
}

