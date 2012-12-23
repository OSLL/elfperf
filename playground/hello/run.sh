#!/bin/bash

# setting : enabling profiling
#export ELFPERF_ENABLE=true
# running
bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib LD_PRELOAD=../../src/libelfperf/libelfperf.so ELFPERF_ENABLE=true ELFPERF_PROFILE_FUNCTION=hello /tools/lib/ld-2.11.3.so ./main'
