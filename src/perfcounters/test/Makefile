all: bin

bin:
	gcc ../time.c ../rdtsc_cntrs.c test.c ../../stack_sample/cdecl_wrapper.c ../../stack_sample/cdecl_wrapper.h  -o test -m32 -lrt

clean:
	rm -f ./test

run:
	./test
