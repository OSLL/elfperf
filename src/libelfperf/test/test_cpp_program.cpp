#include <iostream>
#include <assert.h>
#include <math.h>
using namespace std;


#include "libtest_cpp.h"

int main()
{
    float resultFn2 = testFunction12(TEST_FUNCTION_2_ARG0, TEST_FUNCTION_2_ARG1);
    cout << "Result of testFunction2 = " << resultFn2 << endl;
    assert(fabs(resultFn2 - TEST_FUNCTION_2_RESULT) <= EPS);

    return 0;
}
