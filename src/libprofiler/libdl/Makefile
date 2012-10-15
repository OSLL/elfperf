
find: clean find.c
	gcc find.c -g3 -L. -static -ldl -Wl,--wrap=dlsym -shared  -o find.so

clean: 
	-rm libfakedl.so*
	-rm find
	-rm *.o

