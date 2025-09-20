set(LIB_NAME de_archive)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/archive/archive.h
        ${LIB_HEADER_DIR}/archive/archive_entry.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/archive_acl.c
        ${LIB_SOURCE_DIR}/archive_acl_private.h
        ${LIB_SOURCE_DIR}/archive_check_magic.c
        ${LIB_SOURCE_DIR}/archive_cmdline.c
        ${LIB_SOURCE_DIR}/archive_cmdline_private.h
        ${LIB_SOURCE_DIR}/archive_crc32.h
        ${LIB_SOURCE_DIR}/archive_cryptor.c
        ${LIB_SOURCE_DIR}/archive_cryptor_private.h
        ${LIB_SOURCE_DIR}/archive_digest.c
        ${LIB_SOURCE_DIR}/archive_digest_private.h
        ${LIB_SOURCE_DIR}/archive_endian.h
        ${LIB_SOURCE_DIR}/archive_entry.c
        ${LIB_SOURCE_DIR}/archive_entry_copy_stat.c
        ${LIB_SOURCE_DIR}/archive_entry_link_resolver.c
        ${LIB_SOURCE_DIR}/archive_entry_locale.h
        ${LIB_SOURCE_DIR}/archive_entry_private.h
        ${LIB_SOURCE_DIR}/archive_entry_sparse.c
        ${LIB_SOURCE_DIR}/archive_entry_stat.c
        ${LIB_SOURCE_DIR}/archive_entry_strmode.c
        ${LIB_SOURCE_DIR}/archive_entry_xattr.c
        ${LIB_SOURCE_DIR}/archive_hmac.c
        ${LIB_SOURCE_DIR}/archive_hmac_private.h
        ${LIB_SOURCE_DIR}/archive_match.c
        ${LIB_SOURCE_DIR}/archive_openssl_evp_private.h
        ${LIB_SOURCE_DIR}/archive_openssl_hmac_private.h
        ${LIB_SOURCE_DIR}/archive_options.c
        ${LIB_SOURCE_DIR}/archive_options_private.h
        ${LIB_SOURCE_DIR}/archive_pack_dev.h
        ${LIB_SOURCE_DIR}/archive_pack_dev.c
        ${LIB_SOURCE_DIR}/archive_parse_date.c
        ${LIB_SOURCE_DIR}/archive_pathmatch.c
        ${LIB_SOURCE_DIR}/archive_pathmatch.h
        ${LIB_SOURCE_DIR}/archive_platform.h
        ${LIB_SOURCE_DIR}/archive_platform_acl.h
        ${LIB_SOURCE_DIR}/archive_platform_xattr.h
        ${LIB_SOURCE_DIR}/archive_ppmd_private.h
        ${LIB_SOURCE_DIR}/archive_ppmd8.c
        ${LIB_SOURCE_DIR}/archive_ppmd8_private.h
        ${LIB_SOURCE_DIR}/archive_ppmd7.c
        ${LIB_SOURCE_DIR}/archive_ppmd7_private.h
        ${LIB_SOURCE_DIR}/archive_private.h
        ${LIB_SOURCE_DIR}/archive_random.c
        ${LIB_SOURCE_DIR}/archive_random_private.h
        ${LIB_SOURCE_DIR}/archive_rb.c
        ${LIB_SOURCE_DIR}/archive_rb.h
        ${LIB_SOURCE_DIR}/archive_read.c
        ${LIB_SOURCE_DIR}/archive_read_add_passphrase.c
        ${LIB_SOURCE_DIR}/archive_read_append_filter.c
        ${LIB_SOURCE_DIR}/archive_read_data_into_fd.c
        ${LIB_SOURCE_DIR}/archive_read_disk_entry_from_file.c
        ${LIB_SOURCE_DIR}/archive_read_disk_posix.c
        ${LIB_SOURCE_DIR}/archive_read_disk_private.h
        ${LIB_SOURCE_DIR}/archive_read_disk_set_standard_lookup.c
        ${LIB_SOURCE_DIR}/archive_read_extract.c
        ${LIB_SOURCE_DIR}/archive_read_extract2.c
        ${LIB_SOURCE_DIR}/archive_read_open_fd.c
        ${LIB_SOURCE_DIR}/archive_read_open_file.c
        ${LIB_SOURCE_DIR}/archive_read_open_filename.c
        ${LIB_SOURCE_DIR}/archive_read_open_memory.c
        ${LIB_SOURCE_DIR}/archive_read_private.h
        ${LIB_SOURCE_DIR}/archive_read_set_format.c
        ${LIB_SOURCE_DIR}/archive_read_set_options.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_all.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_by_code.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_bzip2.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_compress.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_gzip.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_grzip.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_lrzip.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_lz4.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_lzop.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_none.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_program.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_rpm.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_uu.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_xz.c
        ${LIB_SOURCE_DIR}/archive_read_support_filter_zstd.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_7zip.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_all.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_ar.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_by_code.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_cab.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_cpio.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_empty.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_iso9660.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_lha.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_mtree.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_rar.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_rar5.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_raw.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_tar.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_warc.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_xar.c
        ${LIB_SOURCE_DIR}/archive_read_support_format_zip.c
        ${LIB_SOURCE_DIR}/archive_string.c
        ${LIB_SOURCE_DIR}/archive_string.h
        ${LIB_SOURCE_DIR}/archive_string_composition.h
        ${LIB_SOURCE_DIR}/archive_string_sprintf.c
        ${LIB_SOURCE_DIR}/archive_time.c
        ${LIB_SOURCE_DIR}/archive_time_private.h
        ${LIB_SOURCE_DIR}/archive_util.c
        ${LIB_SOURCE_DIR}/archive_version_details.c
        ${LIB_SOURCE_DIR}/archive_virtual.c
        ${LIB_SOURCE_DIR}/archive_write.c
        ${LIB_SOURCE_DIR}/archive_write_disk_posix.c
        ${LIB_SOURCE_DIR}/archive_write_disk_private.h
        ${LIB_SOURCE_DIR}/archive_write_disk_set_standard_lookup.c
        ${LIB_SOURCE_DIR}/archive_write_private.h
        ${LIB_SOURCE_DIR}/archive_write_open_fd.c
        ${LIB_SOURCE_DIR}/archive_write_open_file.c
        ${LIB_SOURCE_DIR}/archive_write_open_filename.c
        ${LIB_SOURCE_DIR}/archive_write_open_memory.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_b64encode.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_by_name.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_bzip2.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_compress.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_grzip.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_gzip.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_lrzip.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_lz4.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_lzop.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_none.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_program.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_uuencode.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_xz.c
        ${LIB_SOURCE_DIR}/archive_write_add_filter_zstd.c
        ${LIB_SOURCE_DIR}/archive_write_set_format.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_7zip.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_ar.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_by_name.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_cpio.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_cpio_binary.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_cpio_newc.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_cpio_odc.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_filter_by_ext.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_gnutar.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_iso9660.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_mtree.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_pax.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_private.h
        ${LIB_SOURCE_DIR}/archive_write_set_format_raw.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_shar.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_ustar.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_v7tar.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_warc.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_xar.c
        ${LIB_SOURCE_DIR}/archive_write_set_format_zip.c
        ${LIB_SOURCE_DIR}/archive_write_set_options.c
        ${LIB_SOURCE_DIR}/archive_write_set_passphrase.c
        ${LIB_SOURCE_DIR}/archive_xxhash.h
        ${LIB_SOURCE_DIR}/filter_fork_posix.c
        ${LIB_SOURCE_DIR}/filter_fork.h
        ${LIB_SOURCE_DIR}/xxhash.c
    )

