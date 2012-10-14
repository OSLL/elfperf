#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <dlfcn.h>
#define LIBTEST_LIB_PATH "libs/libtest_lib.so"


int (*testFunction1) (int, int);
int (*testFunction2)(void);

void (*testFunction3)(int, int);
void (*testFunction4)(void);


int main(){
	void * handle = dlopen(LIBTEST_LIB_PATH, RTLD_LAZY); 
	
/*	void * callerPointer = __builtin_extract_return_addr(__builtin_frame_address(0)); 
	unsigned int caller = &callerPointer;
	printf("Caller_main = %x\n",  callerPointer );
	printf("main = %x\n", &main);


           int j, nptrs;
       #define SIZE 100
           void *buffer[100];
           char **strings;

           nptrs = backtrace(buffer, SIZE);
           printf("backtrace() returned %d addresses\n", nptrs);

  //          The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
//              would produce similar output to the following: 

           strings = backtrace_symbols(buffer, nptrs);
           if (strings == NULL) {
               perror("backtrace_symbols");
               exit(EXIT_FAILURE);
           }

           for (j = 0; j < nptrs; j++)
               printf("%s\n", strings[j]);

           free(strings);*/



	testFunction1 = dlsym(handle, "testFunction1");
	//testFunction2 = dlsym(handle, "testFunction2");
	//testFunction3 = dlsym(handle, "testFunction3");
	//testFunction4 = dlsym(handle, "testFunction4");
	printf("\ntestFunction1 = %p\n", testFunction1);	

	printf("Calling testFunction1(): %d\n", (*testFunction1)(1,1));
//	printf("Calling testFunction2(): %d\n", (*testFunction2)());

//	printf("Calling testFunction3()\n");
//	(*testFunction3)(1,1);

//	printf("Calling testFunction4()\n");
//	(*testFunction4)();

	dlclose(handle);	
	return 0;
}
