TARGET = perfcounter

INCLUDEPATH += inc \
    ../common/inc

SOURCES += \
    src/main.cpp \
    ../common/src/CommandExecutor.cpp \
    ../common/src/DataParser.cpp

HEADERS += \
    ../common/inc/CommandExecutor.h \
    ../common/inc/DataParser.h

CONFIG -= qt
CONFIG += debug


