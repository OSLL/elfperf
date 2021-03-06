#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include "functions.h"

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

int testFunction4(int arg0, int arg1)
{
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
