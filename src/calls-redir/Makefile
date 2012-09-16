all: lib bin

lib: 
	gcc -g3 -m64 -fPIC -shared -o libcall_redir_sample.so libcall_redir_sample.c

bin: 
	gcc -g3 -m64 -L$(PWD) -o main main.c elf_hook.h -lcall_redir_sample -ldl

clean: 
	rm -f *.so main

run: 
	export LD_LIBRARY_PATH=$(PWD):$(LD_LIBRARY_PATH); \
	./main
