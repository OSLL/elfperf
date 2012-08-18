INCLUDEPATH += inc

SOURCES += \
    src/main.cpp \
    src/ElfFile.cpp \
    src/SymbolsDataModel.cpp \
    src/SymbolsListWidget.cpp

HEADERS += \
    inc/ElfFile.h \
    inc/SymbolsDataModel.h \
    inc/SymbolsListWidget.h

LIBS += /usr/lib/libbfd.a -liberty


