#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
:__assert_fail\
:__ctype_b_loc\
:__ctype_get_mb_cur_max\
:__ctype_tolower_loc\
:__ctype_toupper_loc\
:__cxa_atexit\
:__errno_location\
:__fpending\
:__libc_start_main\
:__overflow\
:__strtoull_internal\
:__uflow\
:__xstat64\
:_exit\
:_obstack_begin\
:_obstack_newchunk\
:abort\
:bindtextdomain\
:calloc\
:close\
:closedir\
:dcgettext\
:dlopen\
:dlsym\
:error\
:exit\
:fclose\
:fflush_unlocked\
:fnmatch\
:fopen64\
:fprintf\
:fputc_unlocked\
:fputs_unlocked\
:fread_unlocked\
:free\
:fwrite\
:fwrite_unlocked\
:getenv\
:getopt_long\
:getpagesize\
:isatty\
:iswalnum\
:iswalpha\
:iswctype\
:iswlower\
:iswprint\
:iswupper\
:lseek64\
:malloc\
:mbrtowc\
:mbsinit\
:memchr\
:memcmp\
:memcpy\
:memmove\
:memset\
:nl_langinfo\
:obstack_free\
:open64\
:opendir\
:printf\
:putchar\
:re_compile_pattern\
:re_match\
:re_search\
:re_set_syntax\
:read\
:readdir64\
:realloc\
:setlocale\
:strcasecmp\
:strchr\
:strcmp\
:strcoll\
:strcpy\
:strcspn\
:strlen\
:strncmp\
:strncpy\
:strrchr\
:textdomain\
:towlower\
:towupper\
:wcrtomb\
:wcscoll\
:wctob\
:wctype\
  /tools/lib/ld-2.11.3.so /bin/grep "ls" *'
