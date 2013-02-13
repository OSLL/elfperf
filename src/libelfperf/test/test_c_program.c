#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>

#include "libtest_c.h"

int main()
{
//    float resultFn2 = testFunction2(TEST_FUNCTION_2_ARG0, TEST_FUNCTION_2_ARG1);
//    printf("Result of testFunction2 = %f\n", resultFn2);

    void* p = dlopen("./libtest_c.so", RTLD_LAZY);

    void* f = dlsym(p, "testFunction2");
    float(*fn)(int, double) = (float(*)(int, double))f;

    float resultFn2 = fn(TEST_FUNCTION_2_ARG0, TEST_FUNCTION_2_ARG1);
    assert(fabs(resultFn2 - TEST_FUNCTION_2_RESULT) <= EPS);
    printf("Result of testFunction2 = %f\n", resultFn2);

    return 0;
}
