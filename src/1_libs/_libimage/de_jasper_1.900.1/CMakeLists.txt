set(LIB_NAME de_jasper) # JPEG2000 files .jp2, .jpc, .j2k
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/jasper/jasper.h
        ${LIB_HEADER_DIR}/jasper/jas_cm.h
        ${LIB_HEADER_DIR}/jasper/jas_config.h
        ${LIB_HEADER_DIR}/jasper/jas_config2.h
        ${LIB_HEADER_DIR}/jasper/jas_debug.h
        ${LIB_HEADER_DIR}/jasper/jas_fix.h
        ${LIB_HEADER_DIR}/jasper/jas_getopt.h
        ${LIB_HEADER_DIR}/jasper/jas_icc.h
        ${LIB_HEADER_DIR}/jasper/jas_image.h
        ${LIB_HEADER_DIR}/jasper/jas_init.h
        ${LIB_HEADER_DIR}/jasper/jas_malloc.h
        ${LIB_HEADER_DIR}/jasper/jas_math.h
        ${LIB_HEADER_DIR}/jasper/jas_seq.h
        ${LIB_HEADER_DIR}/jasper/jas_stream.h
        ${LIB_HEADER_DIR}/jasper/jas_string.h
        ${LIB_HEADER_DIR}/jasper/jas_tmr.h
        ${LIB_HEADER_DIR}/jasper/jas_tvp.h
        ${LIB_HEADER_DIR}/jasper/jas_types.h
        ${LIB_HEADER_DIR}/jasper/jas_version.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/jas_cm.c
        ${LIB_SOURCE_DIR}/jas_debug.c
        ${LIB_SOURCE_DIR}/jas_getopt.c
        ${LIB_SOURCE_DIR}/jas_icc.c
        ${LIB_SOURCE_DIR}/jas_iccdata.c
        ${LIB_SOURCE_DIR}/jas_image.c
        ${LIB_SOURCE_DIR}/jas_init.c
        ${LIB_SOURCE_DIR}/jas_malloc.c
        ${LIB_SOURCE_DIR}/jas_seq.c
        ${LIB_SOURCE_DIR}/jas_stream.c
        ${LIB_SOURCE_DIR}/jas_string.c
        ${LIB_SOURCE_DIR}/jas_tmr.c
        ${LIB_SOURCE_DIR}/jas_tvp.c
        ${LIB_SOURCE_DIR}/jas_version.c
        ${LIB_SOURCE_DIR}/jp2_cod.c
        ${LIB_SOURCE_DIR}/jp2_cod.h
        ${LIB_SOURCE_DIR}/jp2_dec.c
        ${LIB_SOURCE_DIR}/jp2_dec.h
        ${LIB_SOURCE_DIR}/jp2_enc.c
        ${LIB_SOURCE_DIR}/jpc_bs.c
        ${LIB_SOURCE_DIR}/jpc_bs.h
        ${LIB_SOURCE_DIR}/jpc_cod.h
        ${LIB_SOURCE_DIR}/jpc_cs.c
        ${LIB_SOURCE_DIR}/jpc_cs.h
        ${LIB_SOURCE_DIR}/jpc_dec.c
        ${LIB_SOURCE_DIR}/jpc_dec.h
        ${LIB_SOURCE_DIR}/jpc_enc.c
        ${LIB_SOURCE_DIR}/jpc_enc.h
        ${LIB_SOURCE_DIR}/jpc_fix.h
        ${LIB_SOURCE_DIR}/jpc_flt.h
        ${LIB_SOURCE_DIR}/jpc_math.c
        ${LIB_SOURCE_DIR}/jpc_math.h
        ${LIB_SOURCE_DIR}/jpc_mct.c
        ${LIB_SOURCE_DIR}/jpc_mct.h
        ${LIB_SOURCE_DIR}/jpc_mqcod.c
        ${LIB_SOURCE_DIR}/jpc_mqcod.h
        ${LIB_SOURCE_DIR}/jpc_mqdec.c
        ${LIB_SOURCE_DIR}/jpc_mqdec.h
        ${LIB_SOURCE_DIR}/jpc_mqenc.c
        ${LIB_SOURCE_DIR}/jpc_mqenc.h
        ${LIB_SOURCE_DIR}/jpc_qmfb.c
        ${LIB_SOURCE_DIR}/jpc_qmfb.h
        ${LIB_SOURCE_DIR}/jpc_t1cod.c
        ${LIB_SOURCE_DIR}/jpc_t1cod.h
        ${LIB_SOURCE_DIR}/jpc_t1dec.c
        ${LIB_SOURCE_DIR}/jpc_t1dec.h
        ${LIB_SOURCE_DIR}/jpc_t1enc.c
        ${LIB_SOURCE_DIR}/jpc_t1enc.h
        ${LIB_SOURCE_DIR}/jpc_t2cod.c
        ${LIB_SOURCE_DIR}/jpc_t2cod.h
        ${LIB_SOURCE_DIR}/jpc_t2dec.c
        ${LIB_SOURCE_DIR}/jpc_t2dec.h
        ${LIB_SOURCE_DIR}/jpc_t2enc.c
        ${LIB_SOURCE_DIR}/jpc_t2enc.h
        ${LIB_SOURCE_DIR}/jpc_tagtree.c
        ${LIB_SOURCE_DIR}/jpc_tagtree.h
        ${LIB_SOURCE_DIR}/jpc_tsfb.c
        ${LIB_SOURCE_DIR}/jpc_tsfb.h
        ${LIB_SOURCE_DIR}/jpc_util.c
        ${LIB_SOURCE_DIR}/jpc_util.h
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/jasper)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_JASPER=1)
    target_compile_definitions(${LIB_NAME} PUBLIC WITH_JASPER=1)

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
