#ifndef SYMBOLSLISTWIDGET_H
#define SYMBOLSLISTWIDGET_H

#include <QTableView>
#include "symbolsdatamodel.h"

class SymbolsListWidget : public QTableView
{
    Q_OBJECT

    SymbolsDataModel *m_model;

public:
    explicit SymbolsListWidget(QWidget *parent = 0);

signals:

public slots:

};

#endif // SYMBOLSLISTWIDGET_H
