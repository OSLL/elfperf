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
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "imports.h"

const char* programName;

void printUsage(FILE* stream, int exitCode)
{
    fprintf(stream, "Usage: %s options [ inputfile ... ]\n", programName);
    fprintf(stream,
            "  -h, --help                 Display usage information.\n"
            "  -i, --imports filename     Process getting imports from ELF file with filename.\n");
    exit(exitCode);
}

void readImports(const char* filename)
{
    int importsCount;
    struct Import* imports = getImports(filename, &importsCount);

    if (imports == NULL) {
        fprintf(stderr, "Error while getting imports from the file\n");
        return;
    }

    int i, j;
    for (i = 0; i < importsCount; i++) {
        printf("\n===== %s =====\n", imports[i].lib);
        for (j = 0; j != imports[i].fn_count; j++) {
            printf("%s\n", imports[i].functions[j]);
        }
        free(imports[i].functions);
    }

    free(imports);
}

void printNoArgsMsg(FILE* stream) {
    fprintf(stream, "%s: missing file arguments\n", programName);
    fprintf(stream, "Try '%s --help' for more information.\n", programName);
}

int main(int argc, char **argv)
{
    programName = argv[0];
    const struct option long_options[] = {
        { "help", 1, NULL, 'h' },
        { "imports", 1, NULL, 'i' },
        { NULL,      0, NULL, 0   }
    };
    const char* const short_options = "hi:";

    int next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option)
    {
    case 'h':
        printUsage(stdout, 0);
    case 'i':
        readImports(optarg);
        break;
    case '?':
        printUsage(stderr, 1);
        break;
    case -1:
        printUsage(stderr, 1);
        break;
    default:
        printNoArgsMsg(stdout);
    }

    return 0;
}
