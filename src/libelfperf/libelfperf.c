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



// preallocated array of contexts
static struct WrappingContext contextArray[CONTEXT_PREALLOCATED_NUMBER];
// number of first not used context
static int freeContextNumber = 0 ;

// This function returns address of currently free context
struct WrappingContext * getNewContext_(){

    struct WrappingContext * context;


    //pthread_mutex_lock(&freeContextNumberLock);
    // Check freeContextNumber
    // atomicly increment the freeContextNumber
    // check if it is greater than CONTEXT_PREALLOCATED_NUMBER

    unsigned int number = __sync_fetch_and_add(&freeContextNumber, 1);
    if (number < CONTEXT_PREALLOCATED_NUMBER){
        context = &contextArray[number];
    } else {
        printf("Context buffer is full!!! Exiting\n");
        return 0;
        // exit(1);
    }

    //pthread_mutex_unlock(&freeContextNumberLock);
    return context;
}


// Stack structure

/*
///////top///////
	old_edx
	old_ebx
	old_ecx
	old_ebp

*/
void  wrapper()
{
    asm volatile(
		//// Special creation of a stack frame ebp 
		// Because we placed old registers on the ebp we need to start stack frame from this place
		// not from current esp
		"movl %esp, %eax\n"
		"addl $12, %eax\n"			// %ebp = %esp + 12
		"movl %eax, %ebp\n"			// because 12 is a size of saved old_{ecx,ebx,edx}
		///////////////////
		// By the start of wrapper edx contains jump addres of function, which is wrapped
		"pushl %edx\n"				// Storing wrappedFunction_addr into stack
		"movl (%ebp), %ebx\n"			// ebx = old_ebp
		"subl $0x4, %ebx\n"			// ebx = old_ebp - 4
		"movl (%ebx), %edx\n"			// edx = -4(%old_ebp) = (%ebp)
		// Storing context pointer into freed space (-4(%old_ebp))
		"call getNewContext_\n"			// eax = getNewContext()
		"movl %eax, (%ebx)\n" 			// -4(%old_ebp) = eax
		"movl %eax, %ebx\n"			// %ebx = &context
		"movl %edx, 4(%ebx)\n"			//context->oldEbpLocVar = edx
		// Extracting wrappedFunction_addr from stack and placing it to context
		"popl 20(%ebx)\n"			// context->functionPointer = wrappedFunction_addr
		// Memorization of registers states
		"pop 24(%ebx)\n"	// context.old_ebx = old_ebx
		"pop 28(%ebx)\n"	// context.old_edx = old_edx
		"pop 32(%ebx)\n"	// context.old_ecx = old_ecx

		// Changing return address to wrapper_return_point
		"movl 4(%ebp), %ecx\n"			// Storing real return_addres
		"movl %ecx, (%ebx) \n"
		"movl $wrapper_return_point, 4(%ebp)\n" // changing return address for $wrapper_return_point
		//: : :		// (%ebx) = %eax

		);

    //elfperf_log("WRAPPED!");

    // memorize old return addres and change it for returning in wrapper()
    // stack variables will be damaged, so i use global variable

    asm volatile(	// Calculate address of WrappingContext
		    "movl (%ebp), %ecx\n"			//  %ecx = old_ebp
		    "movl -4(%ecx), %ebx\n"			//  %ebx = context address
		    // WrapperContext struct layout
		    /*
			    (%ebx)		realReturnAddress
			    4(%ebx)	oldEbpLocVar // -4(%old_ebp)
			    8(%ebx)	eax
			    12(%ebx) floatFunctionReturnValue

			*/
		    // Start time recording
		    // record_start_time(%ebx)

/* Commented call of record_start_time_*/

		    "pushl %ebx\n"				// pushing parameter(context address into stack)
		    "call record_start_time\n"		//
		    "add $4, %esp\n"			// cleaning stack
		    // Going to wrapped function (context->functionPointer)
		    "jmp 20(%ebx)\n"
		    //: : :
		    );


	asm volatile(
		// Calculating address of WrappingContext and memorizing return values
		"wrapper_return_point: movl -4(%ebp), %ebx\n"	// %ebx = & context
		"movl %eax, 8(%ebx)\n"			// context->eax = %eax
		"fstpl 0xc(%ebx)\n"			// context->doubleResult = ST0
		// Saving errno
//		"movl errno, %edx\n"			// %edx = errno
//		"movl %edx, 24(%ebx)\n"			// context.errno = %edx
		// Measuring time of function execution

/* Commented call of record_end_time_ */

		"pushl %ebx\n"				// pushing context address to stack
		"call record_end_time\n"		// calling record_end_time
		"add $4, %esp\n"			// cleaning allocated memory

		//	: : :
		);


    // Getting context address
    // restoring value of eax and st0
    // returning to caller
    asm volatile(
		"movl -4(%ebp), %ebx\n"	// get context address
		"movl 4(%ebx), %edx\n"	// restoring -4(old_ebp)
		"movl %edx, -4(%ebp)\n"	// edx = context->oldEbpLocVar ; -4(%ebp) = edx
		"movl 8(%ebx), %eax\n"	// restoring eax
		"fldl 0xc(%ebx)\n"	// restoring ST0
		// Restoring registers
		"movl 28(%ebx), %edx\n" // restoring edx
		"movl 32(%ebx), %ecx\n" // restoring ecx
		//
		"pushl (%ebx)\n"	// returning to caller - pushing return address to stack
		// Restoring %ebx
		"movl 24(%ebx), %ebx\n" // %ebx = old_ebx
		"ret\n"// : : :
		);
}

