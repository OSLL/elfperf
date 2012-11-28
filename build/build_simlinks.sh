#!/bin/bash

## Fix existing simlinks and create nessesary ones
# Do after make intsall

rm /tools/lib/libdl.so 
ln -s /tools/lib/libdl-2.11.3.so /tools/lib/libdl.so
ln -s /tools/lib/libdl-2.11.3.so /tools/lib/libdl.so.2
ln -s /tools/lib/libc-2.11.3.so /tools/lib/libc.so.6

