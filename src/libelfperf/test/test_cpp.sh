#!/bin/bash

bash -c 'make test_cpp_cdecl_64 run_test_cpp clean' 2>/dev/null 1>/dev/null 

if [ $? -eq 0 ] 
then
	echo "[x32] Test of C++ programs for CDECL functions SUCCEEDED!"
else
	echo "[x32] Test of C++ programs for CDECL functions FAILED"
fi


bash -c 'make test_cpp_no_cdecl_32 run_test_cpp clean' 2>/dev/null 1>/dev/null

if [ $? -eq 0 ] 
then
	echo "[x32] Test of C++ programs for NOT CDECL functions SUCCEEDED!"
else
	echo "[x32] Test of C++ programs for NOT CDECL functions FAILED"
fi


bash -c 'make test_cpp_cdecl_64 run_test_cpp clean' 2>/dev/null 1>/dev/null 

if [ $? -eq 0 ] 
then
	echo "[x64] Test of C++ programs for CDECL functions SUCCEEDED!"
else
	echo "[x64] Test of C++ programs for CDECL functions FAILED"
fi


bash -c 'make test_cpp_no_cdecl_64 run_test_cpp clean' 2>/dev/null 1>/dev/null

if [ $? -eq 0 ] 
then
	echo "[x64] Test of C++ programs for NOT CDECL functions SUCCEEDED!"
else
	echo "[x64] Test of C++ programs for NOT CDECL functions FAILED"
fi
