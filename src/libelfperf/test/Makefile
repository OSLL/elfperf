all: clean libelfperf bin

bin: test_redir_cdecl

libelfperf:
	make -C .. lib_64

test_redir_cdecl:
	gcc -m64 -g3 -O0 -Wall test_redir.c func.c -o test_redir -L.. -lelfperf

test_redir_no_cdecl:
	gcc -DLIBELFPERF -m64 -g3 -Wall -O0 -fomit-frame-pointer test_redir.c func.c -o test_redir -L.. -lelfperf

run:
	bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:.. ELFPERF_PROFILE_FUNCTION=testFunction1:testFunction2:testFunction3:testFunction4:testFunction5:testFunction6:testFunction7:testFunction8  ./test_redir'

clean:
	rm -f ./test_redir
	rm -f ../libelfperf.so
