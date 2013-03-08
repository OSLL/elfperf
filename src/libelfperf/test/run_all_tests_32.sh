#!/bin/bash 

tests=`ls test*.sh | grep -v "32" | grep -v "64"`

for i in $tests
do
	echo "Performing $i"
	./$i
done

echo "=== x86 platform dependent tests ==="
x86_tests=`ls test*32.sh`

for i in $x86_tests
do
	echo "Performing $i"
	./$i
done


