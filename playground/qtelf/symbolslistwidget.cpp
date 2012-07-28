#include "symbolslistwidget.h"

SymbolsListWidget::SymbolsListWidget(QWidget *parent) :
    QTableView(parent)
{
    m_model = new SymbolsDataModel(this);
}
