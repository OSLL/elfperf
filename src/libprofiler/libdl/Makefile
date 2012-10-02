lib:
	gcc -fPIC -shared -o libdl1.so dlfcn.c true_libdl.c true_libdl.h /usr/lib/x86_64-linux-gnu/libdl.a -Wl,--version-script=libdl1.version 
clean: 
	rm libdl1.so*
