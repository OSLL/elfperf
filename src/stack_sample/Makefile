all: bin cdecl_wrapper asm_shift_test

bin:
	gcc -m32 -o sample main.c wrappers.c


cdecl_wrapper:
	gcc -m32 -g3 -O0 -o cdecl_wrapp cdecl_wrapper_test.c cdecl_wrapper.c cdecl_wrapper.h ../perfcounters/time.h ../perfcounters/time.c -I. -lrt

asm_shift_test:
	gcc -m32 -g3 -O0 -o asm_shift_test test_shift.c

clean:
	rm -f sample cdecl_wrapp asm_shift_test
