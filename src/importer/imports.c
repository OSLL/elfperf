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
 * \file imports.c
 * \brief Implementation of functions for getting imports from elf file
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "imports.h"
#include "parsers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** execCommand(const char* cmd, const char* mode, int* size)
{
    FILE* stream = popen(cmd, mode);
    if (stream != NULL) {
        char** output = NULL;
        int linesCount = 0;

        char buffer[BUFSIZ + 1];
        long rSize = 1;
        do {
            memset(buffer, '\0', sizeof(buffer));
            rSize = fread(buffer, sizeof(char), BUFSIZ, stream);
            int i;
            int k = 0;
            for (i = 0; i < rSize; i++) {
                if (buffer[i] == '\n') {
                    char* line = strndup(buffer + k, i - k);
                    linesCount++;
                    output= (char**)realloc(output, sizeof(*output) * linesCount);
                    output[linesCount - 1] = line;
                    k = i + 1;
                }
            }
        }
        while(rSize != 0);

        pclose(stream);
        *size = linesCount;
        return output;
    } else {
        pclose(stream);
        *size = 0;
        return NULL;
    }
}

struct Import* getImports(const char* filename, int* count)
{
    // Get data from dynamic symbol table
    char command[100] = "objdump -T ";
    strcat(command, filename);
    strcat(command, " | grep \"\\*UND\\*\" | c++filt");
    int outputSize;
    char** output = execCommand(command, "r", &outputSize);

    if (output == NULL) {
        free(output);
        *count = 0;
        return NULL;
    }

    int symbolsCount;
    char** symbols = getSymbols(output, outputSize, "*UND*", &symbolsCount);

    // Get list of dependencies
    memset(command, '\0', 100);
    strcat(command, "ldd ");
    strcat(command, filename);
    free(output);
    outputSize = 0;
    output = execCommand(command, "r", &outputSize);

    if (output == NULL) {
        free(symbols);
        free(output);
        *count = 0;
        return NULL;
    }

    int libsCount;
    char** libs = getSharedLibs(output, outputSize, &libsCount);

    // Prepare answer by combination of imports and symbols from libs
    struct Import* imports = NULL;
    int importsCount = 0;
    int i;
    for (i = 0; i < libsCount; i++) {
        importsCount++;
        imports = (struct Import*)realloc(imports, sizeof(*imports) * importsCount);

        memset(command, '\0', 100);
        strcat(command, "objdump -T ");
        strcat(command, libs[i]);
        strcat(command, " | grep -w .text | c++filt");
        free(output);

        outputSize = 0;
        output = execCommand(command, "r", &outputSize);
        if (output == NULL) continue;

        int sharedSymbolsCount;
        char** sharedSymbols = getSymbols(output, outputSize, ".text", &sharedSymbolsCount);

        char** resultSymbols = NULL;
        int resultSymbolsCount = 0;

        int j, k;
        for (j = 0; j < sharedSymbolsCount; j++) {
            for (k = 0; k < symbolsCount; k++) {
                if (strcmp(sharedSymbols[j], symbols[k]) == 0) {
                    resultSymbolsCount++;
                    resultSymbols = (char**)realloc(resultSymbols, sizeof(*resultSymbols) * resultSymbolsCount);
                    resultSymbols[resultSymbolsCount - 1] = sharedSymbols[j];
                    break;
                }
            }
        }

        struct Import import;
        import.lib = libs[i];
        import.functions = resultSymbols;
        import.fn_count = resultSymbolsCount;
        imports[importsCount - 1] = import;
        free(sharedSymbols);
    }

    free(output);
    free(symbols);
    free(libs);

    *count = importsCount;
    return imports;
}
