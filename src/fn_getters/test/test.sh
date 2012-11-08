#!/bin/bash

export ELFPERF_PROFILE_FUNCTION=read:write:free:malloc:calloc:printf:scanf;

./test -e ELFPERF_PROFILE_FUNCTION;
