#include "symbolsdatamodel.h"

SymbolsDataModel::SymbolsDataModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

void SymbolsDataModel::setBinary(const QString &name)
{
}

int SymbolsDataModel::rowCount(const QModelIndex &parent) const
{
    return 3;
}

int SymbolsDataModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant SymbolsDataModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        return "!";
    }
    return QVariant();
}
