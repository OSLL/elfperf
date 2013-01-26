all_with_console_output: enable_console_output libelfperf glibc_patch

all_without_console_output: disable_console_output libelfperf glibc_patch

disable_console_output: 
	cp src/libelfperf/config.h_template src/libelfperf/config.h
	sed -i "s/Parameters/#define NO_CONSOLE_OUTPUT_LD_SO/g" src/libelfperf/config.h

enable_console_output:
	cp src/libelfperf/config.h_template src/libelfperf/config.h
	sed -i "s/Parameters/#define YES_CONSOLE_OUTPUT_LD_SO/g" src/libelfperf/config.h

libelfperf:
	make -C src/libelfperf/ lib 

glibc_patch:
	cd build/ && ./build_glibc.sh && make -j5 install
	-build/build_simlinks.sh	
