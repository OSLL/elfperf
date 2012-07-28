#ifndef ELFFILE_H
#define ELFFILE_H

// see http://www.delorie.com/gnu/docs/binutils/bfd_toc.html

#include <QObject>
#include <bfd.h>
#include <QVector>

struct SymbolDescription
{
    QString             section;
    QString             symbol;
    unsigned int        flags;
    unsigned long long  value;
    SymbolDescription() {}

    SymbolDescription(QString             _section,
                      QString             _symbol,
                      unsigned int        _flags,
                      unsigned long long  _value) :
        section(_section), symbol(_symbol), flags(_flags), value(_value) {}
};

class ElfFile : public QObject
{
    Q_OBJECT


    QString                         m_name;
    bool                            m_initialized;
    bfd*                            m_bfd;
    QVector<SymbolDescription>      m_symbols;

public:
    explicit ElfFile(const QString& name, QObject *parent = 0);

    QString name() const;

protected:

    bool isInit();
    bool readExports();

signals:

public slots:

};

#endif // ELFFILE_H
