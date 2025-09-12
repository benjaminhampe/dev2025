set(LIB_NAME de_lame) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/lame)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/libmp3lame)

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/bitstream.c
        # // depcomp*
        ${LIB_SOURCE_DIR}/encoder.c
        ${LIB_SOURCE_DIR}/fft.c
        ${LIB_SOURCE_DIR}/gain_analysis.c
        ${LIB_SOURCE_DIR}/id3tag.c
        ${LIB_SOURCE_DIR}/lame.c
        ${LIB_SOURCE_DIR}/mpglib_interface.c
        ${LIB_SOURCE_DIR}/newmdct.c
        ${LIB_SOURCE_DIR}/presets.c
        ${LIB_SOURCE_DIR}/psymodel.c
        ${LIB_SOURCE_DIR}/quantize.c
        ${LIB_SOURCE_DIR}/quantize_pvt.c
        ${LIB_SOURCE_DIR}/reservoir.c
        ${LIB_SOURCE_DIR}/set_get.c
        ${LIB_SOURCE_DIR}/tables.c
        ${LIB_SOURCE_DIR}/takehiro.c
        ${LIB_SOURCE_DIR}/util.c
        ${LIB_SOURCE_DIR}/vbrquantize.c
        ${LIB_SOURCE_DIR}/VbrTag.c
        ${LIB_SOURCE_DIR}/version.c
        # ${LIB_SOURCE_DIR}/vector/lame_intrin.h
        ${LIB_SOURCE_DIR}/vector/xmm_quantize_sub.c
    )

    # enable_language(NASM)
    enable_language(ASM_NASM)

    set(LIB_SOURCES_ASM
        ${LIB_SOURCE_DIR}/i386/choose_table.nas
        ${LIB_SOURCE_DIR}/i386/cpu_feat.nas
        ${LIB_SOURCE_DIR}/i386/fft.nas
        ${LIB_SOURCE_DIR}/i386/fft3dn.nas
        ${LIB_SOURCE_DIR}/i386/fftfpu.nas
        ${LIB_SOURCE_DIR}/i386/fftsse.nas
        ${LIB_SOURCE_DIR}/i386/ffttbl.nas
        ${LIB_SOURCE_DIR}/i386/nasm.h
        ${LIB_SOURCE_DIR}/i386/scalar.nas
    )

    if(WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/lame.rc ${LIB_SOURCE_DIR}/logoe.ico )
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..) # Provide <lame/lame.h>

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}) # for src <config.h>
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}) # for src

    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} 
            PRIVATE 
            -Wno-sign-compare
            -Wno-unused-parameter
            -Wno-unused-variable
            -Wno-unknown-pragmas
            -Wno-implicit-fallthrough
            -Wno-sign-compare
            -Wno-unused-const-variable
            -Wno-unused-but-set-variable
            -Wno-shift-negative-value)
    endif()
    
    # target_link_libraries(${LIB_NAME} PRIVATE de_sndfile)

endif()

# add_subdirectory(test)
