#include <QDebug>

#include "elffile.h"

ElfFile::ElfFile(const QString& name, QObject *parent) :
    QObject(parent), m_name(name), m_initialized(false)
{
    bfd_init();
    readExports();
}

QString ElfFile::name() const
{
    return m_name;
}

QVector<SymbolDescription> ElfFile::getSymbols() const
{
    return m_symbols;
}

bool ElfFile::isInit()
{
    return m_initialized;
}

bool ElfFile::readExports()
{
    long storage_needed;
    asymbol **symbol_table;
    long number_of_symbols;
    long i;

    m_symbols.clear();

    bfd_set_default_target("bfd_target_elf_flavour");
    m_bfd = bfd_openr(m_name.toStdString().c_str(),NULL);

    if(!bfd_check_format(m_bfd,bfd_object))
    {
        return false;
    }

    storage_needed = bfd_get_symtab_upper_bound(m_bfd);
    symbol_table = (asymbol **)malloc(storage_needed);
    number_of_symbols = bfd_canonicalize_symtab(m_bfd, symbol_table);

    qDebug() << "Number of symbols " << number_of_symbols;

    for(i = 0; i < number_of_symbols; i++)
    {
        m_symbols << SymbolDescription(
                         symbol_table[i]->section->name,
                         symbol_table[i]->name,
                         symbol_table[i]->flags,
                         symbol_table[i]->value);
        qDebug() << "found " << symbol_table[i]->name;
    }
    free(symbol_table);
    symbol_table = NULL;
    return true;
}

bool ElfFile::setFileName(const QString &name)
{
    m_name = name;
    return readExports();
}
