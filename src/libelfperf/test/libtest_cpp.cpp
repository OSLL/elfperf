#include "libtest_cpp.h"
#include <iostream>
#include <assert.h>
#include <math.h>
#include <stdio.h>
using namespace std;

TestClass1::TestClass1(const int& a, const int& b, const double& c)
    : m_a(a), m_b(b), m_c(c)
{   
}
    
void TestClass1::setA(const int& a)
{
    m_a = a;
}

void TestClass1::setB(const int& b)
{
    m_b = b;
}

void TestClass1::setC(const double& c)
{
    m_c = c;
}

int TestClass1::getA()
{
    return m_a;
}

int TestClass1::getB()
{
    return m_b;
}

double TestClass1::getC()
{
    return m_c;
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
