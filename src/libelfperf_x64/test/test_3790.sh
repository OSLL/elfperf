#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so ELFPERF_ENABLE=true ELFPERF_PROFILE_FUNCTION=paaa:ddd /tools/lib/ld-2.11.3.so /bin/ls' 2>/dev/null 1>/dev/null

if [ $? -eq 0 ] 
then
	echo "Test successed"
else
	echo "Test failed"
fi
