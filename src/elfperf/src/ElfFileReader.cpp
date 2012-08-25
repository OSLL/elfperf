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
 * \file ElfFileReader.cpp
 * \brief ElFileReader implementation
 *
 * PROJ: OSLL/elfperf
 * ---------------------------------------------------------------- */

#include "ElfFileReader.h"
#include "CommandExecutor.h"
#include "DynSymTableParser.h"
#include "SharedLibsParser.h"
#include <algorithm>


ElfFileReader::ElfFileReader(const char* filename) :
    m_filename(string(filename))
{
}

ElfFileReader::ElfFileReader(const string& filename) :
    m_filename(filename)
{
}

string ElfFileReader::getFilename() const
{
    return m_filename;
}

void ElfFileReader::setFilename(const string &filename)
{
    m_filename = filename;
}

ImportsMap* ElfFileReader::getImports() const
{
    // Get data from dynamic symbol table
    string command = "objdump -T " + m_filename + " | grep \"\\*UND\\*\" | c++filt";
    CommandExecutor e(command);
    int status = e.exec();
    if (status != 0) {
        return 0;
    }

    DynSymTableParser dstParser(e.getOutput());
    dstParser.parse();

    list<string>* symbols = new list<string>();
    symbols->resize(dstParser.getAnswer()->size());
    copy(dstParser.getAnswer()->begin(), dstParser.getAnswer()->end(), symbols->begin());

    // Get list of dependencies
    command = "ldd " + m_filename;
    e.setCmdName(command);
    status = e.exec();
    if (status != 0) {
        delete(symbols);
        return 0;
    }

    SharedLibsParser slParser(e.getOutput());
    slParser.parse();

    list<string>* libs = new list<string>();
    libs->resize(slParser.getAnswer()->size());
    copy(slParser.getAnswer()->begin(), slParser.getAnswer()->end(), libs->begin());

    // Prepare answer by combination of imports and symbols from libs
    ImportsMap* imports = new ImportsMap();
    list<string>::iterator i, j;
    for (i = libs->begin(); i != libs->end(); i++) {
        command = "objdump -T " + *i + " | grep -w .text | c++filt";
        e.setCmdName(command);
        status = e.exec();
        if (status != 0) {
            continue;
        }

        list<string>* functions = new list<string>();
        imports->insert(pair<string, list<string>*>(*i, functions));

        dstParser.setData(e.getOutput());
        dstParser.setSection(".text");
        dstParser.parse();
        list<string>* sharedLibSymbols = dstParser.getAnswer();

        for (j = symbols->begin(); j != symbols->end(); j++) {
            list<string>::iterator p = find(sharedLibSymbols->begin(), sharedLibSymbols->end(), *j);
            if (p != sharedLibSymbols->end())
                functions->push_back(*j);
        }
    }

    delete(symbols);
    delete(libs);
    return imports;
}
