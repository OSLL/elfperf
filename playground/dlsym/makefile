all: clean lib exe

lib: lib.c
	gcc -g3 lib.c -shared -o libhello.so

exe: main.c
	gcc -g3 main.c -ldl -o exe

deploy:
	cp ../build/elf/ld.so /tools/profiler.so

clean:
	-rm libhello.so
	-rm exe	
