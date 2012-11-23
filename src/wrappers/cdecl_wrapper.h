/*
 * Copyright © 2012 OSLL osll@osll.spb.ru
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * The advertising clause requiring mention in adverts must never be included.
 */
/*! ---------------------------------------------------------------
 * \file cdecl_wrapper.h
 * \brief Declaration of wrapper for cdecl function
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#ifndef _CDECL_WRAPPER_H_
#define _CDECL_WRAPPER_H_
#include <time.h>
// Number of contexts will be allocated
#define CONTEXT_PREALLOCATED_NUMBER 100000

// FIXME doesnt support recoursive calls  because of global stack variables usage
struct WrappingContext {
	// real return address
	void * realReturnAddr; 		// 4bytes
	// content of -4(%%old_ebp)
	void * oldEbpLocVar; 		// 4bytes
	// function return value
	void * eax;			// 4bytes
	double doubleResult;		// 8bytes 
	void * functionPointer;		// 4bytes
	struct timespec startTime; 	// function starting time
	struct timespec endTime;	// function ending time
};


void setFunctionPointer(void * pointer);

void wrapper();

#endif