// Create set of machine instructions
/*
	push %ebp
	push %ecx
	push %edx
	push %ebx
        mov fcnPtr+3, %edx
	mov wrapper_addr+3, %ebx
        jmp *(%ebx)
*/
// and write them to @destination@
void writeRedirectionCode(unsigned char * redirector, void * fcnPtr){
    //	 unsigned char* redirector[REDIRECTOR_WORDS_SIZE*sizeof(void*)];
    // mov $number, %%edx
    // ret = 0xc3
    unsigned int functionPointer = (unsigned int )(fcnPtr)+3;


/*
	push %ebp
	push %ecx
	push %edx
	push %ebx
*/

    redirector[0] = 0x55;
    redirector[1] = 0x51; 
    redirector[2] = 0x52; 
    redirector[3] = 0x53; 

/*
        mov fcnPtr+3, %edx
*/

    redirector[4]=0xba;//0xb8;
    // reversing byte order
    redirector[8] = (functionPointer >> 24) & 0xFF;
    redirector[7] = (functionPointer >> 16) & 0xFF;
    redirector[6] = (functionPointer >>  8) & 0xFF;
    redirector[5] =  functionPointer        & 0xFF;
    //memcpy(redirector+1, &number, 4);

/*
	mov $wrapper+3, %ebx
*/

    // Skip stack frame construction - because we pass some extra params throw stack
    // and esp will not be good addr for new stack frame
    unsigned int wrapper_ = (unsigned int)&wrapper + 3;
    redirector[9] = 0xbb;
    redirector[13] = (wrapper_ >> 24) & 0xFF;
    redirector[12] = (wrapper_ >> 16) & 0xFF;
    redirector[11] = (wrapper_ >>  8) & 0xFF;
    redirector[10] =  wrapper_        & 0xFF;

/*
	jmp *(%ebx)
*/
    redirector[14] = 0xFF;
    redirector[15] = 0xE3;//0x23;

    
    printf("Created redirector for %p at %p, wrapper = %p, %x\n", fcnPtr, redirector, wrapper, wrapper_);
}


// Return index of function with name @name@
unsigned int getFunctionIndex(char* name, struct RedirectorContext context){
    unsigned int i;
    for (i=0; i<context.count; i++){
	printf("Testing %s for %s\n", context.names[i], name);

        if ( !strcmp(name, context.names[i]) ){
    		printf("Index of %s is %x\n", name, i);
        	return i;
	}
    }
    printf("Returning invalid index in getFunctionIndex for %s\n", name);
    return context.count+1;
}

// Return true, if function name exists into s_names array
bool isFunctionInFunctionList(char* name, struct RedirectorContext context){
    printf("Doing isFunctionInFunctionList\n");
    unsigned int index = getFunctionIndex(name, context);
    return (index != (context.count+1) );
}



// Return redirector address for function with name @name@
void* getRedirectorAddressForName(char* name, struct RedirectorContext context){
    int functionIndex = getFunctionIndex(name, context);
    //void * result = context.redirectors +  functionIndex;
    printf("getRedirectorAddressForName\n");
    //_dl_printf("getRedirectorAddressForName = %x\n", s_redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*));
    return context.redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
}

// Return true, if redirector for function with name = @name@ is already registered
bool isFunctionRedirectorRegistered(char* name, struct RedirectorContext context){
    // Gets first byte of redirector
    // check is it 0 or not
    // 0 - code for the first byte of each unregisterred redirector
    void * redirectorAddress = getRedirectorAddressForName(name, context);
    printf("isFunctionRedirectorRegistered\n");
    if (*((unsigned int *)redirectorAddress) != 0){
	printf("%s is registered\n", name);
    }else
    {
	printf("%s is not registered\n", name);
    }

    return ((*((unsigned int *)redirectorAddress)) != 0);
}

// Add new function to the list of redirectors
void addNewFunction(char* name, void * functionAddr, struct RedirectorContext context){
    printf("addNewFunction %s\n", name);
    writeRedirectionCode( getRedirectorAddressForName(name, context), functionAddr);
}

// Initialize s_redirectors and s_names
void initWrapperRedirectors( struct RedirectorContext *context/*char** names,unsigned int count, void * wrapperAddr, void* s_redirectors*/){

    initFunctionInfoStorage();

    printf("Starting initWrapperRedirectors %p\n", context);

    // Memory allocation
    size_t allocSize = sizeof(void*)*REDIRECTOR_WORDS_SIZE*context->count + PAGESIZE-1;
    size_t i = 0;

    context->redirectors = (void *)malloc(allocSize);
    // Aligning by page border
    printf("Before aligment %x, %x\n", context->redirectors, sizeof(void*) * REDIRECTOR_WORDS_SIZE*context->count + PAGESIZE-1);
    //printf("Before aligment \n");
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
    for (i = 0 ; i < sizeof(void*)*REDIRECTOR_WORDS_SIZE*context->count ; i+=REDIRECTOR_SIZE){
        *((unsigned int*)(context->redirectors+i)) = 0;
    }


}

extern void testFunc(char * a){
	printf("Inside testFunc!!!\n");
}
