#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <dlfcn.h>
#include "../../common/tools.h"
#define LIBTEST_LIB_PATH "libs/libtest_lib.so"


int (*testFunction1) (int, int);


int main(){
  print_base_address();

	void * handle = dlopen(LIBTEST_LIB_PATH, RTLD_LAZY); 
	



	testFunction1 = dlsym(handle, "testFunction1");

	printf("\ntestFunction1 = %p\n", testFunction1);	

	printf("Calling testFunction1(): %d\n", testFunction1(1,1));

	print_dump(testFunction1);

	dlclose(handle);	
	return 0;
}
