TARGET = elfperf

INCLUDEPATH += inc \
    ../common/inc

SOURCES += \
    ../common/src/CommandExecutor.cpp \
    ../common/src/DataParser.cpp \
    src/main.cpp \
    src/ElfFileReader.cpp \
    src/DynSymTableParser.cpp \
    src/SharedLibsParser.cpp

HEADERS += \
    ../common/inc/CommandExecutor.h \
    ../common/inc/DataParser.h \
    inc/ElfFileReader.h \
    inc/DynSymTableParser.h \
    inc/SharedLibsParser.h

CONFIG -= qt
CONFIG += debug


