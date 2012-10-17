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
 * \file main.c
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include <stdio.h>
#include "wrappers.h"

int foo1(int arg1)
{
    printf("Foo1: This is an example function with 2 params: %d.\n", arg1);
    return 1;
}

int foo2(int arg1, int arg2, int arg3, int arg4, int arg5)
{
    printf("Foo2: This is an example function with 2 params: %d, %d.\n", arg1, arg2);
    return 2;
}

int foo3(int arg1, int arg2, int arg3)
{
    printf("Foo3: This is an example function with 3 params: %d, %d, %d.\n", arg1, arg2, arg3);
    return 3;
}

int foo4(int arg1, int arg2, int arg3, int arg4)
{
    printf("Foo4: This is an example function with 4 params: %d, %d, %d, %d.\n", arg1, arg2, arg3, arg4);
    return 4;
}

int foo5(int arg1, int arg2, int arg3, int arg4, int arg5)
{
    printf("Foo5: This is an example function with 5 params: %d, %d, %d, %d, %d.\n", arg1, arg2, arg3, arg4, arg5);
    return 5;
}


int main(int argc, char **argv)
{
    int r;
    r = wrapped_fn(foo1, 0);
    printf("Returned value = %d.\n", r);

    r = wrapped_fn(foo2, 0, 1);
    printf("Returned value = %d.\n", r);

    r = wrapped_fn(foo3, 0, 1, 2);
    printf("Returned value = %d.\n", r);

    r = wrapped_fn(foo4, 0, 1, 2, 3);
    printf("Returned value = %d.\n", r);

    r = wrapped_fn(foo5, 0, 1, 2, 3, 4);
    printf("Returned value = %d.\n", r);

    return 0;
}
