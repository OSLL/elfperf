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
 * \file wrappers.c
 * \brief Implementation of functions for wrapping any other functions
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "wrappers.h"
#include <stdarg.h>
#include <stdio.h>

int wrap(void* fn, int count, ...)
{
    printf("Wrapper: Calling of function with %d params.\n", count);

    int i;
    asm("movl %ebp, %edx");
    asm("addl $16, %edx");
    for (i = 0; i < count; i++) {
        asm("addl $4, %edx");
    }

    for (i = 0; i < count; i++) {
        asm("pushl -4(%edx)");
        asm("subl $4, %edx");
        //asm("pushl %0" : :"r"(i) : );
    }

    asm("call %0" : :"r"(fn) : );

    for (i = 0; i < count; i++) {
        asm("popl %edx");
    }

    int result;
    asm("movl %%eax, %0" :"=r"(result): : );
    return result;
}
