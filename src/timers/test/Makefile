all: bin

bin:
	gcc ../hpet_cntrs.c ../rdtsc_cntrs.c test.c ../../wrappers/cdecl_wrapper.c ../../wrappers/cdecl_wrapper.h  -o test -m32 -lrt

clean:
	rm -f ./test

run:
	./test
