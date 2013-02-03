all_with_console_output: enable_console_output libelfperf glibc_patch

all_without_console_output: disable_console_output libelfperf glibc_patch

disable_console_output: 
	cp src/libelfperf/config.h_template src/libelfperf/config.h
	sed -i "s/Parameters/#define NO_CONSOLE_OUTPUT_LD_SO/g" src/libelfperf/config.h

enable_console_output:
	cp src/libelfperf/config.h_template src/libelfperf/config.h
	sed -i "s/Parameters/#define YES_CONSOLE_OUTPUT_LD_SO/g" src/libelfperf/config.h

set_arch_32:
	sed -i "s/Architecture/#define ELFPERF_ARCH_32/g" src/libelfperf/config.h

set_arch_64:
	sed -i "s/Architecture/#define ELFPERF_ARCH_64/g" src/libelfperf/config.h

libelfperf:
	make -C src/libelfperf/ lib 

libelfperf_x64:
	make -C src/libelfperf_x64/ lib 

glibc_patch:
	cd build/ && ./build_glibc.sh && make -j5 install
	-build/build_simlinks.sh	
