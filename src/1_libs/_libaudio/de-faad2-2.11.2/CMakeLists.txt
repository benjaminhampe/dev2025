set(LIB_NAME de_faad2)
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # option(FAAD2_BUILD_MP4FF "Build mp4ff demuxer" OFF)
    # option(FAAD2_ENABLE_DRM "Enable DRM AAC decoding" OFF)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/faad.h
        ${LIB_HEADER_DIR}/neaacdec.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/bits.c
        ${LIB_SOURCE_DIR}/cfft.c
        ${LIB_SOURCE_DIR}/common.c
        ${LIB_SOURCE_DIR}/decoder.c
        ${LIB_SOURCE_DIR}/drc.c
        ${LIB_SOURCE_DIR}/drm_dec.c
        ${LIB_SOURCE_DIR}/error.c
        # ${LIB_SOURCE_DIR}/faad2.pc.in
        ${LIB_SOURCE_DIR}/filtbank.c
        ${LIB_SOURCE_DIR}/hcr.c
        ${LIB_SOURCE_DIR}/huffman.c
        ${LIB_SOURCE_DIR}/ic_predict.c
        ${LIB_SOURCE_DIR}/is.c
        ${LIB_SOURCE_DIR}/lt_predict.c
        ${LIB_SOURCE_DIR}/mdct.c
        ${LIB_SOURCE_DIR}/mp4.c
        ${LIB_SOURCE_DIR}/ms.c
        ${LIB_SOURCE_DIR}/output.c
        ${LIB_SOURCE_DIR}/pns.c
        ${LIB_SOURCE_DIR}/ps_dec.c
        ${LIB_SOURCE_DIR}/ps_syntax.c
        ${LIB_SOURCE_DIR}/pulse.c
        ${LIB_SOURCE_DIR}/rvlc.c
        ${LIB_SOURCE_DIR}/sbr_dct.c
        ${LIB_SOURCE_DIR}/sbr_dec.c
        ${LIB_SOURCE_DIR}/sbr_e_nf.c
        ${LIB_SOURCE_DIR}/sbr_fbt.c
        ${LIB_SOURCE_DIR}/sbr_hfadj.c
        ${LIB_SOURCE_DIR}/sbr_hfgen.c
        ${LIB_SOURCE_DIR}/sbr_huff.c
        ${LIB_SOURCE_DIR}/sbr_qmf.c
        ${LIB_SOURCE_DIR}/sbr_syntax.c
        ${LIB_SOURCE_DIR}/sbr_tf_grid.c
        ${LIB_SOURCE_DIR}/specrec.c
        ${LIB_SOURCE_DIR}/ssr.c
        ${LIB_SOURCE_DIR}/ssr_fb.c
        ${LIB_SOURCE_DIR}/ssr_ipqf.c
        ${LIB_SOURCE_DIR}/syntax.c
        ${LIB_SOURCE_DIR}/tns.c
    )

    set(LIB_SOURCES_HDRS
        ${LIB_SOURCE_DIR}/analysis.h
        ${LIB_SOURCE_DIR}/bits.h
        ${LIB_SOURCE_DIR}/cfft.h
        ${LIB_SOURCE_DIR}/cfft_tab.h
        ${LIB_SOURCE_DIR}/codebook/hcb.h
        ${LIB_SOURCE_DIR}/codebook/hcb_1.h
        ${LIB_SOURCE_DIR}/codebook/hcb_10.h
        ${LIB_SOURCE_DIR}/codebook/hcb_11.h
        ${LIB_SOURCE_DIR}/codebook/hcb_2.h
        ${LIB_SOURCE_DIR}/codebook/hcb_3.h
        ${LIB_SOURCE_DIR}/codebook/hcb_4.h
        ${LIB_SOURCE_DIR}/codebook/hcb_5.h
        ${LIB_SOURCE_DIR}/codebook/hcb_6.h
        ${LIB_SOURCE_DIR}/codebook/hcb_7.h
        ${LIB_SOURCE_DIR}/codebook/hcb_8.h
        ${LIB_SOURCE_DIR}/codebook/hcb_9.h
        ${LIB_SOURCE_DIR}/codebook/hcb_sf.h
        ${LIB_SOURCE_DIR}/common.h
        ${LIB_SOURCE_DIR}/drc.h
        ${LIB_SOURCE_DIR}/drm_dec.h
        ${LIB_SOURCE_DIR}/error.h
        ${LIB_SOURCE_DIR}/filtbank.h
        ${LIB_SOURCE_DIR}/fixed.h
        ${LIB_SOURCE_DIR}/huffman.h
        ${LIB_SOURCE_DIR}/ic_predict.h
        ${LIB_SOURCE_DIR}/iq_table.h
        ${LIB_SOURCE_DIR}/is.h
        ${LIB_SOURCE_DIR}/kbd_win.h
        ${LIB_SOURCE_DIR}/lt_predict.h
        ${LIB_SOURCE_DIR}/mdct.h
        ${LIB_SOURCE_DIR}/mdct_tab.h
        ${LIB_SOURCE_DIR}/mp4.h
        ${LIB_SOURCE_DIR}/ms.h
        ${LIB_SOURCE_DIR}/output.h
        ${LIB_SOURCE_DIR}/pns.h
        ${LIB_SOURCE_DIR}/ps_dec.h
        ${LIB_SOURCE_DIR}/ps_tables.h
        ${LIB_SOURCE_DIR}/pulse.h
        ${LIB_SOURCE_DIR}/rvlc.h
        ${LIB_SOURCE_DIR}/sbr_dct.h
        ${LIB_SOURCE_DIR}/sbr_dec.h
        ${LIB_SOURCE_DIR}/sbr_e_nf.h
        ${LIB_SOURCE_DIR}/sbr_fbt.h
        ${LIB_SOURCE_DIR}/sbr_hfadj.h
        ${LIB_SOURCE_DIR}/sbr_hfgen.h
        ${LIB_SOURCE_DIR}/sbr_huff.h
        ${LIB_SOURCE_DIR}/sbr_noise.h
        ${LIB_SOURCE_DIR}/sbr_qmf.h
        ${LIB_SOURCE_DIR}/sbr_qmf_c.h
        ${LIB_SOURCE_DIR}/sbr_syntax.h
        ${LIB_SOURCE_DIR}/sbr_tf_grid.h
        ${LIB_SOURCE_DIR}/sine_win.h
        ${LIB_SOURCE_DIR}/specrec.h
        ${LIB_SOURCE_DIR}/ssr.h
        ${LIB_SOURCE_DIR}/ssr_fb.h
        ${LIB_SOURCE_DIR}/ssr_ipqf.h
        ${LIB_SOURCE_DIR}/ssr_win.h
        ${LIB_SOURCE_DIR}/structs.h
        ${LIB_SOURCE_DIR}/syntax.h
        ${LIB_SOURCE_DIR}/tns.h
    )

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")

        # if(WIN32)
            # enable_language(RC)
            # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
            # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # endif()
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        target_compile_definitions(${LIB_NAME} PRIVATE FAAD2_STATIC=1)
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES} ${LIB_SOURCES_HDRS})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_FAAD2=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_SYS_TYPES_H=1)  #include <sys/types.h>
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_SYS_STAT_H=1)   #include <sys/stat.h>
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_STRING_H=1)     #include <string.h>
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_STRINGS_H=1)    #include <strings.h>
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_INTTYPES_H=1)   #include <inttypes.h>
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_STDINT_H=1)     #include <stdint.h>

    target_compile_definitions(${LIB_NAME} PRIVATE SBR_DEC=1)
    target_compile_definitions(${LIB_NAME} PRIVATE PS_DEC=1)
    target_compile_definitions(${LIB_NAME} PRIVATE LTP_DEC=1)
    target_compile_definitions(${LIB_NAME} PRIVATE MAIN_DEC=1)
    target_compile_definitions(${LIB_NAME} PRIVATE LD_DEC=1)
    target_compile_definitions(${LIB_NAME} PRIVATE ERROR_RESILIENCE=1)
    target_compile_definitions(${LIB_NAME} PRIVATE PACKAGE_VERSION="2.11.2")
    # if(FAAD2_ENABLE_DRM)
        # target_compile_definitions(${LIB_NAME} PUBLIC DRM)
    # endif()


endif()
