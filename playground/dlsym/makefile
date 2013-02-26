all: clean lib exe

linked_exe: lib
	gcc -m32 -g3 main_linked_exe.c  -o linked_exe -L. -lhello 

big_test: lib
	gcc -m64 -g3 main_big_test.c -o big_test -L. -lhello 	   

lib: lib.c
	gcc -m32 -fPIC -g3 lib.c -shared -o libhello.so

exe: main.c
	gcc -m64 -g3 main.c -ldl -lpthread -o exe

deploy:
	cp ../build/elf/ld.so /tools/profiler.so

clean:
	-rm libhello.so
	-rm exe
	-rm linked_exe
	-rm big_test
