#include <stdio.h>
#include <stdlib.h>

void emptyFunction()
{
}

void fastFunction()
{
//	printf("FN_LOG: fastFunction()\n");
    int i; 
    for (i = 0; i < 1000; i++);
}

void slowFunction()
{
//	printf("FN_LOG: fastFunction()\n");
    int i; 
    for (i = 0; i < 100000; i++);
}

