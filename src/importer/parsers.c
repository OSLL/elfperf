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
 * \file parsers.c
 * \brief Implementation of functions for parsing of program outputs
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "parsers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** getSymbols(char** data, int dataSize, const char* section, int* count)
{
    if (data == NULL || dataSize == 0) {
        *count == 0;
        return NULL;
    }

    char** symbols = NULL;
    int symbolsCount = 0;
    int i;
    for (i = 0; i < dataSize; i++) {
        char* line = data[i];
        char* key = strstr(line, section);
        if (key == NULL) continue;
        key = key + 16;
        while (*key == ' ') key++;
        symbolsCount++;
        symbols = (char**)realloc(symbols, sizeof(*symbols) * symbolsCount);
        symbols[symbolsCount - 1] = key;
    }

    *count = symbolsCount;
    return symbols;
}

char** getSharedLibs(char** data, int dataSize, int* count)
{
    if (data == NULL || dataSize == 0) {
        *count == 0;
        return NULL;
    }

    char** libs = NULL;
    int libsCount = 0;
    int i;
    for (i = 0; i < dataSize; i++) {
        char* line = data[i];
        char* leftBound = strstr(line, "=>");
        char* rightBound = strstr(line, "(");

        if (leftBound == NULL) {
            leftBound = line + 1;
        }
        else
            leftBound += 2;
        if (rightBound == NULL || leftBound > rightBound)
            continue;
        else
            rightBound -= 1;

        while (*leftBound == ' ' && leftBound < rightBound ) {
            leftBound++;
        }
        if (leftBound == rightBound)
            continue;
        libsCount++;
        libs = (char**)realloc(libs, sizeof(*libs) * libsCount);
        libs[libsCount - 1] = strndup(leftBound, rightBound - leftBound);
    }

    *count = libsCount;
    return libs;
}
