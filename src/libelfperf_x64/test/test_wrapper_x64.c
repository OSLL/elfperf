#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../ld-routines.h"
#include "../libelfperf.h"
#include "sys/mman.h"
#include <assert.h>


#define ARG0 1
#define ARG1 2
#define FUNCTION_3_RESULT 4
#define FUNCTION_4_RESULT 3


#define FUNCTION_5_RESULT 5.5
#define FUNCTION_6_RESULT 6.6

#define STRUCT_A_VALUE 1
#define STRUCT_B_VALUE 2
#define STRUCT_C_VALUE 3

typedef void (func)();

// Test structure, with size > 8byte
struct testStruct{
    int a;
    int b;
    int c;
};

// Test functions
void testFunction1() {
    printf("in testFunction1\n");
}

void testFunction2(int arg0, int arg1) {
    printf("in testFunction2( %d, %d)\n", arg0, arg1);
}

int testFunction3() {
    printf("in testFunction3\n");
    return FUNCTION_3_RESULT;
}

int testFunction4(int arg0, int arg1) {
    printf("in testFunction4(%d, %d)\n", arg0, arg1);
    return arg0 + arg1;
}

float testFunction5() {
    printf("in testFunction5()\n");
    return FUNCTION_5_RESULT;
}

double testFunction6() {
    printf("in testFunction6()\n");
    return (double)FUNCTION_6_RESULT;
}

struct testStruct testFunction7(){
//	void * addr;
//	asm("movl 0x8(%%ebp),%0":"=r"(addr));
//	printf("in testFunction7(hidden_parameter = %x)\n",addr);

    printf("in testFunction7()\n");

    struct testStruct result;
    result.a = STRUCT_A_VALUE;
    result.b = STRUCT_B_VALUE;
    result.c = STRUCT_C_VALUE;

    return result;
}

char * testFuncNames[1]= {"testFunction1"};//,"testFunction2","testFunction3","testFunction4",
//    "testFunction5","testFunction6","testFunction7"};

void * funcPointers[7] = {testFunction1, testFunction2, testFunction3, testFunction4, 
    testFunction5, testFunction6,testFunction7};

//#define REDIRECTOR_WORDS_SIZE 4
//#define PAGESIZE 4096

int main()
{
    int intResult = 0;
    float floatResult=0.;
    double doubleResult=0.;

    struct testStruct * structResult=NULL;

    int count = 7;
    size_t allocSize = sizeof(void*) * REDIRECTOR_WORDS_SIZE * count + PAGESIZE - 1;
    size_t i = 0;
    void* redirectors = (void *)malloc(allocSize);

    printf("Before aligment %x, %x\n", redirectors, sizeof(void*) * REDIRECTOR_WORDS_SIZE*count + PAGESIZE-1);
    redirectors = (void *)(((int) redirectors + PAGESIZE-1) & ~(PAGESIZE-1));
    int pagesNum = allocSize/PAGESIZE ;
    printf("Number of memory pages %x\n", pagesNum);
 
    for (i = 0; i < pagesNum; i++) {
        printf("Going to do mprotect\n");
        if (mprotect(redirectors + PAGESIZE*i, PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC)) {
            printf("Couldn't mprotect");
            return 0;
        }
    }

    printf("Zeroing\n");
    for (i = 0 ; i < sizeof(void*) * REDIRECTOR_WORDS_SIZE * count; i += REDIRECTOR_SIZE) {
        *((unsigned int*)(redirectors+i)) = 0;
    }

    printf("\n===== Start of test for wrappers and redirectors ======\n");

    printf("\nTest #1:\n");
    int functionIndex = 0;
    void* redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);
    writeRedirectionCode(redirectorAddress, testFunction1);
    void(*func1)() = redirectorAddress;
    (*func1)();

    printf("\nTest #2:\n");
    functionIndex = 1;
    redirectorAddress = redirectors + REDIRECTOR_WORDS_SIZE * functionIndex*sizeof(void*);   
    writeRedirectionCode(redirectorAddress, testFunction2);    
    void(*func2)(int, int) = redirectorAddress;
    (*func2)(128, 256);

    return 0;
}
