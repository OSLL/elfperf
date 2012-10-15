#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <string.h>
#include <dlfcn.h>
#include "../../common/tools.h"

char SIGN[]= {0x83, 0xEC, 0x3C, 0x89, 0x5C, 0x24, 0x2C, 0x8B, 0x54, 0x24, 0x40, 0xE8};
void* (*real_dlsym)(void*,const char*);



static void redirector()
{
  printf("calling redirector\n");
}


void *getAddress(void* handle,const char *name)
{
	
}


void init_dlsym() {
  int i;
  char *base = (char*)dlopen(0,RTLD_LAZY);
 

	while(base < (char*)init_dlsym)
	{
	  i = 0;
    base = memchr(base,SIGN[0],(unsigned char*)init_dlsym-(unsigned char *)base);
		print_dump(base);
		while(SIGN[i]==base[i] && i<sizeof(SIGN))
		{
			i++;
		}
		if(i==sizeof(SIGN)-1)
		{
			printf("found dlsym at %p\n",base);
			real_dlsym = (void* (*)(void*, const char*))base;
			return;
		}
	}
	printf("dlsym not found\n");
}

void *dlsym (void *__restrict _handle, __const char *__restrict _name)
{
  static int initialized = 0;

	if(!initialized)
	{
		init_dlsym();
		initialized = 1;
	}


  printf("dlsym\n");

  


   return NULL;//lsym(_handle,_name);
}

