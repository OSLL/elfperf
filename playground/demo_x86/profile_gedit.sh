#!/bin/bash

bash -c 'LD_LIBRARY_PATH=/lib:/usr/lib:. LD_PRELOAD=../../src/libelfperf/libelfperf.so:/tools/lib/libdl.so.2 ELFPERF_ENABLE=true \
ELFPERF_PROFILE_FUNCTION=\
IceAddConnectionWatch\
:IceConnectionNumber\
:IceGetConnectionContext\
:IceProcessMessages\
:IceSetErrorHandler\
:IceSetIOErrorHandler\
:SmcCloseConnection\
:SmcDeleteProperties\
:SmcGetIceConnection\
:SmcInteractDone\
:SmcInteractRequest\
:SmcOpenConnection\
:SmcRequestSaveYourself\
:SmcSaveYourselfDone\
:SmcSetErrorHandler\
:SmcSetProperties\
:SmcVendor\
:XFree\
:XGetWindowProperty\
:__errno_location\
:__libc_start_main\
:__xstat\
:accept\
:atk_object_ref_relation_set\
:atk_object_set_description\
:atk_object_set_name\
:atk_object_set_role\
:atk_relation_new\
:atk_relation_set_add\
:bind\
:bind_textdomain_codeset\
:bindtextdomain\
:cairo_clip\
:cairo_create\
:cairo_destroy\
:cairo_fill\
:cairo_fill_preserve\
:cairo_matrix_init\
:cairo_pdf_surface_create_for_stream\
:cairo_rectangle\
:cairo_restore\
:cairo_save\
:cairo_scale\
:cairo_set_line_width\
:cairo_set_source_rgb\
:cairo_stroke\
:cairo_surface_destroy\
:cairo_transform\
:cairo_translate\
:close\
:connect\
:dcgettext\
:dcngettext\
:exit\
:fcntl\
:fflush\
:floor\
:free\
:g_array_append_vals\
:g_array_free\
:g_array_new\
:g_ascii_strcasecmp\
:g_ascii_strdown\
:g_ascii_strtod\
:g_ascii_table\
:g_ascii_tolower\
:g_assertion_message\
:g_assertion_message_expr\
:g_atomic_int_add\
:g_atomic_int_exchange_and_add\
:g_atomic_pointer_get\
:g_boxed_type_register_static\
:g_build_filename\
:g_cancellable_cancel\
:g_cancellable_is_cancelled\
:g_cancellable_new\
:g_cancellable_set_error_if_cancelled\
:g_cclosure_marshal_VOID__BOXED\
:g_cclosure_marshal_VOID__OBJECT\
:g_cclosure_marshal_VOID__POINTER\
:g_cclosure_marshal_VOID__UINT\
:g_cclosure_marshal_VOID__UINT_POINTER\
:g_cclosure_marshal_VOID__VOID\
:g_charset_converter_get_num_fallbacks\
:g_charset_converter_new\
:g_clear_error\
:g_content_type_from_mime_type\
:g_content_type_get_description\
:g_content_type_get_mime_type\
:g_content_type_guess\
:g_content_type_is_a\
:g_content_type_is_unknown\
:g_convert_error_quark\
:g_converter_convert\
:g_converter_get_type\
:g_converter_input_stream_new\
:g_converter_output_stream_new\
:g_converter_reset\
:g_dir_close\
:g_dir_open\
:g_dir_read_name\
:g_direct_equal\
:g_direct_hash\
:g_enum_register_static\
:g_error_free\
:g_file_equal\
:g_file_error_from_errno\
:g_file_error_quark\
:g_file_find_enclosing_mount\
:g_file_get_basename\
:g_file_get_contents\
:g_file_get_parent\
:g_file_get_parse_name\
:g_file_get_path\
:g_file_get_type\
:g_file_get_uri\
:g_file_get_uri_scheme\
:g_file_has_uri_scheme\
:g_file_info_copy_into\
:g_file_info_get_attribute_boolean\
:g_file_info_get_attribute_string\
:g_file_info_get_attribute_uint64\
:g_file_info_get_display_name\
:g_file_info_get_file_type\
:g_file_info_get_icon\
:g_file_info_get_modification_time\
:g_file_info_has_attribute\
:g_file_info_new\
:g_file_info_set_attribute\
:g_file_info_set_attribute_string\
:g_file_mount_enclosing_volume\
:g_file_mount_enclosing_volume_finish\
:g_file_new_for_commandline_arg\
:g_file_new_for_uri\
:g_file_query_exists\
:g_file_query_info\
:g_file_query_info_async\
:g_file_query_info_finish\
:g_file_read_async\
:g_file_read_finish\
:g_file_replace_async\
:g_file_replace_finish\
:g_file_set_attributes_async\
:g_file_set_attributes_finish\
:g_file_set_contents\
:g_file_test\
:g_filename_display_basename\
:g_filename_display_name\
:g_filename_from_uri\
:g_filename_to_uri\
:g_filename_to_utf8\
:g_find_program_in_path\
:g_flags_register_static\
:g_free\
:g_get_application_name\
:g_get_charset\
:g_get_current_time\
:g_get_home_dir\
:g_get_host_name\
:g_get_prgname\
:g_get_tmp_dir\
:g_get_user_cache_dir\
:g_get_user_config_dir\
:g_get_user_name\
:g_getenv\
:g_hash_table_destroy\
:g_hash_table_foreach\
:g_hash_table_foreach_remove\
:g_hash_table_insert\
:g_hash_table_lookup\
:g_hash_table_new\
:g_hash_table_new_full\
:g_hash_table_remove\
:g_idle_add\
:g_idle_add_full\
:g_input_stream_close\
:g_input_stream_close_async\
:g_input_stream_close_finish\
:g_input_stream_get_type\
:g_input_stream_read\
:g_input_stream_read_async\
:g_input_stream_read_finish\
:g_intern_static_string\
:g_io_add_watch\
:g_io_channel_flush\
:g_io_channel_set_line_term\
:g_io_channel_shutdown\
:g_io_channel_unix_get_fd\
:g_io_channel_unix_new\
:g_io_channel_unref\
:g_io_channel_write_chars\
:g_io_error_quark\
:g_key_file_error_quark\
:g_key_file_free\
:g_key_file_get_boolean\
:g_key_file_get_double\
:g_key_file_get_groups\
:g_key_file_get_integer\
:g_key_file_get_keys\
:g_key_file_get_locale_string\
:g_key_file_get_locale_string_list\
:g_key_file_get_start_group\
:g_key_file_get_string\
:g_key_file_get_string_list\
:g_key_file_get_value\
:g_key_file_has_group\
:g_key_file_has_key\
:g_key_file_load_from_data_dirs\
:g_key_file_load_from_dirs\
:g_key_file_load_from_file\
:g_key_file_new\
:g_key_file_set_boolean\
:g_key_file_set_integer\
:g_key_file_set_string\
:g_key_file_set_string_list\
:g_key_file_set_value\
:g_key_file_to_data\
:g_list_append\
:g_list_concat\
:g_list_copy\
:g_list_delete_link\
:g_list_find\
:g_list_find_custom\
:g_list_foreach\
:g_list_free\
:g_list_index\
:g_list_insert_before\
:g_list_last\
:g_list_length\
:g_list_nth\
:g_list_nth_data\
:g_list_prepend\
:g_list_remove\
:g_list_reverse\
:g_list_sort\
:g_listenv\
:g_log\
:g_log_default_handler\
:g_log_set_handler\
:g_malloc\
:g_malloc0\
:g_malloc0_n\
:g_malloc_n\
:g_markup_escape_text\
:g_markup_printf_escaped\
:g_mkdir_with_parents\
:g_module_build_path\
:g_module_close\
:g_module_error\
:g_module_make_resident\
:g_module_open\
:g_module_supported\
:g_module_symbol\
:g_mount_get_name\
:g_mount_operation_new\
:g_object_add_weak_pointer\
:g_object_class_install_property\
:g_object_get\
:g_object_get_data\
:g_object_new\
:g_object_new_valist\
:g_object_notify\
:g_object_ref\
:g_object_ref_sink\
:g_object_set\
:g_object_set_data\
:g_object_set_data_full\
:g_object_unref\
:g_object_weak_ref\
:g_once_init_enter_impl\
:g_once_init_leave\
:g_option_context_add_group\
:g_option_context_add_main_entries\
:g_option_context_free\
:g_option_context_new\
:g_option_context_parse\
:g_option_group_add_entries\
:g_option_group_new\
:g_option_group_set_parse_hooks\
:g_output_stream_close\
:g_output_stream_close_async\
:g_output_stream_close_finish\
:g_output_stream_get_type\
:g_output_stream_write\
:g_output_stream_write_async\
:g_output_stream_write_finish\
:g_param_spec_boolean\
:g_param_spec_boxed\
:g_param_spec_enum\
:g_param_spec_flags\
:g_param_spec_get_name\
:g_param_spec_int\
:g_param_spec_object\
:g_param_spec_pointer\
:g_param_spec_string\
:g_param_spec_uint\
:g_path_get_basename\
:g_path_get_dirname\
:g_path_is_absolute\
:g_pattern_match_string\
:g_pattern_spec_free\
:g_pattern_spec_new\
:g_print\
:g_propagate_error\
:g_ptr_array_add\
:g_ptr_array_foreach\
:g_ptr_array_free\
:g_ptr_array_new\
:g_ptr_array_remove_index_fast\
:g_quark_from_static_string\
:g_random_int\
:g_realloc\
:g_return_if_fail_warning\
:g_set_application_name\
:g_set_error\
:g_set_error_literal\
:g_shell_parse_argv\
:g_shell_quote\
:g_signal_connect_data\
:g_signal_connect_object\
:g_signal_emit\
:g_signal_handler_block\
:g_signal_handler_disconnect\
:g_signal_handler_unblock\
:g_signal_handlers_block_matched\
:g_signal_handlers_disconnect_matched\
:g_signal_handlers_unblock_matched\
:g_signal_has_handler_pending\
:g_signal_lookup\
:g_signal_new\
:g_signal_stop_emission_by_name\
:g_slice_alloc\
:g_slice_free1\
:g_slist_copy\
:g_slist_delete_link\
:g_slist_find\
:g_slist_find_custom\
:g_slist_foreach\
:g_slist_free\
:g_slist_length\
:g_slist_prepend\
:g_slist_remove\
:g_slist_reverse\
:g_slist_sort\
:g_snprintf\
:g_source_remove\
:g_spawn_async_with_pipes\
:g_static_mutex_get_mutex_impl\
:g_str_equal\
:g_str_has_prefix\
:g_str_has_suffix\
:g_str_hash\
:g_strconcat\
:g_strdelimit\
:g_strdup\
:g_strdup_printf\
:g_strdup_vprintf\
:g_strerror\
:g_strfreev\
:g_string_append\
:g_string_append_len\
:g_string_append_printf\
:g_string_free\
:g_string_insert_c\
:g_string_new\
:g_string_new_len\
:g_string_sized_new\
:g_strjoin\
:g_strjoinv\
:g_strndup\
:g_strrstr\
:g_strsplit\
:g_strv_get_type\
:g_strv_length\
:g_thread_functions_for_glib_use\
:g_thread_init\
:g_thread_use_default_impl\
:g_threads_got_initialized\
:g_timeout_add\
:g_timeout_add_full\
:g_timeout_add_seconds\
:g_timer_destroy\
:g_timer_elapsed\
:g_timer_new\
:g_type_add_interface_static\
:g_type_check_class_cast\
:g_type_check_instance_cast\
:g_type_check_instance_is_a\
:g_type_check_is_value_type\
:g_type_class_add_private\
:g_type_class_peek\
:g_type_class_peek_parent\
:g_type_class_ref\
:g_type_class_unref\
:g_type_init\
:g_type_instance_get_private\
:g_type_interface_peek\
:g_type_is_a\
:g_type_module_get_type\
:g_type_module_set_name\
:g_type_module_unuse\
:g_type_module_use\
:g_type_name\
:g_type_register_static\
:g_type_register_static_simple\
:g_type_value_table_peek\
:g_unichar_break_type\
:g_unichar_isdigit\
:g_unlink\
:g_unsetenv\
:g_uri_list_extract_uris\
:g_uri_parse_scheme\
:g_uri_unescape_segment\
:g_uri_unescape_string\
:g_utf8_casefold\
:g_utf8_collate\
:g_utf8_get_char\
:g_utf8_get_char_validated\
:g_utf8_normalize\
:g_utf8_offset_to_pointer\
:g_utf8_skip\
:g_utf8_strchr\
:g_utf8_strlen\
:g_utf8_validate\
:g_value_copy\
:g_value_dup_boxed\
:g_value_dup_string\
:g_value_get_boolean\
:g_value_get_boxed\
:g_value_get_enum\
:g_value_get_flags\
:g_value_get_int\
:g_value_get_object\
:g_value_get_pointer\
:g_value_get_string\
:g_value_get_uint\
:g_value_init\
:g_value_peek_pointer\
:g_value_reset\
:g_value_set_boolean\
:g_value_set_boxed\
:g_value_set_enum\
:g_value_set_flags\
:g_value_set_int\
:g_value_set_object\
:g_value_set_pointer\
:g_value_set_string\
:g_value_set_uint\
:g_value_take_string\
:g_value_transform\
:g_value_unset\
:g_warn_message\
:gconf_client_add_dir\
:gconf_client_error\
:gconf_client_get\
:gconf_client_get_default\
:gconf_client_get_list\
:gconf_client_get_string\
:gconf_client_key_is_writable\
:gconf_client_notify_add\
:gconf_client_set_bool\
:gconf_client_set_int\
:gconf_client_set_list\
:gconf_client_set_string\
:gconf_client_unreturned_error\
:gconf_error_quark\
:gconf_escape_key\
:gconf_value_free\
:gconf_value_get_bool\
:gconf_value_get_int\
:gconf_value_get_list_type\
:gconf_value_get_string\
:gdk_atom_intern_static_string\
:gdk_cairo_create\
:gdk_cairo_rectangle\
:gdk_color_parse\
:gdk_cursor_new\
:gdk_cursor_new_for_display\
:gdk_cursor_unref\
:gdk_display_get_default\
:gdk_display_get_default_screen\
:gdk_display_get_name\
:gdk_display_get_pointer\
:gdk_display_get_screen\
:gdk_display_manager_get\
:gdk_display_manager_list_displays\
:gdk_display_open\
:gdk_display_supports_selection_notification\
:gdk_drag_status\
:gdk_drawable_get_display\
:gdk_drawable_get_size\
:gdk_error_trap_pop\
:gdk_error_trap_push\
:gdk_event_free\
:gdk_event_new\
:gdk_notify_startup_complete\
:gdk_pixbuf_get_height\
:gdk_pixbuf_get_type\
:gdk_pixbuf_get_width\
:gdk_pixbuf_new_from_file\
:gdk_pixbuf_scale_simple\
:gdk_pointer_grab\
:gdk_pointer_is_grabbed\
:gdk_pointer_ungrab\
:gdk_rectangle_intersect\
:gdk_screen_get_default\
:gdk_screen_get_display\
:gdk_screen_get_height\
:gdk_screen_get_monitor_at_window\
:gdk_screen_get_monitor_geometry\
:gdk_screen_get_number\
:gdk_screen_get_resolution\
:gdk_screen_get_root_window\
:gdk_screen_get_type\
:gdk_screen_get_width\
:gdk_screen_make_display_name\
:gdk_set_sm_client_id\
:gdk_threads_enter\
:gdk_threads_leave\
:gdk_threads_lock\
:gdk_threads_unlock\
:gdk_window_at_pointer\
:gdk_window_get_origin\
:gdk_window_get_position\
:gdk_window_get_toplevel\
:gdk_window_get_user_data\
:gdk_window_object_get_type\
:gdk_window_set_cursor\
:gdk_x11_display_broadcast_startup_message\
:gdk_x11_display_get_user_time\
:gdk_x11_display_get_xdisplay\
:gdk_x11_drawable_get_xid\
:gdk_x11_get_server_time\
:gdk_x11_get_xatom_by_name_for_display\
:gdk_x11_window_set_user_time\
:geteuid\
:getpid\
:gtk_about_dialog_get_type\
:gtk_about_dialog_set_url_hook\
:gtk_accel_map_load\
:gtk_accel_map_save\
:gtk_accelerator_get_default_mod_mask\
:gtk_accessible_get_type\
:gtk_action_get_name\
:gtk_action_get_type\
:gtk_action_group_add_action\
:gtk_action_group_add_action_with_accel\
:gtk_action_group_add_actions\
:gtk_action_group_add_toggle_actions\
:gtk_action_group_get_action\
:gtk_action_group_get_sensitive\
:gtk_action_group_list_actions\
:gtk_action_group_new\
:gtk_action_group_remove_action\
:gtk_action_group_set_sensitive\
:gtk_action_group_set_translation_domain\
:gtk_action_new\
:gtk_action_set_sensitive\
:gtk_activatable_get_related_action\
:gtk_activatable_get_type\
:gtk_activatable_set_related_action\
:gtk_adjustment_get_value\
:gtk_adjustment_set_value\
:gtk_adjustment_value_changed\
:gtk_arrow_new\
:gtk_bin_get_child\
:gtk_bin_get_type\
:gtk_binding_entry_add_signal\
:gtk_binding_set_by_class\
:gtk_bindings_activate\
:gtk_box_get_type\
:gtk_box_pack_end\
:gtk_box_pack_start\
:gtk_box_reorder_child\
:gtk_box_set_spacing\
:gtk_builder_add_from_file\
:gtk_builder_add_objects_from_file\
:gtk_builder_get_object\
:gtk_builder_new\
:gtk_button_box_get_type\
:gtk_button_box_set_layout\
:gtk_button_get_type\
:gtk_button_new_from_stock\
:gtk_button_new_with_mnemonic\
:gtk_button_set_image\
:gtk_button_set_label\
:gtk_button_set_relief\
:gtk_cell_layout_add_attribute\
:gtk_cell_layout_get_cells\
:gtk_cell_layout_get_type\
:gtk_cell_layout_pack_end\
:gtk_cell_layout_pack_start\
:gtk_cell_layout_set_attributes\
:gtk_cell_layout_set_cell_data_func\
:gtk_cell_renderer_get_type\
:gtk_cell_renderer_pixbuf_new\
:gtk_cell_renderer_text_new\
:gtk_cell_renderer_toggle_new\
:gtk_check_menu_item_get_active\
:gtk_check_menu_item_get_type\
:gtk_check_menu_item_new_with_label\
:gtk_check_menu_item_new_with_mnemonic\
:gtk_check_menu_item_set_active\
:gtk_clipboard_get_display\
:gtk_clipboard_request_contents\
:gtk_combo_box_entry_get_type\
:gtk_combo_box_get_active\
:gtk_combo_box_get_active_iter\
:gtk_combo_box_get_model\
:gtk_combo_box_get_type\
:gtk_combo_box_new_with_model\
:gtk_combo_box_set_active\
:gtk_combo_box_set_active_iter\
:gtk_combo_box_set_model\
:gtk_combo_box_set_row_separator_func\
:gtk_container_add\
:gtk_container_foreach\
:gtk_container_get_children\
:gtk_container_get_type\
:gtk_container_remove\
:gtk_container_set_border_width\
:gtk_dialog_add_action_widget\
:gtk_dialog_add_button\
:gtk_dialog_add_buttons\
:gtk_dialog_get_action_area\
:gtk_dialog_get_content_area\
:gtk_dialog_get_type\
:gtk_dialog_run\
:gtk_dialog_set_default_response\
:gtk_dialog_set_has_separator\
:gtk_dialog_set_response_sensitive\
:gtk_drag_check_threshold\
:gtk_drag_dest_find_target\
:gtk_drag_dest_get_target_list\
:gtk_drag_dest_set\
:gtk_drag_dest_set_target_list\
:gtk_drag_finish\
:gtk_drag_get_data\
:gtk_editable_get_chars\
:gtk_editable_insert_text\
:gtk_entry_completion_get_entry\
:gtk_entry_completion_get_model\
:gtk_entry_completion_new\
:gtk_entry_completion_set_inline_completion\
:gtk_entry_completion_set_match_func\
:gtk_entry_completion_set_minimum_key_length\
:gtk_entry_completion_set_model\
:gtk_entry_completion_set_popup_completion\
:gtk_entry_completion_set_text_column\
:gtk_entry_get_text\
:gtk_entry_get_type\
:gtk_entry_new\
:gtk_entry_set_activates_default\
:gtk_entry_set_completion\
:gtk_entry_set_icon_from_stock\
:gtk_entry_set_max_length\
:gtk_entry_set_text\
:gtk_entry_set_width_chars\
:gtk_event_box_get_type\
:gtk_event_box_new\
:gtk_event_box_set_visible_window\
:gtk_file_chooser_add_filter\
:gtk_file_chooser_dialog_get_type\
:gtk_file_chooser_dialog_new\
:gtk_file_chooser_get_action\
:gtk_file_chooser_get_file\
:gtk_file_chooser_get_filename\
:gtk_file_chooser_get_files\
:gtk_file_chooser_get_filter\
:gtk_file_chooser_get_type\
:gtk_file_chooser_get_uri\
:gtk_file_chooser_set_current_folder_uri\
:gtk_file_chooser_set_current_name\
:gtk_file_chooser_set_do_overwrite_confirmation\
:gtk_file_chooser_set_extra_widget\
:gtk_file_chooser_set_file\
:gtk_file_chooser_set_filter\
:gtk_file_filter_add_custom\
:gtk_file_filter_add_pattern\
:gtk_file_filter_get_name\
:gtk_file_filter_new\
:gtk_file_filter_set_name\
:gtk_font_button_get_font_name\
:gtk_font_button_get_type\
:gtk_font_button_set_font_name\
:gtk_frame_get_type\
:gtk_frame_new\
:gtk_frame_set_shadow_type\
:gtk_get_current_event\
:gtk_get_current_event_time\
:gtk_get_option_group\
:gtk_grab_add\
:gtk_grab_remove\
:gtk_hbox_get_type\
:gtk_hbox_new\
:gtk_hbutton_box_new\
:gtk_hpaned_new\
:gtk_icon_info_free\
:gtk_icon_info_load_icon\
:gtk_icon_size_lookup\
:gtk_icon_size_lookup_for_settings\
:gtk_icon_theme_append_search_path\
:gtk_icon_theme_get_default\
:gtk_icon_theme_get_for_screen\
:gtk_icon_theme_has_icon\
:gtk_icon_theme_load_icon\
:gtk_icon_theme_lookup_by_gicon\
:gtk_im_context_reset\
:gtk_image_clear\
:gtk_image_get_animation\
:gtk_image_get_icon_name\
:gtk_image_get_icon_set\
:gtk_image_get_image\
:gtk_image_get_pixbuf\
:gtk_image_get_pixmap\
:gtk_image_get_stock\
:gtk_image_get_storage_type\
:gtk_image_get_type\
:gtk_image_menu_item_get_type\
:gtk_image_menu_item_new_with_mnemonic\
:gtk_image_menu_item_set_image\
:gtk_image_new\
:gtk_image_new_from_icon_name\
:gtk_image_new_from_stock\
:gtk_image_set_from_animation\
:gtk_image_set_from_icon_name\
:gtk_image_set_from_icon_set\
:gtk_image_set_from_image\
:gtk_image_set_from_pixbuf\
:gtk_image_set_from_pixmap\
:gtk_image_set_from_stock\
:gtk_info_bar_add_button\
:gtk_info_bar_get_content_area\
:gtk_info_bar_get_type\
:gtk_info_bar_new\
:gtk_info_bar_new_with_buttons\
:gtk_info_bar_set_default_response\
:gtk_info_bar_set_message_type\
:gtk_init\
:gtk_label_get_label\
:gtk_label_get_type\
:gtk_label_new\
:gtk_label_new_with_mnemonic\
:gtk_label_set_ellipsize\
:gtk_label_set_justify\
:gtk_label_set_line_wrap\
:gtk_label_set_markup\
:gtk_label_set_mnemonic_widget\
:gtk_label_set_selectable\
:gtk_label_set_single_line_mode\
:gtk_label_set_text\
:gtk_label_set_use_markup\
:gtk_layout_get_bin_window\
:gtk_layout_get_hadjustment\
:gtk_layout_get_size\
:gtk_layout_get_type\
:gtk_layout_get_vadjustment\
:gtk_layout_new\
:gtk_layout_set_size\
:gtk_list_store_append\
:gtk_list_store_clear\
:gtk_list_store_get_type\
:gtk_list_store_insert\
:gtk_list_store_insert_after\
:gtk_list_store_iter_is_valid\
:gtk_list_store_move_after\
:gtk_list_store_new\
:gtk_list_store_prepend\
:gtk_list_store_remove\
:gtk_list_store_set\
:gtk_main\
:gtk_main_quit\
:gtk_menu_attach\
:gtk_menu_attach_to_widget\
:gtk_menu_get_type\
:gtk_menu_item_get_type\
:gtk_menu_item_new\
:gtk_menu_item_new_with_label\
:gtk_menu_item_new_with_mnemonic\
:gtk_menu_new\
:gtk_menu_popup\
:gtk_menu_shell_append\
:gtk_menu_shell_get_type\
:gtk_menu_shell_prepend\
:gtk_menu_shell_select_first\
:gtk_menu_shell_select_item\
:gtk_menu_tool_button_get_type\
:gtk_menu_tool_button_new_from_stock\
:gtk_menu_tool_button_set_arrow_tooltip_text\
:gtk_menu_tool_button_set_menu\
:gtk_message_dialog_format_secondary_text\
:gtk_message_dialog_get_type\
:gtk_message_dialog_new\
:gtk_message_dialog_new_with_markup\
:gtk_misc_get_type\
:gtk_misc_set_alignment\
:gtk_misc_set_padding\
:gtk_mount_operation_new\
:gtk_notebook_append_page_menu\
:gtk_notebook_get_current_page\
:gtk_notebook_get_n_pages\
:gtk_notebook_get_nth_page\
:gtk_notebook_get_tab_label\
:gtk_notebook_get_tab_pos\
:gtk_notebook_get_type\
:gtk_notebook_insert_page\
:gtk_notebook_new\
:gtk_notebook_next_page\
:gtk_notebook_page_num\
:gtk_notebook_popup_enable\
:gtk_notebook_prev_page\
:gtk_notebook_remove_page\
:gtk_notebook_reorder_child\
:gtk_notebook_set_current_page\
:gtk_notebook_set_scrollable\
:gtk_notebook_set_show_border\
:gtk_notebook_set_show_tabs\
:gtk_notebook_set_tab_pos\
:gtk_object_get_type\
:gtk_orientation_get_type\
:gtk_page_setup_copy\
:gtk_page_setup_get_orientation\
:gtk_page_setup_get_paper_size\
:gtk_page_setup_get_type\
:gtk_page_setup_new\
:gtk_page_setup_new_from_file\
:gtk_page_setup_to_file\
:gtk_paned_get_type\
:gtk_paned_pack1\
:gtk_paned_pack2\
:gtk_paned_set_position\
:gtk_paper_size_get_height\
:gtk_paper_size_get_width\
:gtk_print_context_get_page_setup\
:gtk_print_context_set_cairo_context\
:gtk_print_operation_cancel\
:gtk_print_operation_get_default_page_setup\
:gtk_print_operation_get_error\
:gtk_print_operation_get_print_settings\
:gtk_print_operation_get_type\
:gtk_print_operation_new\
:gtk_print_operation_preview_end_preview\
:gtk_print_operation_preview_get_type\
:gtk_print_operation_preview_is_selected\
:gtk_print_operation_preview_render_page\
:gtk_print_operation_run\
:gtk_print_operation_set_allow_async\
:gtk_print_operation_set_custom_tab_label\
:gtk_print_operation_set_default_page_setup\
:gtk_print_operation_set_embed_page_setup\
:gtk_print_operation_set_job_name\
:gtk_print_operation_set_n_pages\
:gtk_print_operation_set_print_settings\
:gtk_print_operation_set_unit\
:gtk_print_settings_copy\
:gtk_print_settings_get_type\
:gtk_print_settings_new\
:gtk_print_settings_new_from_file\
:gtk_print_settings_to_file\
:gtk_print_settings_unset\
:gtk_progress_bar_get_type\
:gtk_progress_bar_new\
:gtk_progress_bar_pulse\
:gtk_progress_bar_set_fraction\
:gtk_propagate_event\
:gtk_radio_action_get_current_value\
:gtk_radio_action_get_group\
:gtk_radio_action_get_type\
:gtk_radio_action_new\
:gtk_radio_action_set_group\
:gtk_rc_parse_string\
:gtk_rc_style_new\
:gtk_recent_chooser_get_current_uri\
:gtk_recent_chooser_get_type\
:gtk_recent_chooser_menu_new_for_manager\
:gtk_recent_chooser_set_filter\
:gtk_recent_chooser_set_limit\
:gtk_recent_chooser_set_local_only\
:gtk_recent_chooser_set_sort_type\
:gtk_recent_filter_add_group\
:gtk_recent_filter_new\
:gtk_recent_info_get_display_name\
:gtk_recent_info_get_modified\
:gtk_recent_info_get_uri\
:gtk_recent_info_has_group\
:gtk_recent_info_ref\
:gtk_recent_info_unref\
:gtk_recent_manager_add_full\
:gtk_recent_manager_get_default\
:gtk_recent_manager_get_items\
:gtk_recent_manager_remove_item\
:gtk_scrolled_window_get_type\
:gtk_scrolled_window_new\
:gtk_scrolled_window_set_policy\
:gtk_scrolled_window_set_shadow_type\
:gtk_selection_data_get_data\
:gtk_selection_data_targets_include_text\
:gtk_separator_menu_item_get_type\
:gtk_separator_menu_item_new\
:gtk_separator_tool_item_new\
:gtk_show_about_dialog\
:gtk_show_uri\
:gtk_source_buffer_begin_not_undoable_action\
:gtk_source_buffer_can_redo\
:gtk_source_buffer_can_undo\
:gtk_source_buffer_end_not_undoable_action\
:gtk_source_buffer_get_highlight_matching_brackets\
:gtk_source_buffer_get_highlight_syntax\
:gtk_source_buffer_get_language\
:gtk_source_buffer_get_style_scheme\
:gtk_source_buffer_get_type\
:gtk_source_buffer_redo\
:gtk_source_buffer_set_highlight_matching_brackets\
:gtk_source_buffer_set_highlight_syntax\
:gtk_source_buffer_set_language\
:gtk_source_buffer_set_max_undo_levels\
:gtk_source_buffer_set_style_scheme\
:gtk_source_buffer_undo\
:gtk_source_iter_backward_search\
:gtk_source_iter_forward_search\
:gtk_source_language_get_hidden\
:gtk_source_language_get_id\
:gtk_source_language_get_mime_types\
:gtk_source_language_get_name\
:gtk_source_language_get_section\
:gtk_source_language_get_type\
:gtk_source_language_manager_get_language\
:gtk_source_language_manager_get_language_ids\
:gtk_source_language_manager_guess_language\
:gtk_source_language_manager_new\
:gtk_source_print_compositor_draw_page\
:gtk_source_print_compositor_get_n_pages\
:gtk_source_print_compositor_get_pagination_progress\
:gtk_source_print_compositor_get_type\
:gtk_source_print_compositor_paginate\
:gtk_source_print_compositor_set_header_format\
:gtk_source_style_scheme_get_description\
:gtk_source_style_scheme_get_filename\
:gtk_source_style_scheme_get_id\
:gtk_source_style_scheme_get_name\
:gtk_source_style_scheme_get_style\
:gtk_source_style_scheme_get_type\
:gtk_source_style_scheme_manager_append_search_path\
:gtk_source_style_scheme_manager_force_rescan\
:gtk_source_style_scheme_manager_get_scheme\
:gtk_source_style_scheme_manager_get_scheme_ids\
:gtk_source_style_scheme_manager_get_type\
:gtk_source_style_scheme_manager_new\
:gtk_source_view_get_insert_spaces_instead_of_tabs\
:gtk_source_view_get_show_line_numbers\
:gtk_source_view_get_tab_width\
:gtk_source_view_get_type\
:gtk_source_view_set_auto_indent\
:gtk_source_view_set_highlight_current_line\
:gtk_source_view_set_insert_spaces_instead_of_tabs\
:gtk_source_view_set_right_margin_position\
:gtk_source_view_set_show_line_numbers\
:gtk_source_view_set_show_right_margin\
:gtk_source_view_set_smart_home_end\
:gtk_source_view_set_tab_width\
:gtk_spin_button_get_type\
:gtk_spin_button_get_value_as_int\
:gtk_spin_button_set_value\
:gtk_spinner_get_type\
:gtk_spinner_new\
:gtk_spinner_start\
:gtk_spinner_stop\
:gtk_statusbar_get_context_id\
:gtk_statusbar_get_type\
:gtk_statusbar_new\
:gtk_statusbar_pop\
:gtk_statusbar_push\
:gtk_statusbar_remove\
:gtk_statusbar_set_has_resize_grip\
:gtk_table_attach_defaults\
:gtk_table_get_type\
:gtk_table_set_row_spacings\
:gtk_target_list_add_uri_targets\
:gtk_target_list_new\
:gtk_target_list_unref\
:gtk_text_buffer_apply_tag\
:gtk_text_buffer_begin_user_action\
:gtk_text_buffer_copy_clipboard\
:gtk_text_buffer_create_mark\
:gtk_text_buffer_create_tag\
:gtk_text_buffer_cut_clipboard\
:gtk_text_buffer_delete\
:gtk_text_buffer_delete_interactive\
:gtk_text_buffer_delete_mark\
:gtk_text_buffer_delete_selection\
:gtk_text_buffer_end_user_action\
:gtk_text_buffer_get_bounds\
:gtk_text_buffer_get_char_count\
:gtk_text_buffer_get_end_iter\
:gtk_text_buffer_get_has_selection\
:gtk_text_buffer_get_insert\
:gtk_text_buffer_get_iter_at_line\
:gtk_text_buffer_get_iter_at_mark\
:gtk_text_buffer_get_iter_at_offset\
:gtk_text_buffer_get_line_count\
:gtk_text_buffer_get_modified\
:gtk_text_buffer_get_selection_bounds\
:gtk_text_buffer_get_slice\
:gtk_text_buffer_get_start_iter\
:gtk_text_buffer_get_tag_table\
:gtk_text_buffer_get_type\
:gtk_text_buffer_insert\
:gtk_text_buffer_insert_at_cursor\
:gtk_text_buffer_move_mark\
:gtk_text_buffer_move_mark_by_name\
:gtk_text_buffer_paste_clipboard\
:gtk_text_buffer_place_cursor\
:gtk_text_buffer_remove_tag\
:gtk_text_buffer_select_range\
:gtk_text_buffer_set_modified\
:gtk_text_buffer_set_text\
:gtk_text_iter_backward_chars\
:gtk_text_iter_backward_line\
:gtk_text_iter_backward_lines\
:gtk_text_iter_backward_to_tag_toggle\
:gtk_text_iter_begins_tag\
:gtk_text_iter_compare\
:gtk_text_iter_ends_line\
:gtk_text_iter_ends_tag\
:gtk_text_iter_ends_word\
:gtk_text_iter_equal\
:gtk_text_iter_forward_char\
:gtk_text_iter_forward_chars\
:gtk_text_iter_forward_line\
:gtk_text_iter_forward_lines\
:gtk_text_iter_forward_to_line_end\
:gtk_text_iter_forward_to_tag_toggle\
:gtk_text_iter_get_buffer\
:gtk_text_iter_get_bytes_in_line\
:gtk_text_iter_get_char\
:gtk_text_iter_get_chars_in_line\
:gtk_text_iter_get_line\
:gtk_text_iter_get_offset\
:gtk_text_iter_get_slice\
:gtk_text_iter_get_type\
:gtk_text_iter_has_tag\
:gtk_text_iter_in_range\
:gtk_text_iter_is_cursor_position\
:gtk_text_iter_is_end\
:gtk_text_iter_is_start\
:gtk_text_iter_order\
:gtk_text_iter_set_line_offset\
:gtk_text_iter_starts_line\
:gtk_text_iter_starts_word\
:gtk_text_tag_get_type\
:gtk_text_tag_set_priority\
:gtk_text_tag_table_get_size\
:gtk_text_view_buffer_to_window_coords\
:gtk_text_view_get_buffer\
:gtk_text_view_get_editable\
:gtk_text_view_get_line_at_y\
:gtk_text_view_get_line_yrange\
:gtk_text_view_get_overwrite\
:gtk_text_view_get_type\
:gtk_text_view_get_visible_rect\
:gtk_text_view_get_window\
:gtk_text_view_scroll_mark_onscreen\
:gtk_text_view_scroll_to_mark\
:gtk_text_view_set_cursor_visible\
:gtk_text_view_set_editable\
:gtk_text_view_set_wrap_mode\
:gtk_toggle_action_get_active\
:gtk_toggle_action_get_type\
:gtk_toggle_action_set_active\
:gtk_toggle_button_get_active\
:gtk_toggle_button_get_type\
:gtk_toggle_button_new\
:gtk_toggle_button_set_active\
:gtk_toggle_button_set_inconsistent\
:gtk_tool_button_get_type\
:gtk_tool_button_new\
:gtk_tool_button_new_from_stock\
:gtk_tool_button_set_label\
:gtk_tool_button_set_use_underline\
:gtk_tool_item_get_type\
:gtk_tool_item_new\
:gtk_tool_item_set_homogeneous\
:gtk_tool_item_set_is_important\
:gtk_tool_item_set_tooltip_text\
:gtk_toolbar_get_type\
:gtk_toolbar_insert\
:gtk_toolbar_new\
:gtk_toolbar_set_style\
:gtk_toolbar_unset_style\
:gtk_tooltip_set_markup\
:gtk_tooltip_set_text\
:gtk_tree_model_get\
:gtk_tree_model_get_column_type\
:gtk_tree_model_get_iter\
:gtk_tree_model_get_iter_first\
:gtk_tree_model_get_path\
:gtk_tree_model_get_type\
:gtk_tree_model_iter_next\
:gtk_tree_model_sort_new_with_model\
:gtk_tree_path_free\
:gtk_tree_path_get_indices\
:gtk_tree_path_new_from_indices\
:gtk_tree_path_new_from_string\
:gtk_tree_path_next\
:gtk_tree_path_prev\
:gtk_tree_selection_get_selected\
:gtk_tree_selection_get_type\
:gtk_tree_selection_select_iter\
:gtk_tree_selection_selected_foreach\
:gtk_tree_selection_set_mode\
:gtk_tree_sortable_get_type\
:gtk_tree_sortable_set_default_sort_func\
:gtk_tree_sortable_set_sort_column_id\
:gtk_tree_view_append_column\
:gtk_tree_view_column_add_attribute\
:gtk_tree_view_column_new\
:gtk_tree_view_column_new_with_attributes\
:gtk_tree_view_column_pack_start\
:gtk_tree_view_column_set_cell_data_func\
:gtk_tree_view_column_set_resizable\
:gtk_tree_view_column_set_sort_column_id\
:gtk_tree_view_column_set_spacing\
:gtk_tree_view_column_set_title\
:gtk_tree_view_convert_widget_to_bin_window_coords\
:gtk_tree_view_get_bin_window\
:gtk_tree_view_get_cell_area\
:gtk_tree_view_get_column\
:gtk_tree_view_get_cursor\
:gtk_tree_view_get_model\
:gtk_tree_view_get_path_at_pos\
:gtk_tree_view_get_selection\
:gtk_tree_view_get_type\
:gtk_tree_view_new\
:gtk_tree_view_new_with_model\
:gtk_tree_view_set_cursor\
:gtk_tree_view_set_enable_search\
:gtk_tree_view_set_headers_visible\
:gtk_tree_view_set_model\
:gtk_tree_view_set_reorderable\
:gtk_tree_view_set_rules_hint\
:gtk_tree_view_set_search_column\
:gtk_tree_view_set_search_equal_func\
:gtk_ui_manager_add_ui\
:gtk_ui_manager_add_ui_from_file\
:gtk_ui_manager_ensure_update\
:gtk_ui_manager_get_accel_group\
:gtk_ui_manager_get_action\
:gtk_ui_manager_get_widget\
:gtk_ui_manager_insert_action_group\
:gtk_ui_manager_new\
:gtk_ui_manager_new_merge_id\
:gtk_ui_manager_remove_ui\
:gtk_vbox_get_type\
:gtk_vbox_new\
:gtk_vpaned_new\
:gtk_widget_add_events\
:gtk_widget_destroy\
:gtk_widget_destroyed\
:gtk_widget_ensure_style\
:gtk_widget_error_bell\
:gtk_widget_event\
:gtk_widget_get_accessible\
:gtk_widget_get_clipboard\
:gtk_widget_get_direction\
:gtk_widget_get_display\
:gtk_widget_get_pango_context\
:gtk_widget_get_parent\
:gtk_widget_get_screen\
:gtk_widget_get_settings\
:gtk_widget_get_style\
:gtk_widget_get_toplevel\
:gtk_widget_get_type\
:gtk_widget_get_window\
:gtk_widget_grab_focus\
:gtk_widget_hide\
:gtk_widget_is_toplevel\
:gtk_widget_modify_base\
:gtk_widget_modify_font\
:gtk_widget_modify_style\
:gtk_widget_modify_text\
:gtk_widget_queue_draw\
:gtk_widget_queue_draw_area\
:gtk_widget_realize\
:gtk_widget_set_name\
:gtk_widget_set_sensitive\
:gtk_widget_set_size_request\
:gtk_widget_set_tooltip_markup\
:gtk_widget_set_tooltip_text\
:gtk_widget_show\
:gtk_widget_show_all\
:gtk_widget_size_request\
:gtk_widget_style_get\
:gtk_window_activate_key\
:gtk_window_add_accel_group\
:gtk_window_fullscreen\
:gtk_window_get_focus\
:gtk_window_get_group\
:gtk_window_get_position\
:gtk_window_get_role\
:gtk_window_get_screen\
:gtk_window_get_size\
:gtk_window_get_transient_for\
:gtk_window_get_type\
:gtk_window_group_add_window\
:gtk_window_group_new\
:gtk_window_group_remove_window\
:gtk_window_is_active\
:gtk_window_maximize\
:gtk_window_move\
:gtk_window_new\
:gtk_window_present\
:gtk_window_present_with_time\
:gtk_window_propagate_key_event\
:gtk_window_resize\
:gtk_window_set_default_icon_from_file\
:gtk_window_set_default_icon_name\
:gtk_window_set_default_size\
:gtk_window_set_destroy_with_parent\
:gtk_window_set_modal\
:gtk_window_set_position\
:gtk_window_set_resizable\
:gtk_window_set_role\
:gtk_window_set_screen\
:gtk_window_set_skip_taskbar_hint\
:gtk_window_set_title\
:gtk_window_set_transient_for\
:gtk_window_stick\
:gtk_window_unfullscreen\
:gtk_window_unmaximize\
:gtk_window_unstick\
:listen\
:memchr\
:memcpy\
:open\
:pango_context_get_language\
:pango_context_get_metrics\
:pango_find_paragraph_boundary\
:pango_font_description_free\
:pango_font_description_from_string\
:pango_font_metrics_get_approximate_char_width\
:pango_font_metrics_get_approximate_digit_width\
:pango_font_metrics_unref\
:read\
:socket\
:stdout\
:strchr\
:strcmp\
:strlen\
:strncmp\
:strncpy\
:strrchr\
:strtol\
:strtoul\
:textdomain\
:time\
:unlink /tools/lib/ld-2.11.3.so /usr/bin/gedit'

# bug with "setlocale profiling"
