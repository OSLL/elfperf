#include <stdio.h>

/**
 * This file contains of wrapper for cdecl functions
 * it use getFunctionPointer() function for obtainting address of a wrapped function
 *  
 */

// FIXME doesnt support recoursive calls  because of global stack variables usage
struct WrappingContext{
	// real return address
	void * realReturnAddr; 	// 4bytes
	// content of -4(%%old_ebp)
	void * oldEbpLocVar; 		// 4bytes
	// function return value
	void * eax;		// 4bytes
	double doubleResult;	// 8bytes 
};

// Global function pointer - stub for us, until we will get better way to 
// understand which function should be wrapped
static void * functionPointer = NULL;

// Workaround - storing context in global variable
static struct WrappingContext context;

// This function returns address 
static struct WrappingContext * getNewContext(){
	// STUB 
	return &context;
}



// This global variable is storing real return address
//static void * realReturnAddr = NULL;
//static void * eax = NULL;

void * getFunctionPointer(){
	return functionPointer;
}

// Return actual address for jmp
void * getFunctionJmpAddress(){
	/* Adding 3 to addr because each cdecl function contains such code at the start:
	55 	push   %ebp
	89 e5	mov    %esp,%ebp

	This code is changing stack (we dont want to) so we need to skip it.
	*/
	return functionPointer+3;
}

// Set an pointer to wrapped function
void setFunctionPointer(void * pointer){
	functionPointer = pointer;
}



// Wrapper code. 
// We doesnt touch stack and variables, just print something and jmp to wrapped function.
void wrapper(){
	/*
	*/
	asm volatile(				
		"movl (%%ebp), %%ebx\n"			// Start of the moved area 
		"subl $0x4, %%ebx\n"			// ebx = old_ebp - 4
		"movl (%%ebx), %%edx\n"			// edx = -4(%%old_ebp) = (%%ebp)
		// Storing context pointer into freed space
		"call getNewContext\n"			// eax = getNewContext() 
		"movl %%eax, (%%ebx)\n" 
		"movl %%edx, 4(%%eax)\n"		//context->oldEbpLocVar = edx
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
		"movl 4(%%ebp), %%ecx\n"		// Storing real return_addres
		"movl %%ecx, (%%ebx) \n"
		"movl %%ebp, %%ecx\n"                	// Storing ebp for this frame 
                "movl %%ecx, 4(%%ebx) \n"		// needed for backshofting stackframe after $wrapper_return_point$
		// changing return address for $wrapper_return_point
		"movl $wrapper_return_point, 4(%%ebp)\n" : : :
	);	


	// going to wrapped function
	asm("jmp %0" : :"r"(getFunctionJmpAddress()) : );

	// returning back into wrapper()
	// 	memorizing eax value
	asm volatile(
		// Calculating address of WrappingContext and memorizing return values
		"wrapper_return_point: movl -4(%%ebp), %%ebx\n"	// %%ebx = & context  
		"movl %%eax, 8(%%ebx)\n"			// context->eax = %%eax
		"fstpl 0xc(%%ebx)\n"				// context->doubleResult = ST0
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

