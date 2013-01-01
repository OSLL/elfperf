all: clean lib test

lib:
	gcc -m32 -g3 testlib.c -shared -o libtest.so

test:
	gcc -m32 -g3 test.c -ldl -lpthread -o test -lrt

clean:
	rm -f ./libtest.so ./test
