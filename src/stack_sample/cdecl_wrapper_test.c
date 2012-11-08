#include <stdio.h>

#define ARG0 1
#define ARG1 2
#define FUNCTION_3_RESULT 4
#define FUNCTION_4_RESULT 3

void testFunction1(){
	printf("in testFunction1\n");
}

void testFunction2(int arg0, int arg1){
        printf("in testFunction2( %d, %d)\n", arg0, arg1);
}

int testFunction3(){
	printf("in testFunction3\n");
	return FUNCTION_3_RESULT;
}

int testFunction4(int arg0, int arg1){
	printf("in testFunction4( %d, %d)\n", arg0, arg1);
	return arg0 + arg1;
}

int main(){
	int result = 0;

	// Setting up wrapper for testFunction1		
	setFunctionPointer(testFunction1);
	// Calling wrapper instead of testFunction1
	printf("calling wrapper for testFunction1\n");
	wrapper();
	printf("wrapper called for testFunction1\n");

	/////////////////////////////////////////////
	setFunctionPointer(testFunction2);
	printf("\ncalling wrapper for testFunction2\n");
	// Pushing arguments
	asm("pushl $1");
	asm("pushl $2");
	// Calling wrapper instead of testFunction2
	wrapper();
	printf("wrapper called for testFunction2\n");


	/////////////////////////////////////////////
	setFunctionPointer(testFunction3);
	// Calling wrapper instead of testFunction3
	printf("\ncalling wrapper for testFunction3\n");
	wrapper();
	// Getting function return value from eax
	asm("movl %%eax, %0" :"=r"(result) );
	printf("wrapper called for testFunction3, result = %d, expected_result = %d\n", result, FUNCTION_3_RESULT );

	////////////////////////////////////////////
	setFunctionPointer(testFunction4);
	printf("\ncalling wrapper for testFunction4\n");
	// Pushing arguments
	asm("pushl $1");
	asm("pushl $2");
	// Calling wrapper instead of testFunction4
	wrapper();
	// Getting function return value from eax
	asm("movl %%eax, %0" :"=r"(result) );
	printf("wrapper called for testFunction4, result = %d, expected_result = %d\n", result, FUNCTION_4_RESULT );

	return 0;
}
