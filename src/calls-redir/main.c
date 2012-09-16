#define _GNU_SOURCE
#include <link.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "elf_hook.h"
#include "libcall_redir_sample.h"

#define LIB_PATH "libcall_redir_sample.so"

// This variable will store addr of libcall_redir_sample.so in process memory
void * libaddr = NULL;



// Callback for dl_iterate_phdr
static int
callback(struct dl_phdr_info *info, size_t size, void *data)
{
	// Uncomment if debug need
/*	int j;

	printf("name=%s (%d segments)\n", info->dlpi_name,
        info->dlpi_phnum);

	for (j = 0; j < info->dlpi_phnum; j++)
        	printf("\t\t header %2d: address=%10p\n", j,
        		(void *) (info->dlpi_addr + info->dlpi_phdr[j].p_vaddr));*/


	// Check, is current lib libcall_redir_sample.so
	char * substr = strstr(info->dlpi_name, LIB_PATH);
	if (substr){
		// If it is - store its addres into libaddr
		printf("\nFound library, %s , addr = %10p\n", info->dlpi_name,  (void *)(info->dlpi_addr + info->dlpi_phdr[0].p_vaddr));
		libaddr = (void *)(info->dlpi_addr + info->dlpi_phdr[0].p_vaddr);		
	}

	return 0;
}

// This function will be called instead of malloc ( size_t size ) from lib
void * fakeMalloc ( size_t size ){
	void * allocatedMemory = malloc(size);
	printf("alloc called for %d bytes, allocated at addr = %10p\n", size, allocatedMemory);
	return allocatedMemory;
}

// This function will be called instead of free(void * ptr ) from lib
void fakeFree ( void * ptr ){
        printf("free called for memory block = %10p\n",ptr);
	free(ptr);
}


int main()
{
	void *original1, *original2;

	// Getting addres of our shared lib
	dl_iterate_phdr(callback, NULL);

	// Return if lib address not found

	if (!libaddr){
		putc("Library address not found!");
		return 1;
	}


	testCallRedirSample();
	puts("\n");

	// Setup redirect	
	original1 = elf_hook(LIB_PATH, libaddr, "malloc", fakeMalloc);
	original2 = elf_hook(LIB_PATH, libaddr, "free", fakeFree);

	testCallRedirSample();

	// Remove redirect
	original1 = elf_hook(LIB_PATH, libaddr, "malloc", original1);
	original2 = elf_hook(LIB_PATH, libaddr, "free", original2);

	puts ("\n");

	testCallRedirSample();

	return 0;
}
