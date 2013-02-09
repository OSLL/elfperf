#include <stdint.h>

#define EPS 0.000000001

// Test 1
// ...

// Test 2
#define TEST2_ARG0 256
#define TEST2_ARG1 -150 

// Test 3
#define TEST3_RESULT 17

// Test 4
#define TEST4_ARG0 117
#define TEST4_ARG1 1024 
#define TEST4_RESULT TEST4_ARG0 + TEST4_ARG1

// Test 5
#define TEST5_ARG0 0.00125
#define TEST5_RESULT 0.008

// Test 6
#define TEST6_RESULT 0.000512

// Test 7
#define TEST7_ARG0_A 25
#define TEST7_ARG0_B 0
#define TEST7_ARG0_C -75
#define TEST7_RESULT_A 12
#define TEST7_RESULT_B -100
#define TEST7_RESULT_C 12345678
#define TEST7_RESULT_D 0.481516
#define TEST7_RESULT_E 9999999

// Test 8
#define TEST8_RESULT 10000
#define TEST8_ARG0 -4
#define TEST8_ARG1 -3
#define TEST8_ARG2 -2
#define TEST8_ARG3 -1
#define TEST8_ARG4 -0
#define TEST8_ARG5 -1
#define TEST8_ARG6 -2
#define TEST8_ARG7 -3
#define TEST8_ARG8 -4
#define TEST8_ARG9 -5

// Test 9
#define TEST9_ARG0_A -25
#define TEST9_ARG0_B 25
#define TEST9_ARG0_C 9999
#define TEST9_ARG0_D 0.45
#define TEST9_ARG0_E 10000
#define TEST9_RESULT_A 1000
#define TEST9_RESULT_B -150
#define TEST9_RESULT_C 0


struct testStruct
{
    int a;
    int b;
    int c;
};

struct testStruct2
{
    int a;
    int b;
    uint64_t c;
    double d;
    uint64_t e;
};

void testFunction1();
void testFunction2(int arg0, int arg1);
int testFunction3();
int testFunction4(int arg0, int arg1);
float testFunction5(double arg0);
double testFunction6();
struct testStruct2 testFunction7(struct testStruct arg0);
int testFunction8(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9);
struct testStruct testFunction9(struct testStruct2 arg0);
