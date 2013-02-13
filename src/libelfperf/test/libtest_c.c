#include "libtest_c.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void testFunction1()
{
    printf("Inside of testFunction1()\n");
}

float testFunction2(int arg0, double arg1)
{
    printf("Inside of testFunction2(%d, %f)\n", arg0, arg1);
    assert(arg0 == TEST_FUNCTION_2_ARG0);
    assert(fabs(arg1 - TEST_FUNCTION_2_ARG1) <= EPS);

    return TEST_FUNCTION_2_RESULT;
}
