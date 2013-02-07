#!/bin/sh

mkdir -p manual
touch manual/stamp.{o,op,os,OS}
../eglibc-2.11.3/configure --prefix=/tools/ 
make   install

