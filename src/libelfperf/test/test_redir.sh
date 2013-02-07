#!/bin/bash

bash -c 'make test_redir_cdecl_32 run clean' 2>/dev/null 1>/dev/null 

if [ $? -eq 0 ] 
then
	echo "[x32] Test of redirection for CDECL functions SUCCEEDED!"
else
	echo "[x32] Test of redirection for CDECL functions FAILED"
fi


bash -c 'make test_redir_no_cdecl_32 run' 2>/dev/null 1>/dev/null

if [ $? -eq 0 ] 
then
	echo "[x32] Test of redirection for NOT CDECL functions SUCCEEDED!"
else
	echo "[x32] Test of redirection for NOT CDECL functions FAILED"
fi


bash -c 'make test_redir_cdecl_64 run clean' 2>/dev/null 1>/dev/null 

if [ $? -eq 0 ] 
then
	echo "[x64] Test of redirection for CDECL functions SUCCEEDED!"
else
	echo "[x64] Test of redirection for CDECL functions FAILED!"
fi


bash -c 'make test_redir_no_cdecl_64 run clean' 2>/dev/null 1>/dev/null

if [ $? -eq 0 ] 
then
	echo "[x64] Test of redirection for NOT CDECL functions SUCCEEDED!"
else
	echo "[x64] Test of redirection for NOT CDECL functions FAILED"
fi
