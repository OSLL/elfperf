TARGET = elfperf

INCLUDEPATH += inc

SOURCES += \
    src/main.cpp \
    src/CommandExecutor.cpp \
    src/ElfFileReader.cpp \
    src/DynSymTableParser.cpp \
    src/DataParser.cpp \
    src/SharedLibsParser.cpp

HEADERS += \
    inc/CommandExecutor.h \
    inc/ElfFileReader.h \
    inc/DataParser.h \
    inc/DynSymTableParser.h \
    inc/SharedLibsParser.h

CONFIG -= qt
CONFIG += debug


