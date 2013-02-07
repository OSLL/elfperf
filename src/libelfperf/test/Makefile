default: clean test_redir_cdecl_64

all: clean libelfperf bin

bin: test_redir_cdecl

libelfperf_32:
	make -C .. disable_console_output
	make -C .. lib_32

libelfperf_64:
	make -C .. disable_console_output
	make -C .. lib_64

test_redir_cdecl_32: libelfperf_32
	gcc -m32 -g3 -O0 -Wall test_redir.c func.c -o test_redir -L.. -lelfperf

test_redir_no_cdecl_32: libelfperf_32
	gcc -m32 -g3 -Wall -O0 -fomit-frame-pointer test_redir.c func.c -o test_redir -L.. -lelfperf


test_redir_cdecl_64: libelfperf_64
	gcc -m64 -g3 -O0 -Wall test_redir.c func.c -o test_redir -L.. -lelfperf

test_redir_no_cdecl_64: libelfperf_64
	gcc -m64 -g3 -Wall -O0 -fomit-frame-pointer test_redir.c func.c -o test_redir -L.. -lelfperf

run:
	bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:.. ELFPERF_PROFILE_FUNCTION=testFunction1:testFunction2:testFunction3:testFunction4:testFunction5:testFunction6:testFunction7:testFunction8  ./test_redir'

test_all:
	make test_redir_cdecl_32
	make run
	make clean
	make test_redir_no_cdecl_32
	make run
	make clean
	make test_redir_cdecl_64
	make run
	make clean
	make test_redir_no_cdecl_64
	make run
	make clean

clean:
	rm -f ./test_redir
	rm -f ../libelfperf.so
