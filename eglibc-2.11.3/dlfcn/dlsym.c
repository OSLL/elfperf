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
#include <time.h>

#include <ldsodefs.h>

#include "prof/cdecl_wrapper.h"
#include "prof/time.h" 
//#include "prof/wrappers.h"

//#############################################
//### Begin ELFPERF
//#############################################


const int MAX_SLOTS = 3;


#include <limits.h>    /* for PAGESIZE */
#ifndef PAGESIZE
#define PAGESIZE 4096
#endif
#define REDIRECTOR_WORDS_SIZE 4

static void * functionPointer = NULL;
//static pthread_mutex_t functionPointerLock = PTHREAD_MUTEX_INITIALIZER;

// Workaround - storing context in global variable
//static struct WrappingContext context;

// preallocated array of contexts 
static struct WrappingContext contextArray[CONTEXT_PREALLOCATED_NUMBER];
// number of first not used context
static int freeContextNumber = 0 ;
//static pthread_mutex_t freeContextNumberLock = PTHREAD_MUTEX_INITIALIZER;

struct timespec get_accurate_time()
{
    struct timespec time;
    clockid_t clockType = CLOCK_MONOTONIC;
//    clock_gettime(clockType, &time);
    return time;
}

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec res;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        res.tv_sec = end.tv_sec - start.tv_sec - 1;
        res.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        res.tv_sec = end.tv_sec - start.tv_sec;
        res.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return res;
}

void record_start_time(void * context){
	struct WrappingContext * cont = (struct WrappingContext *)context;
	cont->startTime = get_accurate_time();
}



void record_end_time(void * context){
	struct WrappingContext * cont = (struct WrappingContext *)context;
	cont->endTime = get_accurate_time();
	struct timespec duration = diff(cont->startTime, cont->endTime);
	printf("Function duration = %ds %dns\n", duration.tv_sec, duration.tv_nsec);	
}

// This function returns address of currently free context
struct WrappingContext * getNewContext(){
	
	struct WrappingContext * context;
	

	//pthread_mutex_lock(&freeContextNumberLock);
	// Check freeContextNumber
	if (freeContextNumber < CONTEXT_PREALLOCATED_NUMBER){
		context = &contextArray[freeContextNumber++];
	} else {
		printf("Context buffer is full!!! Exiting\n");
		exit(1);
	}
	
	//pthread_mutex_unlock(&freeContextNumberLock);
	return context;
}




static void elfperf_log(const char *msg) 
{
	static int c=0;
	printf("[ELFPERF:%d] %s\n",c++,msg);
}


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
	unsigned int wrapper = &s_wrapperAddress;
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
	unsigned int i = 0;
	for (i = 0; i<REDIRECTOR_WORDS_SIZE*sizeof(void*) ; i++){
		printf("%02hhx ", redirector[i]);
	}
	printf("\nfcn_ptr = %x, destination = %x\n", fcnPtr, redirector);
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
	printf("getRedirectorAddressForName = %x\n", s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*));
	return s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
}


// Add new function to the list of redirectors
void addNewFunction(char* name, void * functionAddr){
	writeRedirectionCode( getRedirectorAddressForName(name), functionAddr);
}

