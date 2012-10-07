#include <stdio.h>
//#include "origLibdl.h"
//#include "true_libdl.h"



/*extern void *dlopen (__const char *__file, int __mode) 
{
//	puts("Hello from fake libdl");
	system("touch /tmp/11212121");
//	return _dlopen(__file,__mode);
	return NULL;

}*/


void *dlsym (void *__restrict __handle,
                    __const char *__restrict __name)
{
      	puts("Hello from fake libdl");
	return d1sym(__handle, __name);
}

