TARGET = test

INCLUDEPATH += \
    ../inc

SOURCES += \
    test.c \
    ../time.c

HEADERS += \
    ../time.h

CONFIG -= qt

LIBS += -lrt
