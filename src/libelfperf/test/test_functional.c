#include <pthread.h>
#include <stdio.h>
#include "func.h"

#define THREADS_COUNT 7
#define LAUNCHES_NUMBER 100

void* threadFunction(void* args)
{
    int i = *((int*) args); 

    printf("%d: ", i);

    switch (i) {
        case 0 :
            testFunction1();
            break;

        case 1 :
            testFunction2(TEST2_ARG0, TEST2_ARG1);
            break;

        case 2 :
            testFunction3();
            break;

        case 3 :
            testFunction4(TEST4_ARG0, TEST4_ARG1);
            break; 

        case 4 :
            testFunction5(TEST5_ARG0);
            break;

        case 5 :
            testFunction6();
            break;

        case 6 :
            testFunction8(TEST8_ARG0, TEST8_ARG1, TEST8_ARG2, TEST8_ARG3, TEST8_ARG4, TEST8_ARG5, TEST8_ARG6, TEST8_ARG7, TEST8_ARG8, TEST8_ARG9);
            break;
    }

    return NULL;
}



int main()
{
    pthread_t threads[THREADS_COUNT];
    int numbers[THREADS_COUNT];
    int i, j;

    for (i = 0; i < LAUNCHES_NUMBER; i++) {
        for (j = 0; j < THREADS_COUNT; j++) {
            numbers[j] = j;
            pthread_create(&threads[j], NULL, &threadFunction, (void*)&numbers[j]);

        }
        for (j = 0; j < THREADS_COUNT; j++) {
            pthread_join(threads[j], NULL);
        }
    }

    printf("PID = %d\n", getpid());

    return 0;
}
