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

#include <QDebug>

ElfFileReader::ElfFileReader(const char* filename) :
    m_filename(QString(filename))
{
}

ElfFileReader::ElfFileReader(const QString &filename) :
    m_filename(filename)
{
}

QString ElfFileReader::getFilename() const
{
    return m_filename;
}

void ElfFileReader::setFilename(const QString &filename)
{
    m_filename = filename;
}

QSharedPointer<QStringList> ElfFileReader::getImports() const
{
    QString command = "objdump -T " + QString(m_filename) + " | grep \"\\*UND\\*\" | c++filt";
    CommandExecutor e(command);
    int status = e.exec();

    if (status != 0) {
        return QSharedPointer<QStringList>(0);
    } else {
        DynSymTableParser parser(e.getOutput());
        parser.parse();
        return parser.getAnswer();
    }
}
