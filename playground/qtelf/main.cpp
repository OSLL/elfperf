#include <QApplication>
#include <QDebug>
#include "elffile.h"
#include "symbolslistwidget.h"

int main(int c, char **v)
{
    QApplication app(c,v);

    SymbolsListWidget slw;
    slw.show();

    return app.exec();
}
