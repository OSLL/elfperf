lib: libelfperf.c
	gcc -DLIBELFPERF -m64 -fPIC -g3 -O0  ../timers/rdtsc_cntrs.c ../timers/global_stats.c libelfperf.c -shared -o libelfperf.so
clean:
	rm -rf *.o *.so
