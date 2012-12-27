lib: libelfperf.c
	./generate_storage.sh 8192
	gcc -m32 -g3  libelfperf.c storage.c -shared -o libelfperf.so
clean:
	rm -rf *.o *.so
