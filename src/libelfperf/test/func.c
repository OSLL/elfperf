#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include "func.h"

void testFunction1() 
{
    printf("TEST_LOG: Inside of testFunction1(void)\n");
}

void testFunction2(int arg0, int arg1) 
{
    printf("TEST_LOG: Inside of testFunction2(%d, %d)\n", arg0, arg1);
    assert(arg0 == TEST2_ARG0);
    assert(arg1 == TEST2_ARG1);
}

int testFunction3() 
{
    printf("TEST_LOG: Inside of testFunction3(void)\n");
    return TEST3_RESULT;
}

int testFunction4(int arg0, int arg1)
{
    printf("TEST_LOG: Inside of testFunction4(%d, %d)\n", arg0, arg1);
    assert(arg0 == TEST4_ARG0);
    assert(arg1 == TEST4_ARG1);

    return TEST4_RESULT;
}

float testFunction5(double arg0)
{
    printf("TEST_LOG: Inside of testFunction5(%f)\n", arg0);
    assert(abs(arg0 - TEST5_ARG0) <= EPS);
    return TEST5_RESULT;
}

double testFunction6() 
{
    printf("TEST_LOG: Inside of testFunction6(void)\n");
    return (double)TEST6_RESULT;
}

struct testStruct2 testFunction7(struct testStruct arg0)
{
    printf("TEST_LOG: Inside of testFunction7(testStruct(%d, %d, %d))\n", arg0.a, arg0.b, arg0.c);
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
    printf("TEST_LOG: Inside of testFunction8(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)\n", arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
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

struct testStruct testFunction9(struct testStruct2 arg0)
{
    printf("TEST_LOG: Inside of testFunction9(testStruct2(%d, %d, %lu, %f, %lu))\n", arg0.a, arg0.b, arg0.c, arg0.d, arg0.e);
    assert(arg0.a == TEST9_ARG0_A);
    assert(arg0.b == TEST9_ARG0_B);
    assert(arg0.c == TEST9_ARG0_C);
    assert(abs(arg0.d - TEST9_ARG0_D) <= EPS);
    assert(arg0.e == TEST9_ARG0_E);

    struct testStruct result;
    result.a = TEST9_RESULT_A;
    result.b = TEST9_RESULT_B;
    result.c = TEST9_RESULT_C;

    return result;
}

// Calc int sum and return
int testFunction10(int num, ...){
    va_list arguments;

    assert(num == TEST10_ARG0);

    int sum = 0, i = 0, currentArg;
    int trueResults[] = {TEST10_ARG1, TEST10_ARG2, TEST10_ARG3};

    va_start ( arguments, num );           
    printf ("TEST_LOG: Inside of testFunction10(%d", num);
    for ( ; i < num; i++ )        
    {
        currentArg = (int) va_arg ( arguments, int );
        assert( currentArg == trueResults[i]);
        printf(", %d", currentArg);
        sum += currentArg; 
    }
    printf(")\n");


    va_end ( arguments ); 
    return sum;

}

// Calc int sum and return
double testFunction11(int num, ...){
    va_list arguments;

    assert(num == TEST11_ARG0);

    double sum = 0.; 
    int i = 0;
    double currentArg;
    double trueResults[] = {TEST11_ARG1, TEST11_ARG2, TEST11_ARG3};

    va_start ( arguments, num );           
    printf ("TEST_LOG: Inside of testFunction11(%f", num);
    for ( ; i < num; i++ )        
    {
        currentArg = (double) va_arg ( arguments, double );
        assert( currentArg == trueResults[i]);
        printf(", %f", currentArg);
        sum += currentArg; 
    }
    printf(")\n");


    va_end ( arguments ); 
    return sum;

}
