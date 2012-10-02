#include <dlfcn.h>

void * _dlsym(void *__restrict __handle, __const char *__restrict __name){
	return dlsym(__handle,__name);
}
