/* Look up a symbol in a shared object loaded by `dlopen'.
   Copyright (C) 1995-2000, 2004 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>

#include <ldsodefs.h>

#include "prof/cdecl_wrapper.h"
#include "prof/time.h" 

//#############################################
//### Begin ELFPERF
//#############################################

static void elfperf_log(const char *msg) 
{
	static int c=0;
	printf("[ELFPERF:%d]%s\n",c++,msg);
}



/**
 * This file contains of wrapper for cdecl functions
 * it use getFunctionPointer() function for obtainting address of a wrapped function
 *  
 */

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
static struct WrappingContext * getNewContext(){
	
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

static void * getFunctionPointer(){
	return functionPointer;
}

// Return actual address for jmp
static void * getFunctionJmpAddress(){
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
void setFunctionPointer(void * pointer){

	//kkv:pthread_mutex_lock(&functionPointerLock);

	functionPointer = pointer;

}



// Wrapper code. 
// We doesnt touch stack and variables, just print something and jmp to wrapped function.
void wrapper(){
#if 0
	/*
	*/
	asm volatile(				
		"movl (%%ebp), %%ebx\n"			// Start of the moved area 
		"subl $0x4, %%ebx\n"			// ebx = old_ebp - 4
		"movl (%%ebx), %%edx\n"			// edx = -4(%%old_ebp) = (%%ebp)
		// Storing context pointer into freed space
		"call getNewContext\n"			// eax = getNewContext() 
		"movl %%eax, (%%ebx)\n" 		// -4(%%old_ebp) = eax
		"movl %%eax, %%ebx\n"			// %ebx = &context
		"movl %%edx, 4(%%ebx)\n"		//context->oldEbpLocVar = edx
		// Storing FunctionJmpAddress at context->functionPointer
		"call getFunctionJmpAddress\n"
		"movl %%eax, 20(%%ebx)\n"		// context->functionPointer = getFunctionJmpAddress() 
		// Changing return address to wrapper_return_point 
		"movl 4(%%ebp), %%ecx\n"		// Storing real return_addres
		"movl %%ecx, (%%ebx) \n"
//		"movl %%ebp, %%ecx\n"                	// Storing ebp for this frame 
//              "movl %%ecx, 4(%%ebx) \n"		// needed for backshofting stackframe after $wrapper_return_point$
		"movl $wrapper_return_point, 4(%%ebp)\n" // changing return address for $wrapper_return_point
	//kkv:&&&	: : :		// (%%ebx) = %%eax
		 
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
		//kkv: : :
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
	//kkv		: : :				// push &context
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
		"ret\n"// : : :
	);
#endif //0
}

//#################################################

#if !defined SHARED && defined IS_IN_libdl


void *
dlsym (void *handle, const char *name)
{
  elfperf_log("dlsym");
  return __dlsym (handle, name, RETURN_ADDRESS (0));
}

#else

struct dlsym_args
{
  /* The arguments to dlsym_doit.  */
  void *handle;
  const char *name;
  void *who;

  /* The return value of dlsym_doit.  */
  void *sym;
};

static void
dlsym_doit (void *a)
{
  struct dlsym_args *args = (struct dlsym_args *) a;

  args->sym = _dl_sym (args->handle, args->name, args->who);
}


void *
__dlsym (void *handle, const char *name DL_CALLER_DECL)
{
# ifdef SHARED
	elfperf_log("__dlsym ehter");
  if (__builtin_expect (_dlfcn_hook != NULL, 0))
    return _dlfcn_hook->dlsym (handle, name, DL_CALLER);
# endif

  struct dlsym_args args;
  args.who = DL_CALLER;
  args.handle = handle;
  args.name = name;

  /* Protect against concurrent loads and unloads.  */
  __rtld_lock_lock_recursive (GL(dl_load_lock));

  void *result = (_dlerror_run (dlsym_doit, &args) ? NULL : args.sym);

  __rtld_lock_unlock_recursive (GL(dl_load_lock));

  return result;
}
# ifdef SHARED
strong_alias (__dlsym, dlsym)
# endif
#endif
