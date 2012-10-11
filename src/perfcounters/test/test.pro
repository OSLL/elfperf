TARGET = test

INCLUDEPATH += \
    ../inc

SOURCES += \
    test.cpp \
    ../src/time_cntrs.cpp


HEADERS += \
    ../inc/time_cntrs.h


CONFIG -= qt

LIBS += -lrt
