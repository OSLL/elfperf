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
 * \file SharedLibsParser.cpp
 * \brief SharedLibsParser implementation
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "SharedLibsParser.h"
#include <stdio.h>

SharedLibsParser::SharedLibsParser(list<string> *data) :
    DataParser(data)
{
}

void SharedLibsParser::parse()
{
    if (getData() != 0) {
        list<string>* answer = new list<string>();
        list<string>* data = getData();
        list<string>::iterator i;
        for (i = data->begin(); i != data->end(); i++) {
            string line = *i;
            int leftBound = line.find("=>");
            int rightBound = line.find("(");
            if (leftBound == -1)
                leftBound = 1;
            else
                leftBound += 2;
            if (rightBound == -1 || leftBound > rightBound)
                continue;
            else
                rightBound -= 1;
            while (line[leftBound] == ' ' && leftBound < rightBound ) {
                leftBound++;
            }
            if (leftBound == rightBound)
                continue;
            answer->push_back(line.substr(leftBound , rightBound - leftBound));
        }
        setAnswer(answer);
    }
}

SharedLibsParser::~SharedLibsParser()
{
}
