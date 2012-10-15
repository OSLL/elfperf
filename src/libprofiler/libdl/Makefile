
find: clean find.c
	gcc -m32 -fPIC find.c -g3 -L. -static -lc -ldl  -shared  -o libfakedl.so #-Wl,--version-script=libfakedl.version
clean: 
	-rm libfakedl.so*
	-rm find
	-rm *.o

prepare_libdl.a:
	cp /usr/lib32/libdl.a .
	objcopy --redefine-sym dlsym=dvsym ./libdl.a
