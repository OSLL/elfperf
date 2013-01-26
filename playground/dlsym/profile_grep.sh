#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=fclose:free:fwrite:printf:fputs_unlocked:fwrite_unlocked:xstat64:isatty \
/tools/lib/ld-2.11.3.so /bin/grep "ls" *'
