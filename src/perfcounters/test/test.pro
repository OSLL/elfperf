TARGET = test

INCLUDEPATH += \
    ../inc

SOURCES += \
    test.c \
    ../time.c \
    ../stack_sample/cdecl_wrapper.c 

HEADERS += \
    ../time.h \
    ../stack_sample/cdecl_wrapper.h 

CONFIG -= qt

LIBS += -lrt
