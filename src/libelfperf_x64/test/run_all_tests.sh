#!/bin/bash 

tests=`ls test*.sh`

for i in $tests
do
	echo "Performing $i"
	./$i
done

