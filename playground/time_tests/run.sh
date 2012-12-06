#!/bin/bash

# setting : separated list of profiled functions to env variable
export ELFPERF_PROFILE_FUNCTION=emptyFunction:fastFunction:slowFunction
# running
bash -c 'LD_LIBRARY_PATH=/tools/lib ./test'
