all: clean bin 

bin: test_wrapper_x64

libelfperf:
	make -C ..

test_wrapper_x64:
	gcc -DLIBELFPERF -m64 -g3 -O0 -o test_wrapper_x64 test_wrapper_x64.c ../libelfperf.c ../../timers/rdtsc_cntrs.c ../../timers/global_stats.c

clean:
	rm -f test_wrapper_x64
