#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
fclose\
:fwrite\
:g_getenv\
:g_unsetenv\
:IceSetIOErrorHandler\
:IceConnectionNumber\
:SmcSetErrorHandler\
:g_pattern_match_string\
:g_dir_read_name\
:gtk_action_group_new\
:gtk_action_group_set_translation_domain\
:gtk_ui_manager_insert_action_group\
:gtk_ui_manager_new_merge_id\
:gtk_ui_manager_add_ui\
:gtk_toggle_action_get_type\
:gtk_toggle_action_set_active\
:gtk_toggle_action_get_active\
:gtk_source_language_manager_new\
:gtk_source_language_manager_get_language\
:gtk_source_language_manager_get_language_ids\
:gtk_action_group_add_action_with_accel\
:g_get_user_name /tools/lib/ld-2.11.3.so /usr/bin/gedit'

