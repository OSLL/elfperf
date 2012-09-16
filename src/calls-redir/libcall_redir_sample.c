#include "libcall_redir_sample.h"


// Library function
void testCallRedirSample(){
	char * a =(char*)malloc(10) ;
	puts("Hello from lib");
	free(a);
}

