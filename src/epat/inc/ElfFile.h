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
 * \file ElfFile.h
 * \brief ElFile declaration
 *
 * PROJ: OSLL/epat
 * ---------------------------------------------------------------- */

#ifndef ELFFILE_H
#define ELFFILE_H

// see http://www.delorie.com/gnu/docs/binutils/bfd_toc.html

#include <QObject>
#include <bfd.h>
#include <QVector>
#include <QSharedPointer>

struct SymbolDescription
{
    QString             section;
    QString             symbol;
    unsigned int        flags;
    unsigned long long  value;
    SymbolDescription() {}

    SymbolDescription(QString             _section,
                      QString             _symbol,
                      unsigned int        _flags,
                      unsigned long long  _value) :
        section(_section), symbol(_symbol), flags(_flags), value(_value) {}
};

class ElfFile : public QObject
{
    Q_OBJECT


    QString                         m_name;
    bool                            m_initialized;
    QSharedPointer<bfd>             m_bfd;
    QVector<SymbolDescription>      m_symbols;

public:
    explicit ElfFile(const QString& name, QObject* parent = 0);

    QString name() const;
    QVector<SymbolDescription> getSymbols() const;

protected:

    bool isInit();
    bool readExports();

signals:

public slots:

    bool setFileName(const QString& name);

};

#endif // ELFFILE_H
