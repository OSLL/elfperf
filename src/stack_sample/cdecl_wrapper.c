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
	// ebp register value -  border of moved area
	void * ebp; 		// 4bytes
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
		Stack shifted to the bottom for one word(4 bytes) \/
		Shifting stareted from the end to start
		Address of fried space is a -4(old_ebp) 
	*/
	asm volatile(	"movl %%ebp, %%ecx\n"		// End of moved area 	ecx = ebp	
		"movl (%%ebp), %%ebx\n"			// Start of the moved area 
		"subl $0x4, %%ebx\n"			// ebx = old_ebp - 4
		"shift_act: movl (%%ecx), %%eax\n" 	// Loop:
			"movl %%eax, -4(%%ecx)\n" 		// -4(%%ecx) = (%%ecx)
			"addl $0x4, %%ecx\n" 			// %%ecx += 4
			"cmp %%ecx, %%ebx\n" 			// if (%%ecx == %%ebx)
			"jne shift_act\n"			// goto loop_act
		"subl $0x4, %%esp\n"			// %%esp -= 4
		"subl $0x4, %%ebp\n"			// %%ebp -= 4
		// Storing context pointer into freed space
		"call getNewContext\n"			// eax = getNewContext() 
		"movl %%eax, (%%ebx)\n" : : :		// (%%ebx) = %%eax
		 
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
			4(%%ebx)	ebp
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
		"wrapper_return_point: movl -4(%%ebp), %%ebx\n"	// 
		"movl %%eax, 8(%%ebx)\n"			// context->eax = %%eax
		"fstpl 0xc(%%ebx)\n"				// context->doubleResult = ST0
		// Context pointer saving to %%edx
		"movl %%ebx, %%edx\n"				// %%edx =  &context
		// Backshift of stack frame
		"movl 4(%%ebx), %%ecx\n" 			// %%ebx = ebp
		"movl %%ecx, %%ebx\n"
		"movl %%ebp, %%ecx\n"				// %%ecx = old_ebp-4
		"subl $0x4, %%ecx\n"
		"backshift_act: movl (%%ecx), %%eax\n"
			"movl %%eax, 4(%%ecx)\n"
			"subl $0x4, %%ecx\n"
			"cmp %%ecx, %%ebx\n"
			"jng backshift_act\n"
		"addl 0x4, %%esp\n"				// esp += 4
		"pushl %%edx\n"	: : :				// push &context
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
		"popl %%ebx\n"			// get context address
		"movl 0xa(%%ebx), %%eax\n"	// restoring eax
		"fldl 0xc(%%ebx)\n"		// restoring ST0
		"pushl (%%ebx)\n"			// returning to caller
		"ret\n" : : :
	);

}

