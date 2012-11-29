TARGET = cpufreqmeter

SOURCES += \
    cpufreqmeter.c \
    ../timers/rdtsc_cntrs.c

HEADERS += \
    ../timers/rdtsc_cntrs.h

CONFIG -= qt

LIBS += -lrt
