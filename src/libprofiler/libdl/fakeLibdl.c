#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <string.h>
//#include "origLibdl.h"
//#include "true_libdl.h"

void* getReturnAddress(){ 
	void * returnAddr = NULL;
        int j, nptrs;
        #define SIZE 100
        void *buffer[100];
        char **strings;

        nptrs = backtrace(buffer, SIZE);
        //printf("backtrace() returned %d addresses\n", nptrs);

       //          The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
       //              would produce similar output to the following: 

        strings = backtrace_symbols(buffer, nptrs);
        if (strings == NULL) {
            perror("backtrace_symbols");
	    return returnAddr; 
        }

	char * addr;

	printf("%s\n", strings[2]);
	addr =  strchr ( strings[2], '[' ); 	
	unsigned int addr_i;
	sscanf(addr,"[0x%x]",&addr_i);

	printf("addr = \"%s\", return addr = %#x, points to = %p\n", addr, addr_i, (void*)addr_i);
	//returnAddr = (void*)0x804873a;
	returnAddr = (void*)addr_i;
	printf("pointer = %p\n", returnAddr);
	
        free(strings);
	return returnAddr;
}

void *dlsym (void *__restrict _handle,
                    __const char *__restrict _name)
{
      	printf("Hello from fake dlsym(%p,%s)",_handle,_name);
//	void * callerPointer = __builtin_return_address (0); 
//	unsigned int caller = &callerPointer;
//	printf("Caller = %x, this = %x\n", caller, &dlsym );
	void * caller = getReturnAddress();
	void * result = NULL; 
	asm volatile(
		"movl %1,%%eax;"
		"movl %2,%%edx;"
		"movl %3,%%ecx;"
		"call _dl_sym;"
		: "=r"(result)
		: "r"(_handle),"r"(_name),"r"(caller)
		: "eax", "edx", "ecx"
	);
	printf("after _dl_sym call = %p", result);
	//void * result = (void *)_dl_sym(__handle, __name, caller);
//	void * result = (void *)_dl_sym(_handle, _name, caller);
	return result;
}

