TARGET = test

INCLUDEPATH += \
    ../inc

SOURCES += \
    test.c \
    ../time.c \
    ../rdtsc_cntrs.c \
    ../../stack_sample/cdecl_wrapper.c

HEADERS += \
    ../time.h \
    ../rdtsc_cntrs.h \
    ../../stack_sample/cdecl_wrapper.h

CONFIG -= qt

LIBS += -lrt
