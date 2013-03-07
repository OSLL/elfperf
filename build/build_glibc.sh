#!/bin/sh

mkdir -p manual
touch manual/stamp.{o,op,os,OS}
`dirname "$0"`/../eglibc-2.11.3/configure --prefix=/tools/ 
make
make   install

