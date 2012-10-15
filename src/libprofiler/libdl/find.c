#include <stdio.h>
//#include <dlfcn.h>
//#include "../common/tools.h"


#define DLOPEN_IMAGE  "../test/libs/libtest_lib.so"

int (*testFunction1)(int arg1, int arg2);

void * dlsym(void * handle, char* name)
{
		puts("__wrap_dlsym");
		return dvsym(handle, name);
}

/*void * dlsym(void * handle, char* name)
{
	printf("hello from dlsym\n");
}*/



#if 0
int main()
{

	void *h = dlopen(DLOPEN_IMAGE,1);

  printf("h=%p\n",h);

	void *ptr = dlsym(h,"testFunction1");

	testFunction1 = (int (*)(int,int))ptr;

	printf("result=%d\n",testFunction1(2,3));

//  print_base_address();

	printf("handle=%p dlsym=%p main=%p\n",h, ptr, main);
	
//	print_dump(ptr);
	
	return 0;
}

#endif
