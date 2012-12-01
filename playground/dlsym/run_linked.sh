#!/bin/bash

export ELFPERF_ENABLE=true

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_DEBUG=all LD_DEBUG_OUTPUT=1 /tools/lib/ld-2.11.3.so ./linked_exe'
