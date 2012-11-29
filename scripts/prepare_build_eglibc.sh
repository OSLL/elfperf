#!/bin/bash

# This script makes preparations for eglibc build

rm -rf build
mkdir -p build/manual
touch build/manual/stamp.{o,op,os,OS}
cd build 
../../eglibc-2.11.3/configure --enable-shared  --disable-sanity-checks

