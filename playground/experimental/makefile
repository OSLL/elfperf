all: clean lib test

test: lib
	gcc -m32 -g3 main.c -o test -L. -lhello
    
lib_without_frame_pointer: lib.c
	gcc -m32 -g3 lib.c -shared -fomit-frame-pointer -o libhello.so

lib: lib.c
	gcc -m32 -g3 lib.c -shared -o libhello.so

clean:
	-rm libhello.so
	-rm test
