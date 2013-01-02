#!/bin/bash


bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so ELFPERF_ENABLE=true ELFPERF_PROFILE_FUNCTION=hello:hella:testFunc /tools/lib/ld-2.11.3.so ./linked_exe'
