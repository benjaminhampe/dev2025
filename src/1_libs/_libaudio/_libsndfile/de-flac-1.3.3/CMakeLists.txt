set(LIB_NAME de_flac) # We dont like to clash with system libs.
project(${LIB_NAME}-dev-1.3.3 C) #  ASM_NASM

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(FLAC_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/FLAC)
    set(FLAC_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/FLAC)
    set(FLAC_HEADERS
        ${FLAC_HEADER_DIR}/config.h
        ${FLAC_HEADER_DIR}/all.h
        ${FLAC_HEADER_DIR}/assert.h
        ${FLAC_HEADER_DIR}/callback.h
        ${FLAC_HEADER_DIR}/export.h
        ${FLAC_HEADER_DIR}/format.h
        ${FLAC_HEADER_DIR}/metadata.h
        ${FLAC_HEADER_DIR}/ordinals.h
        ${FLAC_HEADER_DIR}/stream_decoder.h
        ${FLAC_HEADER_DIR}/stream_encoder.h
    )
    set(FLAC_SOURCES
        ${FLAC_HEADER_DIR}/share/alloc.h
        ${FLAC_HEADER_DIR}/share/compat.h
        ${FLAC_HEADER_DIR}/share/endswap.h
        ${FLAC_HEADER_DIR}/share/getopt.h
        ${FLAC_HEADER_DIR}/share/grabbag.h
        ${FLAC_HEADER_DIR}/share/macros.h
        ${FLAC_HEADER_DIR}/share/private.h
        ${FLAC_HEADER_DIR}/share/replaygain_analysis.h
        ${FLAC_HEADER_DIR}/share/replaygain_synthesis.h
        ${FLAC_HEADER_DIR}/share/safe_str.h
        ${FLAC_HEADER_DIR}/share/utf8.h
        ${FLAC_HEADER_DIR}/share/grabbag/cuesheet.h
        ${FLAC_HEADER_DIR}/share/grabbag/file.h
        ${FLAC_HEADER_DIR}/share/grabbag/picture.h
        ${FLAC_HEADER_DIR}/share/grabbag/replaygain.h
        ${FLAC_HEADER_DIR}/share/grabbag/seektable.h
        # share
        ${FLAC_SOURCE_DIR}/share/getopt/getopt.c
        ${FLAC_SOURCE_DIR}/share/getopt/getopt1.c
        ${FLAC_SOURCE_DIR}/share/grabbag/alloc.c
        ${FLAC_SOURCE_DIR}/share/grabbag/cuesheet.c
        ${FLAC_SOURCE_DIR}/share/grabbag/file.c
        ${FLAC_SOURCE_DIR}/share/grabbag/picture.c
        ${FLAC_SOURCE_DIR}/share/grabbag/replaygain.c
        ${FLAC_SOURCE_DIR}/share/grabbag/seektable.c
        ${FLAC_SOURCE_DIR}/share/grabbag/snprintf.c
        ${FLAC_SOURCE_DIR}/share/replaygain_analysis/replaygain_analysis.c
        ${FLAC_SOURCE_DIR}/share/replaygain_synthesis/replaygain_synthesis.c
        ${FLAC_SOURCE_DIR}/share/utf8/charmaps.h
        ${FLAC_SOURCE_DIR}/share/utf8/charset.c
        ${FLAC_SOURCE_DIR}/share/utf8/charset.h
        # ${FLAC_SOURCE_DIR}/share/utf8/charset_test.c
        ${FLAC_SOURCE_DIR}/share/utf8/iconvert.h
        ${FLAC_SOURCE_DIR}/share/utf8/iconvert.c
        # ${FLAC_SOURCE_DIR}/share/utf8/makemap.c # app
        ${FLAC_SOURCE_DIR}/share/utf8/utf8.c
        # private headers
        ${FLAC_SOURCE_DIR}/include/private/all.h
        ${FLAC_SOURCE_DIR}/include/private/bitmath.h
        ${FLAC_SOURCE_DIR}/include/private/bitreader.h
        ${FLAC_SOURCE_DIR}/include/private/bitwriter.h
        ${FLAC_SOURCE_DIR}/include/private/cpu.h
        ${FLAC_SOURCE_DIR}/include/private/crc.h
        ${FLAC_SOURCE_DIR}/include/private/fixed.h
        ${FLAC_SOURCE_DIR}/include/private/float.h
        ${FLAC_SOURCE_DIR}/include/private/format.h
        ${FLAC_SOURCE_DIR}/include/private/lpc.h
        ${FLAC_SOURCE_DIR}/include/private/macros.h
        ${FLAC_SOURCE_DIR}/include/private/md5.h
        ${FLAC_SOURCE_DIR}/include/private/memory.h
        ${FLAC_SOURCE_DIR}/include/private/metadata.h
        ${FLAC_SOURCE_DIR}/include/private/ogg_decoder_aspect.h
        ${FLAC_SOURCE_DIR}/include/private/ogg_encoder_aspect.h
        ${FLAC_SOURCE_DIR}/include/private/ogg_helper.h
        ${FLAC_SOURCE_DIR}/include/private/ogg_mapping.h
        ${FLAC_SOURCE_DIR}/include/private/stream_encoder.h
        ${FLAC_SOURCE_DIR}/include/private/stream_encoder_framing.h
        ${FLAC_SOURCE_DIR}/include/private/window.h
        # protected headers
        ${FLAC_SOURCE_DIR}/include/protected/all.h
        ${FLAC_SOURCE_DIR}/include/protected/stream_decoder.h
        ${FLAC_SOURCE_DIR}/include/protected/stream_encoder.h
        # private sources
        ${FLAC_SOURCE_DIR}/bitmath.c
        ${FLAC_SOURCE_DIR}/bitreader.c
        ${FLAC_SOURCE_DIR}/bitwriter.c
        ${FLAC_SOURCE_DIR}/cpu.c
        ${FLAC_SOURCE_DIR}/crc.c
        ${FLAC_SOURCE_DIR}/fixed.c
        ${FLAC_SOURCE_DIR}/fixed_intrin_sse2.c
        ${FLAC_SOURCE_DIR}/fixed_intrin_ssse3.c
        ${FLAC_SOURCE_DIR}/float.c
        ${FLAC_SOURCE_DIR}/format.c
        ${FLAC_SOURCE_DIR}/lpc.c
        ${FLAC_SOURCE_DIR}/lpc_intrin_avx2.c
        ${FLAC_SOURCE_DIR}/lpc_intrin_sse.c
        ${FLAC_SOURCE_DIR}/lpc_intrin_sse2.c
        ${FLAC_SOURCE_DIR}/lpc_intrin_sse41.c
        ${FLAC_SOURCE_DIR}/lpc_intrin_vsx.c
        ${FLAC_SOURCE_DIR}/md5.c
        ${FLAC_SOURCE_DIR}/memory.c
        ${FLAC_SOURCE_DIR}/metadata_iterators.c
        ${FLAC_SOURCE_DIR}/metadata_object.c
        ${FLAC_SOURCE_DIR}/ogg_decoder_aspect.c
        ${FLAC_SOURCE_DIR}/ogg_encoder_aspect.c
        ${FLAC_SOURCE_DIR}/ogg_helper.c
        ${FLAC_SOURCE_DIR}/ogg_mapping.c
        ${FLAC_SOURCE_DIR}/stream_decoder.c
        ${FLAC_SOURCE_DIR}/stream_encoder.c
        ${FLAC_SOURCE_DIR}/stream_encoder_framing.c
        ${FLAC_SOURCE_DIR}/stream_encoder_intrin_avx2.c
        ${FLAC_SOURCE_DIR}/stream_encoder_intrin_sse2.c
        ${FLAC_SOURCE_DIR}/stream_encoder_intrin_ssse3.c
        ${FLAC_SOURCE_DIR}/window.c
    )

    if (WIN32)
        list(APPEND FLAC_HEADERS
            ${FLAC_HEADER_DIR}/share/win_utf8_io.h
            ${FLAC_HEADER_DIR}/share/windows_unicode_filenames.h)
        list(APPEND FLAC_SOURCES
            ${FLAC_SOURCE_DIR}/windows_unicode_filenames.c
            ${FLAC_SOURCE_DIR}/share/win_utf8_io/win_utf8_io.c)

        enable_language(RC)
        list(APPEND FLAC_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND FLAC_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        #target_compile_definitions(${LIB_NAME} PUBLIC FLAC__NO_DLL=1)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${FLAC_HEADERS} PRIVATE ${FLAC_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC "${FLAC_HEADER_DIR}/..") # for lib user
    target_include_directories(${LIB_NAME} PRIVATE "${FLAC_SOURCE_DIR}") # for lib sources
    target_include_directories(${LIB_NAME} PRIVATE "${FLAC_HEADER_DIR}") # for lib sources
    target_include_directories(${LIB_NAME} PRIVATE "${FLAC_SOURCE_DIR}/include/private/..") # for lib sources
    target_include_directories(${LIB_NAME} PRIVATE "${FLAC_SOURCE_DIR}/include/protected/..") # for lib sources

    #    set_source_files_properties(lpc.c lpc_intrin_sse.c lpc_intrin_sse2.c lpc_intrin_sse41.c
    #                               lpc_intrin_avx2.c
    #                               PROPERTIES COMPILE_FLAGS -Wno-implicit-fallthrough )
    #

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_FLAC=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_ogg)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough
                      -Wno-format
                      -Wno-format-extra-args
                      -Wno-sign-compare
                      -Wno-type-limits
        )
        
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
    
endif()

# add_subdirectory(test)
