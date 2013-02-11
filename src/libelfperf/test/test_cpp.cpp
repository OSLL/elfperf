#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
using namespace std;

#include "func.h"
#include "libtest_cpp.h"

//#define _dl_debug_printf printf
 
extern "C" {
    #include "../ld-routines.h"
    #include "../libelfperf.h"
}

void* symbols[] = {
                    (void*)testFunction1, (void*)testFunction2, (void*)testFunction3, (void*)testFunction4,
                    (void*)testFunction5, (void*)testFunction6, (void*)testFunction7, (void*)testFunction8,
                    (void*)testFunction9, (void*)(&TestClass1::getA), (void*)(&TestClass1::getB), (void*)(&TestClass1::getC),
                    (void*)(&TestClass1::setA), (void*)(&TestClass1::setB), (void*)(&TestClass1::setC), (void*)(&TestClass1::function1)
                  };

/*
char* names[] = { 
                  "_Z13testFunction1v", "_Z13testFunction2ii", "_Z13testFunction3v", "_Z13testFunction4ii",
                  "_Z13testFunction5d", "_Z13testFunction6v", "_Z13testFunction710testSt", "_Z13testFunction8iiiiiiii",
                  "_Z13testFunction911testSt", "_ZN10TestClass14getAEv", "_ZN10TestClass14getBEv", "_ZN10TestClass14getCEv"
                };
*/
int main()
{
    struct RedirectorContext context;
    context.names = get_fn_list(ELFPERF_PROFILE_FUNCTION_ENV_VARIABLE, &(context.count));

    initWrapperRedirectors(&context);
    for (int i = 0; i < context.count; i++) {
       addNewFunction(context.names[i], symbols[i], context); 
    }

    cout << "\n===== Test of C++ programs  ======\n";;
  
    cout << "\nTest #1:" << endl;
    int i = 0;
    void(*func1)() = (void(*)())getRedirectorAddressForName(context.names[i], context);
    (*func1)();
    cout << "Test #1 is successfull!!!" << endl;

    cout << "\nTest #2:" << endl;
    i = 1;
    void(*func2)(int, int) = (void(*)(int, int))getRedirectorAddressForName(context.names[i], context);
    (*func2)(TEST2_ARG0, TEST2_ARG1);
    cout << "Test #2 is successfull!!!" << endl;

    cout << "\nTest #3:" << endl;
    i = 2;
    int(*func3)() = (int(*)())getRedirectorAddressForName(context.names[i], context);
    int resultFn3 = (*func3)();
    assert(resultFn3 == TEST3_RESULT);
    cout << "Test #3 is successfull!!! Result = " << resultFn3 << endl;

    cout << "\nTest #4:" << endl;
    i = 3;
    int(*func4)(int, int) = (int(*)(int, int))getRedirectorAddressForName(context.names[i], context);
    int resultFn4 = (*func4)(TEST4_ARG0, TEST4_ARG1);
    assert(resultFn4 = TEST4_RESULT);
    cout << "Test #4 is successfull!!! Result = " << resultFn4 << endl;

    cout << "\nTest #5:" << endl;
    i = 4;
    float(*func5)(double) = (float(*)(double))getRedirectorAddressForName(context.names[i], context);
    float resultFn5 = (*func5)(TEST5_ARG0);
    assert(abs(resultFn5 - TEST5_RESULT) <= EPS);
    cout << "Test #5 is successfull. Result = " << resultFn5 << endl;

    cout << "\nTest #6:" << endl;
    i = 5;
    double(*func6)() = (double(*)())getRedirectorAddressForName(context.names[i], context);
    double resultFn6 = (*func6)();
    assert(abs(resultFn6 - (double)(TEST6_RESULT)) <= EPS);
    cout << "Test #6 is successfull. Result = " << resultFn6 << endl;

    cout << "\nTest #7:" << endl;
    i = 6;
    struct testStruct2(*func7)(struct testStruct) = (struct testStruct2(*)(struct testStruct))getRedirectorAddressForName(context.names[i], context);
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
    cout << "Test #7 is successfull. Result = testStruct2(" << resultFn7.a << ", " << resultFn7.b << ", " << resultFn7.c << ", " << resultFn7.d << ", " << resultFn7.e << ")" << endl;

    cout << "\nTest #8:" << endl;
    i = 7;
    int(*func8)(int, int, int, int, int, int, int, int, int, int) = (int(*)(int, int, int, int, int, int, int, int, int, int))getRedirectorAddressForName(context.names[i], context);
    int resultFn8 = (*func8)(TEST8_ARG0, TEST8_ARG1, TEST8_ARG2, TEST8_ARG3, TEST8_ARG4, TEST8_ARG5, TEST8_ARG6, TEST8_ARG7, TEST8_ARG8, TEST8_ARG9);
    assert(resultFn8 == TEST8_RESULT);
    cout << "Test #8 is successfull. Result = " << resultFn8 << endl;

    cout << "\nTest #9:" << endl;
    i = 8;
    struct testStruct(*func9)(struct testStruct2 arg0) = (struct testStruct(*)(struct testStruct2))getRedirectorAddressForName(context.names[i], context);
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
    cout << "Test #9 is successfull. Result = testStruct(" << resultFn9.a << ", " << resultFn9.b << ", " << resultFn9.c << ")" << endl;
   

    TestClass1* obj1 = new TestClass1(TESTCLASS1_A, TESTCLASS1_B, (double)TESTCLASS1_C);

    cout << "\nTest #10: Class Get-Methods" << endl;
    i = 9;
    int (*func10) (TestClass1*) = reinterpret_cast<int(*)(TestClass1*)>(getRedirectorAddressForName(context.names[i], context));
    int resultTest10_a = func10(obj1);
    assert(resultTest10_a == TESTCLASS1_A);
    i = 10;
    func10 = reinterpret_cast<int(*)(TestClass1*)>(getRedirectorAddressForName(context.names[i], context));
    int resultTest10_b = func10(obj1);
    assert(resultTest10_b == TESTCLASS1_B);
    i = 11;
    double(*func10_1)(TestClass1*) = reinterpret_cast<double(*)(TestClass1*)>(getRedirectorAddressForName(context.names[i], context));
    double resultTest10_c = func10_1(obj1);
    assert(fabs(resultTest10_c - TESTCLASS1_C) <= EPS);
    cout << "Test #10 is successfull. Result: a = " << resultTest10_a << ", b = "<< resultTest10_b << ", c = "<< resultTest10_c << endl;

    cout << "\nTest #11: Class Set-Methods" << endl;
    i = 12;
    void (*func11) (TestClass1*, const int&) = reinterpret_cast<void(*)(TestClass1*, const int&)>(getRedirectorAddressForName(context.names[i], context));
    func11(obj1, TESTCLASS1_A);
    i = 13;
    func11 = reinterpret_cast<void(*)(TestClass1*, const int&)>(getRedirectorAddressForName(context.names[i], context));
    func11(obj1, TESTCLASS1_B);
    i = 14;
    void (*func11_2) (TestClass1*, const double&) = reinterpret_cast<void(*)(TestClass1*, const double&)>(getRedirectorAddressForName(context.names[i], context));
    func11_2(obj1, (double)TESTCLASS1_C);
    cout << "Test #11 is successfull." << endl;

    cout << "\nTest #12: Class static members" << endl;
    i = 15;
    double(*func12)(float, int) = (double(*)(float, int))getRedirectorAddressForName(context.names[i], context);
    double resultFn12 = (*func12)(TESTCLASS1_STATIC_ARG0, TESTCLASS1_STATIC_ARG1);
    assert(fabs(resultFn12 - TESTCLASS1_STATIC_RESULT) <= EPS);
    cout << "Test #12 is successfull. Result = " << resultFn12 << endl;

    return 0;
}
