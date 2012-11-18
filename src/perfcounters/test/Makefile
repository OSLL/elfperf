all: bin

bin:
	gcc ../time.c test.c ../../stack_sample/cdecl_wrapper.c ../../stack_sample/cdecl_wrapper.h  -o test -m32 -lrt -I../../stack_sample/

clean:
	rm -f ./test

run:
	./test
