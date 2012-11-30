#!/bin/bash

export ELFPERF_ENABLE=true

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. /tools/lib/ld-2.11.3.so ./linked_exe'
