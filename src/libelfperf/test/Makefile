default: clean test_cpp

all: clean libelfperf bin

bin: test_redir_cdecl

run: run_test_cpp

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

lib_cpp:
	g++ -m64 -fPIC -g3 libtest_cpp.cpp -shared -o libtest_cpp.so 

test_cpp_cdecl_32: libelfperf_32
	g++ -m32 -g3 test_cpp.cpp libtest_cpp.cpp func.c -o test_cpp -L.. -lelfperf

test_cpp_no_cdecl_32: libelfperf_32
	g++ -m32 -g3 -fomit-frame-pointer test_cpp.cpp libtest_cpp.cpp func.c -o test_cpp -L.. -lelfperf

test_cpp_cdecl_64: libelfperf_64
	g++ -m64 -g3 test_cpp.cpp libtest_cpp.cpp func.c -o test_cpp -L.. -lelfperf

test_cpp_no_cdecl_64: libelfperf_64
	g++ -m64 -g3 -fomit-frame-pointer test_cpp.cpp libtest_cpp.cpp func.c -o test_cpp -L.. -lelfperf

run_test_redir:
	bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:.. ELFPERF_PROFILE_FUNCTION=testFunction1:testFunction2:testFunction3:testFunction4:testFunction5:testFunction6:testFunction7:testFunction8:testFunction9 ./test_redir'

run_test_cpp:
	bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:/usr/lib64:.. ELFPERF_PROFILE_FUNCTION=_Z13testFunction1v:_Z13testFunction2ii:_Z13testFunction3v:_Z13testFunction4ii:_Z13testFunction5d:_Z13testFunction6v:_Z13testFunction710testSt:_Z13testFunction8iiiiiiii:_Z13testFunction911testSt:_ZN10TestClass14getAEv:_ZN10TestClass14getBEv:_ZN10TestClass14getCEv:_ZN10TestClass14setAERKi:ZN10TestClass14setBERKi:_ZN10TestClass14setCERKd:_ZN10TestClass19function1  ./test_cpp'

test_cpp_program: lib_cpp
	g++ -m64 -g3 test_cpp_program.cpp -o test_cpp_program -L. -ltest_cpp

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
	rm -f ./test_cpp
	rm -f ./libtest_cpp.so
	rm -f ./elfperf_results_*
