default: enable_console_output lib_64

disable_console_output: 
	cp ./config.h_template ./config.h
	sed -i "s/Parameters/#define NO_CONSOLE_OUTPUT_LD_SO/g" ./config.h

enable_console_output:
	cp ./config.h_template ./config.h
	sed -i "s/Parameters/#define YES_CONSOLE_OUTPUT_LD_SO/g" ./config.h

set_arch_32:
	sed -i "s/Architecture/#define ELFPERF_ARCH_32/g" ./config.h

set_arch_64:
	sed -i "s/Architecture/#define ELFPERF_ARCH_64/g" ./config.h

lib_32: set_arch_32
	gcc -DLIBELFPERF -m32 -g3 -O0 libelfperf.c rdtsc_cntrs.c global_stats.c -shared -o libelfperf.so

lib_64: set_arch_64
	gcc -DLIBELFPERF -m64 -fPIC -g3 -O0 libelfperf.c rdtsc_cntrs.c global_stats.c -shared -o libelfperf.so

clean:
	rm -rf *.o *.so
