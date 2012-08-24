TARGET = elfperf

INCLUDEPATH += inc

SOURCES += \
    src/main.cpp \
    src/CommandExecutor.cpp \
    src/ElfFileReader.cpp \
    src/DynSymTableParser.cpp \
    src/DataParser.cpp

HEADERS += \
    inc/CommandExecutor.h \
    inc/ElfFileReader.h \
    inc/DataParser.h \
    inc/DynSymTableParser.h

LIBS += /usr/lib/libbfd.a -liberty

CONFIG -= qt
CONFIG += debug


