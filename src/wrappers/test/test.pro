TARGET = test

SOURCES = \
    ../../timers/hpet_cntrs.c \
    ../../timers/rdtsc_cntrs.c \
    ../cdecl_wrapper.c \
    ../wrapper_redirector.c \
    test_cdecl_wrapper.c \
    test_shift.c \
    test_many_redirs.c


HEADERS = \
    ../../timers/hpet_cntrs.c \
    ../../timers/rdtsc_cntrs.c \
    ../cdecl_wrapper.h \
    ../wrapper_redirector.h

CONFIG -= qt
