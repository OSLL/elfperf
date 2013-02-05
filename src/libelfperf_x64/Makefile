lib: libelfperf.c
	gcc -DLIBELFPERF -m64 -fPIC -g3 -O0  rdtsc_cntrs.c global_stats.c libelfperf.c -shared -o libelfperf.so
clean:
	rm -rf *.o *.so
