#include <stdio.h>

/**
 * This file contains of wrapper for cdecl functions
 * it use getFunctionPointer() function for obtainting address of a wrapped function
 *  
 */

// Global function pointer - stub for us, until we will get better way to 
// understand which function should be wrapped
static void * functionPointer = NULL;


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

	asm("jmp %0" : :"r"(getFunctionJmpAddress()) : );
}

