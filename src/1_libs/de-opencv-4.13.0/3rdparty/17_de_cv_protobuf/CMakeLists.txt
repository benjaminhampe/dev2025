set(LIB_NAME de_cv_protobuf)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/google/protobuf/any.h
        ${LIB_HEADER_DIR}/google/protobuf/arena.h
        ${LIB_HEADER_DIR}/google/protobuf/arenastring.h
        ${LIB_HEADER_DIR}/google/protobuf/arena_impl.h
        ${LIB_HEADER_DIR}/google/protobuf/descriptor.h
        ${LIB_HEADER_DIR}/google/protobuf/descriptor.pb.h
        ${LIB_HEADER_DIR}/google/protobuf/descriptor_database.h
        ${LIB_HEADER_DIR}/google/protobuf/dynamic_message.h
        ${LIB_HEADER_DIR}/google/protobuf/explicitly_constructed.h
        ${LIB_HEADER_DIR}/google/protobuf/extension_set.h
        ${LIB_HEADER_DIR}/google/protobuf/extension_set_inl.h
        ${LIB_HEADER_DIR}/google/protobuf/field_access_listener.h
        ${LIB_HEADER_DIR}/google/protobuf/field_mask.pb.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_enum_reflection.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_enum_util.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_message_bases.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_message_reflection.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_message_table_driven.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_message_tctable_decl.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_message_tctable_impl.h
        ${LIB_HEADER_DIR}/google/protobuf/generated_message_tctable_impl.inc
        ${LIB_HEADER_DIR}/google/protobuf/generated_message_util.h
        ${LIB_HEADER_DIR}/google/protobuf/has_bits.h
        ${LIB_HEADER_DIR}/google/protobuf/implicit_weak_message.h
        ${LIB_HEADER_DIR}/google/protobuf/inlined_string_field.h
        ${LIB_HEADER_DIR}/google/protobuf/io
        ${LIB_HEADER_DIR}/google/protobuf/io/coded_stream.h
        ${LIB_HEADER_DIR}/google/protobuf/io/io_win32.h
        ${LIB_HEADER_DIR}/google/protobuf/io/package_info.h
        ${LIB_HEADER_DIR}/google/protobuf/io/strtod.h
        ${LIB_HEADER_DIR}/google/protobuf/io/tokenizer.h
        ${LIB_HEADER_DIR}/google/protobuf/io/zero_copy_stream.h
        ${LIB_HEADER_DIR}/google/protobuf/io/zero_copy_stream_impl.h
        ${LIB_HEADER_DIR}/google/protobuf/io/zero_copy_stream_impl_lite.h
        ${LIB_HEADER_DIR}/google/protobuf/map.h
        ${LIB_HEADER_DIR}/google/protobuf/map_entry.h
        ${LIB_HEADER_DIR}/google/protobuf/map_entry_lite.h
        ${LIB_HEADER_DIR}/google/protobuf/map_field.h
        ${LIB_HEADER_DIR}/google/protobuf/map_field_inl.h
        ${LIB_HEADER_DIR}/google/protobuf/map_field_lite.h
        ${LIB_HEADER_DIR}/google/protobuf/map_type_handler.h
        ${LIB_HEADER_DIR}/google/protobuf/message.h
        ${LIB_HEADER_DIR}/google/protobuf/message_lite.h
        ${LIB_HEADER_DIR}/google/protobuf/metadata.h
        ${LIB_HEADER_DIR}/google/protobuf/metadata_lite.h
        ${LIB_HEADER_DIR}/google/protobuf/parse_context.h
        ${LIB_HEADER_DIR}/google/protobuf/port.h
        ${LIB_HEADER_DIR}/google/protobuf/port_def.inc
        ${LIB_HEADER_DIR}/google/protobuf/port_undef.inc
        ${LIB_HEADER_DIR}/google/protobuf/reflection.h
        ${LIB_HEADER_DIR}/google/protobuf/reflection_internal.h
        ${LIB_HEADER_DIR}/google/protobuf/reflection_ops.h
        ${LIB_HEADER_DIR}/google/protobuf/repeated_field.h
        ${LIB_HEADER_DIR}/google/protobuf/repeated_ptr_field.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs
        ${LIB_HEADER_DIR}/google/protobuf/stubs/bytestream.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/callback.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/casts.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/common.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/hash.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/int128.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/logging.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/macros.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/map_util.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/mutex.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/once.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/platform_macros.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/port.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/status.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/stl_util.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/stringpiece.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/stringprintf.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/strutil.h
        ${LIB_HEADER_DIR}/google/protobuf/stubs/substitute.h
        ${LIB_HEADER_DIR}/google/protobuf/text_format.h
        ${LIB_HEADER_DIR}/google/protobuf/unknown_field_set.h
        ${LIB_HEADER_DIR}/google/protobuf/wire_format.h
        ${LIB_HEADER_DIR}/google/protobuf/wire_format_lite.h		
    )	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/google/protobuf/any.cc
        ${LIB_SOURCE_DIR}/google/protobuf/any_lite.cc
        ${LIB_SOURCE_DIR}/google/protobuf/arena.cc
        ${LIB_SOURCE_DIR}/google/protobuf/arenastring.cc
        ${LIB_SOURCE_DIR}/google/protobuf/descriptor.cc
        ${LIB_SOURCE_DIR}/google/protobuf/descriptor.pb.cc
        ${LIB_SOURCE_DIR}/google/protobuf/descriptor_database.cc
        ${LIB_SOURCE_DIR}/google/protobuf/dynamic_message.cc
        ${LIB_SOURCE_DIR}/google/protobuf/extension_set.cc
        ${LIB_SOURCE_DIR}/google/protobuf/extension_set_heavy.cc
        ${LIB_SOURCE_DIR}/google/protobuf/generated_message_reflection.cc
        ${LIB_SOURCE_DIR}/google/protobuf/generated_message_util.cc
        ${LIB_SOURCE_DIR}/google/protobuf/implicit_weak_message.cc
        ${LIB_SOURCE_DIR}/google/protobuf/io
        ${LIB_SOURCE_DIR}/google/protobuf/io/coded_stream.cc
        ${LIB_SOURCE_DIR}/google/protobuf/io/io_win32.cc
        ${LIB_SOURCE_DIR}/google/protobuf/io/strtod.cc
        ${LIB_SOURCE_DIR}/google/protobuf/io/tokenizer.cc
        ${LIB_SOURCE_DIR}/google/protobuf/io/zero_copy_stream.cc
        ${LIB_SOURCE_DIR}/google/protobuf/io/zero_copy_stream_impl.cc
        ${LIB_SOURCE_DIR}/google/protobuf/io/zero_copy_stream_impl_lite.cc
        ${LIB_SOURCE_DIR}/google/protobuf/map.cc
        ${LIB_SOURCE_DIR}/google/protobuf/map_field.cc
        ${LIB_SOURCE_DIR}/google/protobuf/message.cc
        ${LIB_SOURCE_DIR}/google/protobuf/message_lite.cc
        ${LIB_SOURCE_DIR}/google/protobuf/parse_context.cc
        ${LIB_SOURCE_DIR}/google/protobuf/reflection_ops.cc
        ${LIB_SOURCE_DIR}/google/protobuf/repeated_field.cc
        ${LIB_SOURCE_DIR}/google/protobuf/repeated_ptr_field.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/bytestream.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/common.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/int128.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/status.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/stringpiece.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/stringprintf.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/structurally_valid.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/strutil.cc
        ${LIB_SOURCE_DIR}/google/protobuf/stubs/substitute.cc
        ${LIB_SOURCE_DIR}/google/protobuf/text_format.cc
        ${LIB_SOURCE_DIR}/google/protobuf/unknown_field_set.cc
        ${LIB_SOURCE_DIR}/google/protobuf/wire_format.cc
        ${LIB_SOURCE_DIR}/google/protobuf/wire_format_lite.cc		
    )
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
		# target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/libjpeg)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
	# target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GMP_MINI=1)
	
    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
	# target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
	# target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)
		
	# check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

	# target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)
    
	# target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

endif()

# add_subdirectory(test)
