#include <stdio.h>
#include  <dlfcn.h>
#define LIBTEST_LIB_PATH "libs/libtest_lib.so"


int (*testFunction1) (int, int);
int (*testFunction2)(void);

void (*testFunction3)(int, int);
void (*testFunction4)(void);


int main(){
	void * handle = dlopen(LIBTEST_LIB_PATH, RTLD_LAZY); 
	
	testFunction1 = dlsym(handle, "testFunction1");
	testFunction2 = dlsym(handle, "testFunction2");
	testFunction3 = dlsym(handle, "testFunction3");
	testFunction4 = dlsym(handle, "testFunction4");
	
	printf("Calling testFunction1(): %d\n", (*testFunction1)(1,1));
	printf("Calling testFunction2(): %d\n", (*testFunction2)());

	printf("Calling testFunction3()\n");
	(*testFunction3)(1,1);

	printf("Calling testFunction4()\n");
	(*testFunction4)();

	dlclose(handle);	
	return 0;
}
