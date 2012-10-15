#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>

static void print_base_address()
{
  void * base = dlopen(0, RTLD_LAZY);
	printf("base_address=%p\n",base);
}

static void print_dump(void *p)
{
	char *ptr=(char*)p;
	int i = 0;

	for(i=0;i<30;i++)
	{
		printf("%02X ",(unsigned char)(*ptr++));
	}
	printf("\n");
}
#endif

