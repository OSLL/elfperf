#include <stdio.h>

/**
 * This file contains of wrapper for cdecl functions
 * it use getFunctionPointer() function for obtainting address of a wrapped function
 *  
 */

// FIXME doesnt support recoursive calls  because of global stack variables usage

// Global function pointer - stub for us, until we will get better way to 
// understand which function should be wrapped
static void * functionPointer = NULL;

// This global variable is storing real return address
static void * realReturnAddr = NULL;
static void * eax = NULL;

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
	puts("WRAPPED!");	
	// memorize old return addres and change it for returning in wrapper()
	// stack variables will be damaged, so i use global variable
	asm(	"movl 4(%%ebp), %0\n"
		"movl $wrapper_return_point, 4(%%ebp)":"=r"(realReturnAddr));	

	// going to wrapped function
	asm("jmp %0" : :"r"(getFunctionJmpAddress()) : );

	// returning back into wrapper()
	// memorizing eax value
	asm(	"wrapper_return_point: nop\n"
		"movl %%eax, %0":"=m"(eax):);

	// Change this call to any needed routine
	puts("back to wrapper!");	

	// restoring real return_address and eax and return
	asm(	"movl %1, %%eax\n"
		"jmp %0" : :"r"(realReturnAddr), "r"(eax) : "%eax");
}

