#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include "libtest_c.h"

int main()
{
    float resultFn2 = testFunction2(TEST_FUNCTION_2_ARG0, TEST_FUNCTION_2_ARG1);
    printf("Result of testFunction2 = %f\n", resultFn2);

    assert(fabs(resultFn2 - TEST_FUNCTION_2_RESULT) <= EPS);

    return 0;
}
