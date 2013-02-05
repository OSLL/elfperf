#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sys/mman.h"
#include <assert.h>
#include <math.h>
#include "../ld-routines.h"
#include "../libelfperf.h"

#define EPS 0.000000001

// Test 1
// ...

// Test 2
#define TEST2_ARG0 256
#define TEST2_ARG1 -150 

// Test 3
#define TEST3_RESULT 17

// Test 4
#define TEST4_ARG0 117
#define TEST4_ARG1 1024 
#define TEST4_RESULT TEST4_ARG0 + TEST4_ARG1

// Test 5
#define TEST5_ARG0 0.00125
#define TEST5_RESULT 0.008

// Test 6
#define TEST6_RESULT 0.000512

// Test 7
#define TEST7_ARG0_A 25
#define TEST7_ARG0_B 0
#define TEST7_ARG0_C -75
#define TEST7_RESULT_A 12
#define TEST7_RESULT_B -100
#define TEST7_RESULT_C 12345678
#define TEST7_RESULT_D 0.481516
#define TEST7_RESULT_E 9999999

// Test 8
#define TEST8_RESULT 10000
#define TEST8_ARG0 -4
#define TEST8_ARG1 -3
#define TEST8_ARG2 -2
#define TEST8_ARG3 -1
#define TEST8_ARG4 -0
#define TEST8_ARG5 -1
#define TEST8_ARG6 -2
#define TEST8_ARG7 -3
#define TEST8_ARG8 -4
#define TEST8_ARG9 -5


struct testStruct
{
    int a;
    int b;
    int c;
};

struct testStruct2
{
    int a;
    int b;
    uint64_t c;
    double d;
    uint64_t e;
};


void testFunction1() 
{
    printf("Inside of testFunction1(void)\n");
}

void testFunction2(int arg0, int arg1) 
{
    printf("Inside of testFunction2(%d, %d)\n", arg0, arg1);
    assert(arg0 == TEST2_ARG0);
    assert(arg1 == TEST2_ARG1);
}

int testFunction3() 
{
    printf("Inside of testFunction3(void)\n");
    return TEST3_RESULT;
}

int testFunction4(int arg0, int arg1) {
    printf("Inside of testFunction4(%d, %d)\n", arg0, arg1);
    assert(arg0 == TEST4_ARG0);
    assert(arg1 == TEST4_ARG1);

    return TEST4_RESULT;
}

float testFunction5(double arg0)
{
    printf("Inside of testFunction5(%f)\n", arg0);
    assert(abs(arg0 - TEST5_ARG0) <= EPS);
    return TEST5_RESULT;
}

double testFunction6() 
{
    printf("Inside of testFunction6(void)\n");
    return (double)TEST6_RESULT;
}

struct testStruct2 testFunction7(struct testStruct arg0)
{
    printf("Inside of testFunction7(testStruct(%d, %d, %d))\n", arg0.a, arg0.b, arg0.c);
    assert(arg0.a == TEST7_ARG0_A);
    assert(arg0.b == TEST7_ARG0_B);
    assert(arg0.c == TEST7_ARG0_C);

    struct testStruct2 result;
    result.a = TEST7_RESULT_A;
    result.b = TEST7_RESULT_B;
    result.c = TEST7_RESULT_C;
    result.d = TEST7_RESULT_D;
    result.e = TEST7_RESULT_E;

    return result;
}

int testFunction8(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9)
{
    printf("Inside of testFunction8(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)\n", arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    assert(arg0 == TEST8_ARG0);
    assert(arg1 == TEST8_ARG1);
    assert(arg2 == TEST8_ARG2);
    assert(arg3 == TEST8_ARG3);
    assert(arg4 == TEST8_ARG4);
    assert(arg5 == TEST8_ARG5);
    assert(arg6 == TEST8_ARG6);
    assert(arg7 == TEST8_ARG7);
    assert(arg8 == TEST8_ARG8);
    assert(arg9 == TEST8_ARG9);

    return TEST8_RESULT;
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

    printf("\nTest #1:\n");
    functionIndex = 0;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
    writeRedirectionCode(redirectorAddress, testFunction1);
    void(*func1)() = redirectorAddress;
    (*func1)();
    printf("\nTest #1 is successfull!!!\n");

    printf("\nTest #2:\n");
    functionIndex = 1;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction2);    
    void(*func2)(int, int) = redirectorAddress;
    (*func2)(TEST2_ARG0, TEST2_ARG1);
    printf("\nTest #2 is successfull!!!\n");

    printf("\nTest #3:\n");
    functionIndex = 2;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction3);
    int(*func3)() = redirectorAddress;
    int resultFn3 = (*func3)();
    assert(resultFn3 == TEST3_RESULT);
    printf("\nTest #3 is successfull!!! Result = %d\n", resultFn3);

    printf("\nTest #4:\n");
    functionIndex = 3;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction4);    
    int(*func4)(int, int) = redirectorAddress;
    int resultFn4 = (*func4)(TEST4_ARG0, TEST4_ARG1);
    assert(resultFn4 = TEST4_RESULT);
    printf("\nTest #4 is successfull!!! Result = %d\n", resultFn4);

    printf("\nTest #5:\n");
    functionIndex = 4;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction5);    
    float(*func5)(double) = redirectorAddress;
    float resultFn5 = (*func5)(TEST5_ARG0);
    assert(abs(resultFn5 - TEST5_RESULT) <= EPS);
    printf("\nTest #5 is successfull!! Result = %f\n", resultFn5);

 
    printf("\nTest #6:\n");
    functionIndex = 5;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction6);    
    double(*func6)() = redirectorAddress;
    double resultFn6 = (*func6)();
    assert(abs(resultFn6 - (double)(TEST6_RESULT)) <= EPS);
    printf("\nTest #6 is successfull!!! Result = %f\n", resultFn6);

    printf("\nTest #7:\n");
    functionIndex = 6;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction7);    
    struct testStruct2(*func7)(struct testStruct) = redirectorAddress;
    struct testStruct arg0;
    arg0.a = TEST7_ARG0_A;
    arg0.b = TEST7_ARG0_B;
    arg0.c = TEST7_ARG0_C;
    struct testStruct2 resultFn7 = (*func7)(arg0);
    assert(resultFn7.a == TEST7_RESULT_A);
    assert(resultFn7.b == TEST7_RESULT_B);
    assert(resultFn7.c == TEST7_RESULT_C);
    assert(abs(resultFn7.a - (double)TEST7_RESULT_A) <= EPS);
    assert(resultFn7.a == TEST7_RESULT_A);
    printf("\nTest #7 is successfull!!! Result = testStruct(%d, %d, %d, %f, %d)\n", resultFn7.a, resultFn7.b, resultFn7.c, resultFn7.d, resultFn7.e);

    printf("\nTest #8:\n");
    functionIndex = 8;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction8);    
    int(*func8)(int, int, int, int, int, int, int, int, int, int) = redirectorAddress;
    int resultFn8 = (*func8)(TEST8_ARG0, TEST8_ARG1, TEST8_ARG2, TEST8_ARG3, TEST8_ARG4, TEST8_ARG5, TEST8_ARG6, TEST8_ARG7, TEST8_ARG8, TEST8_ARG9);
    assert(resultFn8 == TEST8_RESULT);
    printf("\nTest #8 is successfull!!! Result = %d\n", resultFn8);

    return 0;
}
