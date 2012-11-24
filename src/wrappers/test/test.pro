TARGET = test

SOURCES = \
    ../../timers/hpet_cntrs.c \
    ../../timers/rdtsc_cntrs.c \
    ../../timers/global_stats.c \
    ../../timers/test/test.c \
    ../cdecl_wrapper.c \
    ../wrapper_redirector.c \
    test_cdecl_wrapper.c \
    test_shift.c \
    test_many_redirs.c


HEADERS = \
    ../../timers/hpet_cntrs.h \
    ../../timers/rdtsc_cntrs.h \
    ../../timers/global_stats.h \
    ../cdecl_wrapper.h \
    ../wrapper_redirector.h

CONFIG -= qt
