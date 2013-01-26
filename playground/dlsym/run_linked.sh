#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so ELFPERF_ENABLE=true ELFPERF_PROFILE_FUNCTION=hello:hella:testFunc:fclose:printf:sprintf:aaa:bbb:ccc:hello1:hello2:hello3:hello4:hello5 /tools/lib/ld-2.11.3.so ./linked_exe'
