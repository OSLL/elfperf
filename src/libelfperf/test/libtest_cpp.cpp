#include "libtest_cpp.h"
#include <iostream>
#include <assert.h>
#include <math.h>
#include <stdio.h>
using namespace std;

TestClass1::TestClass1(const int& a, const int& b, const double& c)
    : m_a(a), m_b(b), m_c(c)
{   
    cout << "TEST_LOG: Inside of TestClass1::TestClass1(" << a << ", " << b << ", " << c << ")" << endl;
}
    
void TestClass1::setA(const int& a)
{
    cout << "TEST_LOG: Inside of TestClass1::setA(" << a << ")" << endl;
    assert(a == TESTCLASS1_A);
    m_a = a;
}

void TestClass1::setB(const int& b)
{
    cout << "TEST_LOG: Inside of TestClass1::setB(" << b << ")" << endl;
    assert(b == TESTCLASS1_B);
    m_b = b;
}

void TestClass1::setC(const double& c)
{
    cout << "TEST_LOG: Inside of TestClass1::setC(" << c << ")" << endl;
    assert(c == TESTCLASS1_C);
    m_c = c;
}

int TestClass1::getA()
{
    cout << "TEST_LOG: Inside of TestClass1::getA" << endl;
    return m_a;
}

int TestClass1::getB()
{
    cout << "TEST_LOG: Inside of TestClass1::getB" << endl;
    return m_b;
}

double TestClass1::getC()
{
    cout << "TEST_LOG: Inside of TestClass1::getC" << endl;
    return m_c;
}

double TestClass1::function1(float arg0, int arg1)
{
    cout << "TEST_LOG: Inside of TestClass1::function1(" << arg0 << ", " << arg1 << ")" << endl;
    assert(fabs(arg0 - TESTCLASS1_STATIC_ARG0) <= EPS);
    assert(arg1 == TESTCLASS1_STATIC_ARG1);

    return TESTCLASS1_STATIC_RESULT;
}


TestClass1::~TestClass1()
{
}

void testFunction11()
{
    cout << "Inside of testFunction1()" << endl;
}

float testFunction12(int arg0, double arg1)
{
    cout << "Inside of testFunction2(" << arg0 << ", " << arg1 << ")" << endl;

    assert(arg0 == TEST_FUNCTION_2_ARG0);
    //assert(fabs(arg1 - TEST_FUNCTION_2_ARG1) <= EPS);

    return TEST_FUNCTION_2_RESULT;
}
