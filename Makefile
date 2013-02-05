all_with_console_output_32: enable_console_output libelfperf_32 glibc_patch

all_without_console_output_32: disable_console_output libelfperf_32 glibc_patch

all_with_console_output_64: enable_console_output libelfperf_64 glibc_patch

all_without_console_output_64: disable_console_output libelfperf_64 glibc_patch

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


libelfperf_32: set_arch_32
	make -C src/libelfperf/ lib_32

libelfperf_64: set_arch_64
	make -C src/libelfperf/ lib_64 

glibc_patch:
	cd build/ && ./build_glibc.sh && make -j5 install
	-build/build_simlinks.sh	
