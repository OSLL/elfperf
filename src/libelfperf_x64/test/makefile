all: clean libelfperf bin

bin: test

libelfperf:
	make -C ..

test:
	gcc -m64 -g3 -O0 -Wall test_wrapper_x64.c functions.c -o test_wrapper_x64 -L.. -lelfperf

test_no_frame:
	gcc -DLIBELFPERF -m64 -g3 -Wall -O0 -fomit-frame-pointer test_wrapper_x64.c functions.c -o test_wrapper_x64 -L.. -lelfperf

run:
	bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:.. ./test_wrapper_x64'

clean:
	rm -f ./test_wrapper_x64
	rm -f ../libelfperf.so
