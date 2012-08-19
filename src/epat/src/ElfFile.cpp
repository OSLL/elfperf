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
 * \file ElfFile.cpp
 * \brief ElFile implementation
 *
 * PROJ: OSLL/epat
 * ---------------------------------------------------------------- */

#include <QDebug>

#include "ElfFile.h"

ElfFile::ElfFile(const QString& name, QObject* parent) :
    QObject(parent), m_name(name), m_initialized(false),
    m_symbols(QSharedPointer<SymbolsMap>(new SymbolsMap()))
{
    bfd_init();
    readExports();
}

QString ElfFile::name() const
{
    return m_name;
}

QSharedPointer<SymbolsMap> ElfFile::getSymbols() const
{
    return m_symbols;
}

/*QSharedPointer<SymbolsMap> ElfFile::getSymbolsMap() const
{
    return m_symbolsMap;
}*/

bool ElfFile::isInit()
{
    return m_initialized;
}

bool ElfFile::readExports()
{
    long storage_needed;
    asymbol **symbol_table;
    long number_of_symbols;
    long i;

    m_symbols->clear();

    bfd_set_default_target("bfd_target_elf_flavour");
    m_bfd = QSharedPointer<bfd>(bfd_openr(m_name.toStdString().c_str(),NULL));

    if(!bfd_check_format(m_bfd.data(), bfd_object))
    {
        return false;
    }

    storage_needed = bfd_get_symtab_upper_bound(m_bfd.data());
    symbol_table = (asymbol **)malloc(storage_needed);
    number_of_symbols = bfd_canonicalize_symtab(m_bfd.data(), symbol_table);

    qDebug() << "Number of symbols " << number_of_symbols;

    for(i = 0; i < number_of_symbols; i++)
    {
        QString symbolName = symbol_table[i]->section->name;
        SymbolDescription symbol(
                    symbol_table[i]->section->name,
                    symbol_table[i]->name,
                    symbol_table[i]->flags,
                    symbol_table[i]->value);
        if (!m_symbols->contains(symbolName)) {
            m_symbols->insert(symbolName, QSharedPointer<Symbols>(new Symbols()));
        }
        m_symbols->value(symbolName)->append(symbol);
        //qDebug() << m_symbols->value(symbolName) << " : " << symbol;
        /*m_symbols->append(SymbolDescription(
                              symbol_table[i]->section->name,
                              symbol_table[i]->name,
                              symbol_table[i]->flags,
                              symbol_table[i]->value));
        //qDebug() << "found " << symbol_table[i]->name;*/
    }
    free(symbol_table);
    symbol_table = NULL;
    return true;
}

bool ElfFile::setFileName(const QString& name)
{
    m_name = name;
    return readExports();
}
