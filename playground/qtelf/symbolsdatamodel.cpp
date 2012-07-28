#include "symbolsdatamodel.h"
#include <QApplication>

SymbolsDataModel::SymbolsDataModel(QObject *parent) :
    QAbstractTableModel(parent), m_elf(qApp->applicationFilePath())
{
    m_data = m_elf.getSymbols();
}

void SymbolsDataModel::setBinary(const QString &name)
{
    m_data = m_elf.getSymbols();
    emit layoutChanged();
}

int SymbolsDataModel::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}

int SymbolsDataModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

QVariant SymbolsDataModel::data(const QModelIndex &index, int role) const
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
