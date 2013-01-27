lib: libelfperf.c
	gcc -DLIBELFPERF -m32 -g3  libelfperf.c ../timers/rdtsc_cntrs.c ../timers/global_stats.c -shared -o libelfperf.so
clean:
	rm -rf *.o *.so
