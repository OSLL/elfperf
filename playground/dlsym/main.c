#include <stdio.h>
#include <dlfcn.h>
#include <time.h>
#include <pthread.h> 
#include <math.h>
#include <string.h>
#include <stdlib.h>

void *p, *f;

#define REPEATS_NUMBER 4
#define THREAD_COUNT 4 

void * threadFunction(int* threadNum){
	int i=0;
	for(i=0;i<REPEATS_NUMBER;i++)
	{
	   char buf[100];
	   sprintf(buf, "caller %d, thread %d", i , *threadNum);	
		((void (*)(const char*))f)(buf);

	}

	pthread_exit(0);
}




int main(int argc, char** argv)
{
	srand(time(NULL));	

	int i=0;
	p = dlopen("./libhello.so",RTLD_LAZY);
	printf("handle=%p\n",p);

	f = dlsym(p,"hello");
	printf("f=%p\n",f);
	
	pthread_t threads[THREAD_COUNT];
	int threadNumbers[THREAD_COUNT];

	// Creating threads
	for( i=0 ; i < THREAD_COUNT; i++ ){
		threadNumbers[i] = i;
		pthread_create(threads+i, NULL, threadFunction, threadNumbers+i);
	} 

	
	// Waiting for all threads exiting
	
	for( i=0 ; i < THREAD_COUNT; i++ ){
		pthread_join(threads[i], NULL);
	}

	printf("Valid number of calls should be %dx%d = %d\n", THREAD_COUNT, REPEATS_NUMBER, THREAD_COUNT*REPEATS_NUMBER);

	dlclose(p);
	printf("PID == %d \n", getpid() );
	return 0;
}
