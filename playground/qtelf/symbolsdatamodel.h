#ifndef SYMBOLSDATAMODEL_H
#define SYMBOLSDATAMODEL_H

#include <QAbstractTableModel>
#include "elffile.h"

class SymbolsDataModel : public QAbstractTableModel
{
    Q_OBJECT

    ElfFile                     m_elf;
    QVector<SymbolDescription>  m_data;

public:
    explicit SymbolsDataModel(QObject *parent = 0);

    void setBinary(const QString& name);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:

};

#endif // SYMBOLSDATAMODEL_H
