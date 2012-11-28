#!/bin/bash

# setting : separated list of profiled functions to env variable
export ELFPERF_PROFILE_FUNCTION=hello
# running
bash -c 'LD_LIBRARY_PATH=/tools/lib ./exe'
