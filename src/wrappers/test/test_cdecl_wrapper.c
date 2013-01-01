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
 * \file test_cdecl_wrapper.c
 * \brief Test for wrapper
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "../cdecl_wrapper.h"
#include "../../timers/global_stats.h"

// This tests check how wrapper work
// For all test functions wrapper is setted up for their pointers,
// arguments is pushed to stack(if need) and wrapper call is performed
// after it check of return value is performed

#define ARG0 1
#define ARG1 2
#define FUNCTION_3_RESULT 4
#define FUNCTION_4_RESULT 3


#define FUNCTION_5_RESULT 5.5
#define FUNCTION_6_RESULT 6.6

#define STRUCT_A_VALUE 1
#define STRUCT_B_VALUE 2
#define STRUCT_C_VALUE 3

typedef void (func)();

// Test structure, with size > 8byte
struct testStruct{
    int a;
    int b;
    int c;
};

// Test functions
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

float testFunction5(){
    printf("in testFunction5()\n");
    return FUNCTION_5_RESULT;
}

double testFunction6(){
    printf("in testFunction6()\n");
    return (double)FUNCTION_6_RESULT;
}

struct testStruct testFunction7(){
//	void * addr;
//	asm("movl 0x8(%%ebp),%0":"=r"(addr));
//	printf("in testFunction7(hidden_parameter = %x)\n",addr);

    printf("in testFunction7()\n");

    struct testStruct result;
    result.a = STRUCT_A_VALUE;
    result.b = STRUCT_B_VALUE;
    result.c = STRUCT_C_VALUE;

    return result;
}

char * testFuncNames [7]= {"testFunction1","testFunction2","testFunction3","testFunction4",
    "testFunction5","testFunction6","testFunction7"};

void * funcPointers[7] = {testFunction1, testFunction2, testFunction3, testFunction4, 
    testFunction5, testFunction6,testFunction7};

int main(){
    int intResult = 0;
    float floatResult=0.;
    double doubleResult=0.;

    struct testStruct * structResult=NULL;

    initWrapperRedirectors(testFuncNames, 7, (void *)wrapper);
    unsigned int i = 0;
    for (i = 0; i < 7 ; i++){
        addNewFunction(testFuncNames[i], funcPointers[i]);
    }

    // Workaround for test
    // Each function which return object with size >8byte should have one hidden parameter
    // address  where memory is allocated
    struct testStruct workaroundVar ;//= (struct testStruct*)malloc(sizeof(struct testStruct));

    printf("Type sizes: sizeof(int) = %d, sizepof(double) = %d, sizeof(float) = %d,"
        "sizeof(longlong) = %d, sizeof(testStruct) = %d, sizeof(void*) = %d, sizeof(unsigned int) = %d\n",
        sizeof(int), sizeof(double), sizeof(float), sizeof(long long), sizeof(struct testStruct), sizeof(void*),
        sizeof(unsigned int));

    // Setting up wrapper for testFunction1
    // Calling wrapper instead of testFunction1
    printf("calling wrapper for testFunction1\n");
    func *func1 = getRedirectorAddressForName("testFunction1");
    (*func1)();
    (*func1)();
    (*func1)();
    (*func1)();
    (*func1)();
    (*func1)();
    (*func1)();
    (*func1)();
    printf("wrapper called for testFunction1\n");

    /////////////////////////////////////////////
    //setFunctionPointer(testFunction2);
    printf("\ncalling wrapper for testFunction2\n");
    void (*func2)(int,int) = getRedirectorAddressForName("testFunction2");
    // Calling wrapper instead of testFunction2
    (*func2)(1,2);
    (*func2)(1,2);
    (*func2)(1,2);
    printf("wrapper called for testFunction2\n");

    /////////////////////////////////////////////
    // Calling wrapper instead of testFunction3
    printf("\ncalling wrapper for testFunction3\n");
    func *func3 = getRedirectorAddressForName("testFunction3");
    (*func3)();
    (*func3)();
    // Getting function return value from eax
    asm("movl %%eax, %0" :"=r"(intResult) );
    printf("wrapper called for testFunction3, result = %d, expected_result = %d\n", intResult, FUNCTION_3_RESULT );

    ////////////////////////////////////////////
    printf("\ncalling wrapper for testFunction4\n");
    int (*func4)(int, int) = getRedirectorAddressForName("testFunction4");
    // Pushing arguments
    // Calling wrapper instead of testFunction4
    (*func4)(1, 2);
    (*func4)(1, 2);
    (*func4)(1, 2);
    (*func4)(1, 2);
    (*func4)(1, 2);
    // Getting function return value from eax
    asm("movl %%eax, %0" :"=r"(intResult) );
    printf("wrapper called for testFunction4, result = %d, expected_result = %d\n", intResult, FUNCTION_4_RESULT );

    /////////////////////////////////////////////
    // Calling wrapper instead of testFunction5
    printf("\ncalling wrapper for testFunction5\n");
    func *func5 = getRedirectorAddressForName("testFunction5");
    // Calling wrapper
    (*func5)();
    (*func5)();
    (*func5)();
    // Getting function return value from eax
    asm("mov %%eax, %0" :"=m"(floatResult));
    printf("wrapper called for testFunction5, result = %f, expected_result = %f\n", floatResult, FUNCTION_5_RESULT );

    /////////////////////////////////////////////
    // Calling wrapper instead of testFunction6
    printf("\ncalling wrapper for testFunction6\n");
    func *func6 = getRedirectorAddressForName("testFunction6");
    // Calling wrapper
    (*func6)();
    // Getting function return value from st0
    asm("fstpl %0" :"=m"(doubleResult));
    printf("wrapper called for testFunction6, result = %f, expected_result = %f\n", doubleResult, FUNCTION_6_RESULT );

    /////////////////////////////////////////////
    // Calling wrapper instead of testFunction7
    printf("\ncalling wrapper for testFunction7, hidden_param = %x\n", &workaroundVar);
    struct testStruct (*func7)() = getRedirectorAddressForName("testFunction7");
    // calling wrapper and storing return value
    workaroundVar =  ((*func7)());
    structResult = &workaroundVar;

    printf("wrapper called for testFunction7, result = (%d,%d,%d), expected_result = (%d,%d,%d)\n", structResult->a,
        structResult->b, structResult->c, STRUCT_A_VALUE, STRUCT_B_VALUE, STRUCT_C_VALUE);

    printf("=== Total statistics ===\n");
    struct FunctionStatistic* stat = getFunctionStatistic(testFunction1);
    printf("Function1 statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);
    stat = getFunctionStatistic(testFunction2);
    printf("Function2 statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);
    stat = getFunctionStatistic(testFunction3);
    printf("Function3 statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);
    stat = getFunctionStatistic(testFunction4);
    printf("Function4 statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);
    stat = getFunctionStatistic(testFunction5);
    printf("Function5 statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);
    stat = getFunctionStatistic(testFunction6);
    printf("Function6 statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);
    stat = getFunctionStatistic(testFunction7);
    printf("Function7 statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);

    return 0;
}
