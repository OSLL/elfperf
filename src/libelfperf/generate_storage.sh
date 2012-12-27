#!/bin/bash

# This script creates source storage.c
# with function void storage() which will be about $1 bytes size


##### Usage 
# ./generate_storage size

if [ -n "$1" ]
then
	filename="storage.c";
	template1="void storage(){";
	template2="asm volatile(";
	template3=");";
	template4="}";

	echo $template1 > $filename;
	echo $template2 >> $filename;

	yes "	\"nop\\n\"" | head -n $1 >> $filename;
	echo $template3 >> $filename;
	echo $template4 >> $filename;

else
	echo "Not enaugh arguments, usage\n ./generate_storage size"
fi
