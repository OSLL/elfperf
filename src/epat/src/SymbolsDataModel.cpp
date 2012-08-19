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
 * \file SymbolsDataModel.cpp
 * \brief SymbolsDataModel implementation
 *
 * PROJ: OSLL/epat
 * ---------------------------------------------------------------- */

#include "SymbolsDataModel.h"
#include <QApplication>

SymbolsDataModel::SymbolsDataModel(QObject *parent) :
    QAbstractTableModel(parent), m_elf(QSharedPointer<ElfFile>(new ElfFile(qApp->applicationFilePath())))
{
    m_data = m_elf->getSymbols();
}

void SymbolsDataModel::setBinary(const QString& name)
{
    m_elf = QSharedPointer<ElfFile>(new ElfFile(name));
    m_data = m_elf->getSymbols();
    emit layoutChanged();
}

int SymbolsDataModel::rowCount(const QModelIndex& parent) const
{
    return m_data.size();
}

int SymbolsDataModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant SymbolsDataModel::data(const QModelIndex& index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        const SymbolDescription& sd=m_data[index.row()];

        switch(index.column())
        {
        case 0: // number
            return QVariant(index.row());
        case 1: // section
            return sd.section;
        case 2: // symbol
            return sd.symbol;
        case 3: // flags
            return sd.flags;
        case 4: // section
            return sd.value;
        }

        return "!";
    }
    return QVariant();
}
