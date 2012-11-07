#include <stdio.h>
#include <dlfcn.h>
#include <time.h>
#include <pthread.h> 

void * p;
void * f;

#define REPEATS_NUMBER 10

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


int main()
{
	printf("Sizeof(time_t) = %d, sizeof(long int) = %d, sizeof(unsigned long long int) = %d \n", 
		sizeof(time_t), sizeof(long int), sizeof(unsigned long long int) );
	

	int i=0;
	p = dlopen("./libhello.so",RTLD_LAZY);
	printf("handle=%p\n",p);

	f = dlsym(p,"hello");
	printf("f=%p\n",f);
	
	pthread_t thread_0, thread_1, thread_2;
	int num0=0, num1=1, num2=2;

	// Create 3 threads
	pthread_create(&thread_0, NULL, threadFunction, &num0);
	pthread_create(&thread_1, NULL, threadFunction, &num1);
	pthread_create(&thread_2, NULL, threadFunction, &num2);
	
	// Waiting for all threads exiting
	pthread_join(thread_0, NULL);
	pthread_join(thread_1, NULL);
	pthread_join(thread_2, NULL);

	printf("Valid number of calls should be 3x%d = %d\n", REPEATS_NUMBER, 3*REPEATS_NUMBER);

	dlclose(p);
	return 0;
}
