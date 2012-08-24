TARGET = elfperf

INCLUDEPATH += inc

SOURCES += \
    src/main.cpp \
    src/ElfFile.cpp \
    src/SymbolsDataModel.cpp \
    src/SymbolsListWidget.cpp \
    src/MainWindow.cpp \
    src/CommandExecutor.cpp \
    src/ElfFileReader.cpp \
    src/DynSymTableParser.cpp \
    src/DataParser.cpp

HEADERS += \
    inc/ElfFile.h \
    inc/SymbolsDataModel.h \
    inc/SymbolsListWidget.h \
    inc/MainWindow.h \
    inc/CommandExecutor.h \
    inc/ElfFileReader.h \
    inc/DataParser.h \
    inc/DynSymTableParser.h

LIBS += /usr/lib/libbfd.a -liberty


