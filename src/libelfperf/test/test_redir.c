#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "sys/mman.h"
#include <assert.h>
#include <math.h>
#include "func.h"

#define _dl_debug_printf printf

#include "../ld-routines.h"
#include "../libelfperf.h"

//#define PAGESIZE 4096
//#define REDIRECTOR_WORDS_SIZE 4
//#define REDIRECTOR_SIZE 16


void* symbols[] = {
                    testFunction1, testFunction2, testFunction3, testFunction4, 
                    testFunction5, testFunction6, testFunction7, testFunction8,
                    testFunction9
                  };

int main()
{
    // Initializing shared memory for functions storage
    struct RedirectorContext context;
    context.names = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &(context.count));

    initWrapperRedirectors(&context);

    int i;
    for (i = 0; i < context.count; i++) {
        addNewFunction(context.names[i], symbols[i], context);
    }

    fprintf(stdout, "\n===== Test of redirection mechanism  ======\n");

    fprintf(stdout, "\nTest #1:\n");
    i = 0;
    void(*func1)() = getRedirectorAddressForName(context.names[i], context);
    (*func1)();
    fprintf(stdout, "Test #1 is successfull!!!\n");

    fprintf(stdout, "\nTest #2:\n");
    void(*func2)(int, int) = getRedirectorAddressForName(context.names[i], context);
    (*func2)(TEST2_ARG0, TEST2_ARG1);
    fprintf(stdout, "Test #2 is successfull!!!\n");

    fprintf(stdout, "\nTest #3:\n");
    i = 2;
    int(*func3)() = getRedirectorAddressForName(context.names[i], context);
    int resultFn3 = (*func3)();
    assert(resultFn3 == TEST3_RESULT);
    fprintf(stdout, "Test #3 is successfull!!! Result = %d\n", resultFn3);

    fprintf(stdout, "\nTest #4:\n");
    i = 3;
    int(*func4)(int, int) = getRedirectorAddressForName(context.names[i], context);
    int resultFn4 = (*func4)(TEST4_ARG0, TEST4_ARG1);
    assert(resultFn4 = TEST4_RESULT);
    fprintf(stdout, "Test #4 is successfull!!! Result = %d\n", resultFn4);

    fprintf(stdout, "\nTest #5:\n");
    i = 4;
    float(*func5)(double) = getRedirectorAddressForName(context.names[i], context);
    float resultFn5 = (*func5)(TEST5_ARG0);
    assert(abs(resultFn5 - TEST5_RESULT) <= EPS);
    fprintf(stdout, "Test #5 is successfull. Result = %f\n", resultFn5);

    fprintf(stdout, "\nTest #6:\n");
    i = 5;
    double(*func6)() = getRedirectorAddressForName(context.names[i], context);
    double resultFn6 = (*func6)();
    assert(abs(resultFn6 - (double)(TEST6_RESULT)) <= EPS);
    fprintf(stdout, "Test #6 is successfull. Result = %f\n", resultFn6);

    fprintf(stdout, "\nTest #7:\n");
    i = 6;
    struct testStruct2(*func7)(struct testStruct) = getRedirectorAddressForName(context.names[i], context);
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
    fprintf(stdout, "Test #7 is successfull. Result = testStruct2(%d, %d, %lu, %f, %lu)\n", resultFn7.a, resultFn7.b, resultFn7.c, resultFn7.d, resultFn7.e);

    fprintf(stdout, "\nTest #8:\n");
    i = 7;
    int(*func8)(int, int, int, int, int, int, int, int, int, int) = getRedirectorAddressForName(context.names[i], context);
    int resultFn8 = (*func8)(TEST8_ARG0, TEST8_ARG1, TEST8_ARG2, TEST8_ARG3, TEST8_ARG4, TEST8_ARG5, TEST8_ARG6, TEST8_ARG7, TEST8_ARG8, TEST8_ARG9);
    assert(resultFn8 == TEST8_RESULT);
    fprintf(stdout, "Test #8 is successfull. Result = %d\n", resultFn8);

    fprintf(stdout, "\nTest #9:\n");
    i = 8;
    struct testStruct(*func9)(struct testStruct2 arg0) = getRedirectorAddressForName(context.names[i], context);
    struct testStruct2 arg0_test9;
    arg0_test9.a = TEST9_ARG0_A;
    arg0_test9.b = TEST9_ARG0_B;
    arg0_test9.c = TEST9_ARG0_C;
    arg0_test9.d = TEST9_ARG0_D;
    arg0_test9.e = TEST9_ARG0_E;
    struct testStruct resultFn9 = (*func9)(arg0_test9);
    assert(resultFn9.a == TEST9_RESULT_A);
    assert(resultFn9.b == TEST9_RESULT_B);
    assert(resultFn9.c == TEST9_RESULT_C);
    fprintf(stdout, "Test #9 is successfull. Result = testStruct(%d, %d, %d)\n", resultFn9.a, resultFn9.b, resultFn9.c);

    return 0;
}
