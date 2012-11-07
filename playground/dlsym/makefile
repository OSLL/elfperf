all: clean lib exe

lib: lib.c
	gcc -m32 -g3 lib.c -shared -o libhello.so

exe: main.c
	gcc -m32 -g3 main.c -ldl -lpthread -o exe

deploy:
	cp ../build/elf/ld.so /tools/profiler.so

clean:
	-rm libhello.so
	-rm exe	
