#include "symbolslistwidget.h"
#include <QDropEvent>
#include <QDebug>

SymbolsListWidget::SymbolsListWidget(QWidget *parent) :
    QTableView(parent)
{
    m_model = new SymbolsDataModel(this);
    setModel(m_model);
}

void SymbolsListWidget::setBinary(const QString &binary)
{
    m_model->setBinary(binary);
}
