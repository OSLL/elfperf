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
 * \file main.cpp
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>
#include "CommandExecutor.h"

const char* programName;

void printUsage(FILE* stream, int exitCode)
{
    fprintf(stream, "Usage: %s options [ inputfile ... ]\n", programName);
    fprintf(stream,
            "  -h, --help              Display usage information.\n"
            "  -t, --time filename     Get program execution time.\n");
    exit(exitCode);
}

void getExecutionTime(const char* filename)
{
    string command = "perf stat -e task-clock -x , " + string(filename) + " 0> /tmp/ep_runtime ";
    CommandExecutor e(command);
    int status = e.exec();
    if (status != 0) {
        return;
    }

    command = "cat /tmp/ep_runtime ";
    e.setCmdName(command);
    status = e.exec();
    if (status != 0) {
        return;
    }

    string answer = *(e.getOutput()->begin());
    int keyPos = answer.find(',');
    if (keyPos == -1)
        return;

    answer = answer.substr(0, answer.length() - keyPos - 4);
    cout << "Execution time of " << filename << " = " << answer << " ms" << endl;

    command = "rm -f /tmp/ep_runtime ";
    e.setCmdName(command);
    status = e.exec();
    if (status != 0) {
        return;
    }
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
        { "time", 1, NULL, 't' },
        { NULL,      0, NULL, 0   }
    };
    const char* const short_options = "ht:";

    int next_option = getopt_long(argc, argv, short_options, long_options, NULL);
    switch (next_option)
    {
    case 'h':
        printUsage(stdout, 0);
    case 't':
        getExecutionTime(optarg);
        break;
    case '?':
        printUsage(stderr, 1);
        break;
    case '-1':
        break;
    default:
        printNoArgsMsg(stdout);
    }

    return 0;
}
