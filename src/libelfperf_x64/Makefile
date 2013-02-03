lib: libelfperf.c
	gcc -DLIBELFPERF -m64 -fPIC -g3 -O0 libelfperf.c ../timers/rdtsc_cntrs.c ../timers/global_stats.c -shared -o libelfperf.so
clean:
	rm -rf *.o *.so
