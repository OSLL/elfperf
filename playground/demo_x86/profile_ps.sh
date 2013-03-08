#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
__libc_start_main\
:closeproc\
:ctime\
:dev_to_tty\
:display_version\
:escape_command\
:escape_str\
:escape_strlist\
:exit\
:fprintf\
:freesupgrp\
:fwrite\
:get_pid_digits\
:getenv\
:geteuid\
:getgrnam\
:getpagesize\
:getpwnam\
:gnu_dev_major\
:gnu_dev_minor\
:ioctl\
:isatty\
:localtime\
:look_up_our_self\
:lookup_wchan\
:meminfo\
:mmap\
:mprotect\
:open_psdb\
:openproc\
:printf\
:puts\
:qsort\
:read\
:readproc\
:readproctab2\
:readtask\
:setlocale\
:sigaction\
:sigfillset\
:signal_number_to_name\
:snprintf\
:strcasecmp\
:strcmp\
:strcspn\
:strdup\
:strftime\
:strlen\
:strncmp\
:strncpy\
:strpbrk\
:strspn\
:strtoul\
:time\
:uptime\
  /tools/lib/ld-2.11.3.so /bin/ps'

#Empty results with
# strtol
# strcpy
# malloc
# memcpy
# free
# _exit
# atoi
# close
# strchr
# open
# memset
# __xstat
# bsearch
