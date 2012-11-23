TARGET = test

INCLUDEPATH += \
    ../inc

SOURCES += \
    test.c \
    ../hpet_cntrs.c \
    ../rdtsc_cntrs.c \
    ../../wrappers/cdecl_wrapper.c

HEADERS += \
    ../hpet_cntrs.h \
    ../rdtsc_cntrs.h \
    ../../wrappers/cdecl_wrapper.h

CONFIG -= qt

LIBS += -lrt
