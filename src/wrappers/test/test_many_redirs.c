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
 * \file test_many_redirs.c
 * \brief Test for realy big amount of redirectors
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cdecl_wrapper.h"
#include "../wrapper_redirector.h"
#include "../../timers/global_stats.h"

#define STRUCT_A_VALUE 1
#define STRUCT_B_VALUE 2
#define STRUCT_C_VALUE 3

// Test structure, with size > 8byte
struct testStruct{
    int a;
    int b;
    int c;
};


struct testStruct testFunction(){
    struct testStruct result;
    result.a = STRUCT_A_VALUE;
    result.b = STRUCT_B_VALUE;
    result.c = STRUCT_C_VALUE;
    int i;
    for (i = 0; i < 1000000; i++);
    return result;
}

int main()
{
    int n = 100;
    char* funcNames[n];
    void* funcPointers[n];

    int i;
    for (i = 0; i < n; i++) {
        funcNames[i] = malloc(13 * sizeof(char));
        sprintf(funcNames[i], "testFunction%d",i);
        funcPointers[i] = testFunction;
    }

    initWrapperRedirectors(funcNames, n, (void*)wrapper);

    for (i = 0; i < n; i++) {
        addNewFunction(funcNames[i], funcPointers[i]);
    }

    struct testStruct result;
    for (i = 0; i < n; i++) {
        printf("%d: => Calling wrapper for %s\n", i, funcNames[i]);
        struct testStruct (*func)() = getRedirectorAddressForName(funcNames[i]);
        // Calling wrapper instead of testFunction2
        result =  (*func)();
        printf("<= Wrapper called for %s, result = (%d,%d,%d), expected_result = (%d,%d,%d)\n",
               funcNames[i],
               result.a, result.b, result.c,
               STRUCT_A_VALUE, STRUCT_B_VALUE, STRUCT_C_VALUE);
        struct FunctionStatistic* stat = getFunctionStatistic(testFunction);
        printf("Function statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);
    }

    printf("=== Total statistics ===\n");
    struct FunctionStatistic* stat = getFunctionStatistic(testFunction);
    printf("Function statistics:\n\tduration = %llu ticks\n", stat->totalDiffTime);

    return 0;
}
