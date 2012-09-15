TEMPLATE = lib

TARGET = ../lib/epcommon

INCLUDEPATH += inc

SOURCES += \
    src/CommandExecutor.cpp \
    src/DataParser.cpp

HEADERS += \
    inc/CommandExecutor.h \
    inc/DataParser.h

CONFIG -= qt
