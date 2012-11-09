all: bin cdecl_wrapper

bin:
	gcc -m32 -o sample main.c wrappers.c


cdecl_wrapper:
	gcc -m32 -g3 -O0 -o cdecl_wrapp cdecl_wrapper_test.c cdecl_wrapper.c

clean:
	rm -f sample cdecl_wrapp
