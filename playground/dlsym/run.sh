#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true ELFPERF_PROFILE_FUNCTION=hello:printf /tools/lib/ld-2.11.3.so ./exe'
