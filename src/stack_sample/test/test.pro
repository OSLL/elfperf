TARGET = test

SOURCES = \
    ../../perfcounters/time.c \
    ../cdecl_wrapper.c \
    ../wrapper_redirector.c \
    test_cdecl_wrapper.c \
    test_shift.c \
    test_many_redirs.c


HEADERS = \
    ../../perfcounters/time.h \
    ../cdecl_wrapper.h \
    ../wrapper_redirector.h

CONFIG -= qt