// Initialize s_redirectors and s_names
void initWrapperRedirectors(char** names,unsigned int count, void * wrapperAddr){
	s_wrapperAddress = wrapperAddr;

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

// Wrapper code. 
// We doesnt touch stack and variables, just print something and jmp to wrapped function.
void wrapper(){
	/*
	*/
	asm volatile(				
		// by the start eax contains address of the wrapped function
		"pushl %eax\n"				//storing wrappedFunction_addr into stack
		"movl (%ebp), %ebx\n"			// Start of the moved area 
		"subl $0x4, %ebx\n"			// ebx = old_ebp - 4
		"movl (%ebx), %edx\n"			// edx = -4(%old_ebp) = (%ebp)
		// Storing context pointer into freed space
		"call getNewContext\n"			// eax = getNewContext() 
		"movl %eax, (%ebx)\n" 		// -4(%old_ebp) = eax
		"movl %eax, %ebx\n"			// %ebx = &context
		"movl %edx, 4(%ebx)\n"		//context->oldEbpLocVar = edx
		// Extracting wrappedFunction_addr from stack and placing it to context
		"popl 20(%ebx)\n"			// context->functionPointer = wrappedFunction_addr 
		// Changing return address to wrapper_return_point 
		"movl 4(%ebp), %ecx\n"		// Storing real return_addres
		"movl %ecx, (%ebx) \n"
//		"movl %ebp, %ecx\n"                	// Storing ebp for this frame 
//                "movl %ecx, 4(%ebx) \n"		// needed for backshofting stackframe after $wrapper_return_point$
		"movl $wrapper_return_point, 4(%ebp)\n" // changing return address for $wrapper_return_point
		//: : :		// (%ebx) = %eax
		 
	);

	puts("WRAPPED!");

	// memorize old return addres and change it for returning in wrapper()
	// stack variables will be damaged, so i use global variable

	asm volatile(	// Calculate address of WrappingContext
		"movl (%ebp), %ecx\n"			//  old_ebp -> %ecx
		"movl -4(%ecx), %ebx\n"		//  %ebx = context address 
		// WrapperContext struct layout
		/*
			(%ebx)		realReturnAddress 
			4(%ebx)	oldEbpLocVar // -4(%old_ebp)
			8(%ebx)	eax
			
		*/
		"pushl %ebx\n"				// start recodring function time using record_start_time	
		"call record_start_time\n"
		// Going to wrapped function (context->functionPointer)
		"jmp 20(%ebx)\n"	
		//: : :
	);	


	// going to wrapped function
//	asm("jmp %0" : :"r"(getFunctionJmpAddress()));

	// returning back into wrapper()
	// 	memorizing eax value
	asm volatile(
		// Calculating address of WrappingContext and memorizing return values
		"wrapper_return_point: movl -4(%ebp), %ebx\n"	// %ebx = & context  
		"movl %eax, 8(%ebx)\n"			// context->eax = %eax
		"fstpl 0xc(%ebx)\n"				// context->doubleResult = ST0
		"pushl %ebx\n"
		"call record_end_time\n"
		//	: : :				// push &context
	);


/*	asm(	"wrapper_return_point: movl 0(%ebp), %0\n"
		"movl %2, 0(%ebp)\n"
		"movl %eax, %1 ": "=r"(context_), "=r"(context_->eax):"r"(context_->oldEbp):"%eax");*/

	// Change this call to any needed routine
	puts("back to wrapper!");	

	// restoring real return_address and eax and return
/*	asm(	"movl %1, %eax\n"
		"jmp %0" : :"r"(context_->realReturnAddr), "r"(context_->eax) : "%eax");*/

	// Getting context address 
	// restoring value of eax and st0
	// returning to caller
	asm volatile(
		"movl -4(%ebp), %ebx\n"	// get context address
		"movl 4(%ebx), %edx\n"	// restoring -4(old_ebp)
		"movl %edx, -4(%ebp)\n"	// edx = context->oldEbpLocVar ; -4(%ebp) = edx
		"movl 8(%ebx), %eax\n"	// restoring eax
		"fldl 0xc(%ebx)\n"		// restoring ST0
		"pushl (%ebx)\n"		// returning to caller - pushing return address to stack
		"ret\n"// : : :
	);

}

void some_func()
{
	elfperf_log("some function");
}

void *
__dlsym (void *handle, const char *name DL_CALLER_DECL)
{
  static int initialized = 0;
  char *names[]={"hello","hello","hello"};

  if(0==initialized)
  {
	initWrapperRedirectors(names,3,wrapper);
	initialized = 1;
  }



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

  elfperf_log("returning result");



  elfperf_log(name);

  addNewFunction(name,result);

  return getRedirectorAddressForName(name);
}
# ifdef SHARED
strong_alias (__dlsym, dlsym)
# endif
#endif
