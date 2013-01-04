#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../libelfperf.so ELFPERF_ENABLE=true ELFPERF_PROFILE_FUNCTION=fclose /tools/lib/ld-2.11.3.so /bin/ls'

if [ $? -eq 0 ] 
then
	echo "Test successed"
else
	echo "Test failed"
fi