# if (ARCHIVE_BLAKE2)
    list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/archive_blake2sp_ref.c)
    list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/archive_blake2s_ref.c)
# endif()

if (WIN32 AND NOT CYGWIN)
    list(APPEND LIB_SOURCES 
        ${LIB_SOURCE_DIR}/archive_entry_copy_bhfi.c
        ${LIB_SOURCE_DIR}/archive_read_disk_windows.c
        ${LIB_SOURCE_DIR}/archive_windows.c
        ${LIB_SOURCE_DIR}/archive_windows.h
        ${LIB_SOURCE_DIR}/archive_write_disk_windows.c
        ${LIB_SOURCE_DIR}/filter_fork_windows.c
    )
endif()

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        target_compile_definitions(${LIB_NAME} PUBLIC LIBARCHIVE_STATIC=1)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/archive)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBARCHIVE=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    target_link_libraries(${LIB_NAME} PRIVATE de_zlib)
	target_link_libraries(${LIB_NAME} PRIVATE de_bzip2)
    target_link_libraries(${LIB_NAME} PRIVATE de_b2)    # blake
    target_link_libraries(${LIB_NAME} PRIVATE de_cpio)
    target_link_libraries(${LIB_NAME} PRIVATE de_xz_static) # -> TODO: integrate sources
    target_link_libraries(${LIB_NAME} PRIVATE de_iconv)
    target_link_libraries(${LIB_NAME} PRIVATE de_xml2)
    target_link_libraries(${LIB_NAME} PRIVATE de_mbedtls_static) # -> TODO: integrate sources
    target_link_libraries(${LIB_NAME} PRIVATE de_zstd)
    target_link_libraries(${LIB_NAME} PRIVATE de_lz4)
    target_link_libraries(${LIB_NAME} PRIVATE de_lzo2)
    target_link_libraries(${LIB_NAME} PRIVATE de_archive_fe)
    target_link_libraries(${LIB_NAME} PRIVATE de_pcre2)
    
    target_link_libraries(${LIB_NAME} PRIVATE bcrypt) # Windows CNG API, cryptographic op's MD5, SHA1, SHA256, etc.

    if(MSVC)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-maybe-uninitialized)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
    endif()
    
    # file(COPY ${LIB_HEADERS} DESTINATION ${LIB_INCLUDE_DIR}) # For the SDK

endif()

# add_subdirectory(test)
