#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../ld-routines.h"
#include "../libelfperf.h"
#include "sys/mman.h"
#include <assert.h>


#define ARG0 1
#define ARG1 2
#define FUNCTION_3_RESULT 4
#define FUNCTION_4_RESULT 3

#define FUNCTION_5_RESULT 5.5
#define FUNCTION_6_RESULT 6.6

#define FUNCTION_8_RESULT 25

#define STRUCT_A_VALUE 1
#define STRUCT_B_VALUE 2
#define STRUCT_C_VALUE 3

struct testStruct
{
    int a;
    int b;
    int c;
};

void testFunction1() 
{
    printf("Inside of testFunction1(void)\n");
}

void testFunction2(int arg0, int arg1) 
{
    printf("Inside of testFunction2(%d, %d)\n", arg0, arg1);
}

int testFunction3() 
{
    printf("Inside of testFunction3(void)\n");
    return FUNCTION_3_RESULT;
}

int testFunction4(int arg0, int arg1) {
    printf("Inside of testFunction4(%d, %d)\n", arg0, arg1);
    return arg0 + arg1;
}

float testFunction5(double arg0)
{
    printf("Inside of testFunction5(%f)\n", arg0);
    return FUNCTION_5_RESULT;
}

double testFunction6() 
{
    printf("Inside of testFunction6(void)\n");
    return (double)FUNCTION_6_RESULT;
}

struct testStruct testFunction7()
{
    printf("Inside of testFunction7(void)\n");

    struct testStruct result;
    result.a = STRUCT_A_VALUE;
    result.b = STRUCT_B_VALUE;
    result.c = STRUCT_C_VALUE;

    return result;
}

int testFunction8(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9)
{
    printf("Inside of testFunction8(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)\n", arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

    return FUNCTION_8_RESULT;
}

int main()
{
    int count = 8;
    size_t allocSize = sizeof(void*) * REDIRECTOR_WORDS_SIZE * count + PAGESIZE - 1;
    size_t i = 0;
    void* redirectors = (void *)malloc(allocSize);

    printf("Before aligment %x, %x\n", redirectors, sizeof(void*) * REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1);
    redirectors = (void *)(((int) redirectors + PAGESIZE-1) & ~(PAGESIZE-1));
    int pagesNum = allocSize/PAGESIZE ;
    printf("Number of memory pages %x\n", pagesNum);
 
    for (i = 0; i < pagesNum; i++) {
        printf("Going to do mprotect\n");
        if (mprotect(redirectors + PAGESIZE*i, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            printf("Couldn't mprotect");
            return 0;
        }
    }

    printf("Zeroing\n");
    for (i = 0 ; i < sizeof(void*) * REDIRECTOR_WORDS_SIZE * count; i += REDIRECTOR_SIZE) {
        *((unsigned int*)(redirectors+i)) = 0;
    }

    printf("\n===== Start of test for wrappers and redirectors ======\n");

    int functionIndex;
    void* redirectorAddress;

/*    printf("\nTest #1:\n");
    functionIndex = 0;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
    writeRedirectionCode(redirectorAddress, testFunction1);
    void(*func1)() = redirectorAddress;
    (*func1)();
    printf("\tTest #1 finished\n");

    printf("\nTest #2:\n");
    functionIndex = 1;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction2);    
    void(*func2)(int, int) = redirectorAddress;
    (*func2)(128, 256);
    printf("\tTest #2 finished\n");

    printf("\nTest #3:\n");
    functionIndex = 2;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction3);
    int(*func3)() = redirectorAddress;
    int resultFn3 = (*func3)();
    assert(resultFn3 == FUNCTION_3_RESULT);
    printf("\tTest #3 finished with result: %d\n", resultFn3);
*/
    printf("\nTest #4:\n");
    functionIndex = 3;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction4);    
    int(*func4)(int, int) = redirectorAddress;
    int resultFn4 = (*func4)(1024, -64);
    assert(resultFn4 = FUNCTION_4_RESULT);
    printf("\tTest #4 finished with result: %d\n", resultFn4);

    printf("\nTest #5:\n");
    functionIndex = 4;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction5);    

    float resultF5 = testFunction5(0.5);

    float(*func5)(double) = redirectorAddress;
    float resultFn5 = (*func5)(0.5);
    //assert();
    printf("\tTest #5 finished with result: %f\n", resultFn5);

    printf("Size of float: %d\n", sizeof(float));
    printf("Size of double: %d\n", sizeof(double));
   
    printf("\nTest #6:\n");
    functionIndex = 5;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction6);    
    double(*func6)() = redirectorAddress;
    double resultFn6 = (*func6)();
    //assert();
    printf("\tTest #6 finished with result: %f\n", resultFn6);

    printf("\nTest #7:\n");
    functionIndex = 6;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction7);    
    struct testStruct(*func7)() = redirectorAddress;
    struct testStruct resultFn7 = (*func7)();
    //assert();
    printf("\tTest #7 finished with result: testStruct(%d, %d, %d)\n", resultFn7.a, resultFn7.b, resultFn7.c);

    printf("\nTest #8:\n");
    functionIndex = 8;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction8);    
    int(*func8)(int, int, int, int, int, int, int, int, int, int) = redirectorAddress;
    int resultFn8 = (*func8)(-4, -3, -2, -1, 0, 1, 2, 3, 4, 5);
    assert(resultFn8 == FUNCTION_8_RESULT);
    printf("\tTest #7 finished with result: %d\n", resultFn8);

    return 0;
}
