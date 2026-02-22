set(LIB_NAME de_openjpeg)
project(${LIB_NAME}-2.5.3)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/openjp2/bio.h
        ${LIB_HEADER_DIR}/openjp2/cidx_manager.h
        ${LIB_HEADER_DIR}/openjp2/cio.h
        ${LIB_HEADER_DIR}/openjp2/dwt.h
        ${LIB_HEADER_DIR}/openjp2/event.h
        ${LIB_HEADER_DIR}/openjp2/function_list.h
        ${LIB_HEADER_DIR}/openjp2/image.h
        ${LIB_HEADER_DIR}/openjp2/indexbox_manager.h
        ${LIB_HEADER_DIR}/openjp2/invert.h
        ${LIB_HEADER_DIR}/openjp2/j2k.h
        ${LIB_HEADER_DIR}/openjp2/jp2.h
        ${LIB_HEADER_DIR}/openjp2/mct.h
        ${LIB_HEADER_DIR}/openjp2/mqc.h
        ${LIB_HEADER_DIR}/openjp2/mqc_inl.h
        ${LIB_HEADER_DIR}/openjp2/openjpeg.h
        ${LIB_HEADER_DIR}/openjp2/opj_clock.h
        ${LIB_HEADER_DIR}/openjp2/opj_codec.h
        ${LIB_HEADER_DIR}/openjp2/opj_common.h
        ${LIB_HEADER_DIR}/openjp2/opj_config.h
        ${LIB_HEADER_DIR}/openjp2/opj_config_private.h
        ${LIB_HEADER_DIR}/openjp2/opj_includes.h
        ${LIB_HEADER_DIR}/openjp2/opj_intmath.h
        ${LIB_HEADER_DIR}/openjp2/opj_inttypes.h
        ${LIB_HEADER_DIR}/openjp2/opj_malloc.h
        ${LIB_HEADER_DIR}/openjp2/opj_stdint.h
        ${LIB_HEADER_DIR}/openjp2/pi.h
        ${LIB_HEADER_DIR}/openjp2/sparse_array.h
        ${LIB_HEADER_DIR}/openjp2/t1.h
        ${LIB_HEADER_DIR}/openjp2/t1_ht_luts.h
        ${LIB_HEADER_DIR}/openjp2/t1_luts.h
        ${LIB_HEADER_DIR}/openjp2/t2.h
        ${LIB_HEADER_DIR}/openjp2/tcd.h
        ${LIB_HEADER_DIR}/openjp2/tgt.h
        ${LIB_HEADER_DIR}/openjp2/thread.h
        ${LIB_HEADER_DIR}/openjp2/tls_keys.h
    )
    set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/bench_dwt.c
        ${LIB_SOURCE_DIR}/bio.c
        # ${LIB_SOURCE_DIR}/cidx_manager.c
        ${LIB_SOURCE_DIR}/cio.c
        ${LIB_SOURCE_DIR}/dwt.c
        ${LIB_SOURCE_DIR}/event.c
        ${LIB_SOURCE_DIR}/function_list.c
        ${LIB_SOURCE_DIR}/ht_dec.c
        ${LIB_SOURCE_DIR}/image.c
        ${LIB_SOURCE_DIR}/invert.c
        ${LIB_SOURCE_DIR}/j2k.c
        ${LIB_SOURCE_DIR}/jp2.c
        ${LIB_SOURCE_DIR}/mct.c
        ${LIB_SOURCE_DIR}/mqc.c
        ${LIB_SOURCE_DIR}/openjpeg.c
        ${LIB_SOURCE_DIR}/opj_clock.c
        ${LIB_SOURCE_DIR}/opj_malloc.c
        # ${LIB_SOURCE_DIR}/phix_manager.c
        ${LIB_SOURCE_DIR}/pi.c
        # ${LIB_SOURCE_DIR}/ppix_manager.c
        ${LIB_SOURCE_DIR}/sparse_array.c
        ${LIB_SOURCE_DIR}/t1.c
        # ${LIB_SOURCE_DIR}/t1_generate_luts.c
        # ${LIB_SOURCE_DIR}/t1_ht_generate_luts.c
        ${LIB_SOURCE_DIR}/t2.c
        ${LIB_SOURCE_DIR}/tcd.c
        # ${LIB_SOURCE_DIR}/test_sparse_array.c
        ${LIB_SOURCE_DIR}/tgt.c
        # ${LIB_SOURCE_DIR}/thix_manager.c
        ${LIB_SOURCE_DIR}/thread.c
        # ${LIB_SOURCE_DIR}/tpix_manager.c
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
        target_compile_definitions(${LIB_NAME} PRIVATE OPJ_EXPORTS=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        target_compile_definitions(${LIB_NAME} PUBLIC OPJ_STATIC=1)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/openjp2)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_OPENJPEG=1)
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
