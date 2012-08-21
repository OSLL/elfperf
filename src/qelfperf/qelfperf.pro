TARGET = elfperf

INCLUDEPATH += inc

SOURCES += \
    src/main.cpp \
    src/ElfFile.cpp \
    src/SymbolsDataModel.cpp \
    src/SymbolsListWidget.cpp \
    src/MainWindow.cpp

HEADERS += \
    inc/ElfFile.h \
    inc/SymbolsDataModel.h \
    inc/SymbolsListWidget.h \
    inc/MainWindow.h

LIBS += /usr/lib/libbfd.a -liberty


