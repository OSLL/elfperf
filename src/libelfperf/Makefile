lib: libelfperf.c
	./generate_storage.sh 4096
	gcc -m32 -g3  storage.c libelfperf.c ../timers/rdtsc_cntrs.c ../timers/global_stats.c -shared -o libelfperf.so
clean:
	rm -rf *.o *.so
