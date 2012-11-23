all: bin 

bin: many_redirs_test cdecl_wrapper asm_shift_test 

cdecl_wrapper:
	gcc -m32 -g3 -O0 -o cdecl_wrapp test_cdecl_wrapper.c ../cdecl_wrapper.c ../../timers/hpet_cntrs.c ../../timers/global_stats.c ../wrapper_redirector.c -I. -lrt

asm_shift_test:
	gcc -m32 -g3 -O0 -o asm_shift_test test_shift.c

many_redirs_test:
	gcc -m32 -o many_redirs_test test_many_redirs.c ../cdecl_wrapper.c ../cdecl_wrapper.h ../../timers/hpet_cntrs.c ../../timers/global_stats.c ../wrapper_redirector.c -I. -lrt

clean:
	rm -f cdecl_wrapp asm_shift_test many_redirs_test
