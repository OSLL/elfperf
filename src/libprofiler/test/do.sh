#!/bin/sh


make clean
make

export LD_PRELOAD=" ../libdl/libfakedl.so "
#export LD_LIBRARY_PATH="../libdl/"

./test_dynamic_load_app
