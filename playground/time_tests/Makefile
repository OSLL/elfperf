all: clean lib test rdtsc_test

lib:
	gcc -m32 -g3 testlib.c -shared -o libtest.so

test:
	gcc -m32 -g3 test.c -ldl -lpthread -o test -lrt

rdtsc_test:
	gcc -m32 -g3 rdtsc_test.c -o rdtsc_test -lrt

clean:
	rm -f ./libtest.so ./test ./rdtsc_test
