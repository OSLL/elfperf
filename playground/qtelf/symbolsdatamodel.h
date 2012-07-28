#ifndef SYMBOLSDATAMODEL_H
#define SYMBOLSDATAMODEL_H

#include <QAbstractTableModel>

class SymbolsDataModel : public QAbstractTableModel
{
    Q_OBJECT
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
