set(LIB_NAME de_lame_3100) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/lame/lame.h)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/config.h

        ${LIB_SOURCE_DIR}/bitstream.c
        # // depcomp*
        ${LIB_SOURCE_DIR}/encoder.c
        ${LIB_SOURCE_DIR}/fft.c
        ${LIB_SOURCE_DIR}/gain_analysis.c
        ${LIB_SOURCE_DIR}/id3tag.c
        ${LIB_SOURCE_DIR}/lame.c
        # ${LIB_SOURCE_DIR}/mpglib_interface.c
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

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/lame)

    set_source_files_properties(
        ${LIB_SOURCES}
        PROPERTIES COMPILE_FLAGS "-UHAVE_NASM"
    )

    set_source_files_properties(
        ${LIB_SOURCES}
        PROPERTIES COMPILE_DEFINITIONS "HAVE_NASM=0"
    )

    # target_compile_options(${LIB_NAME} PRIVATE
    #     -O2
    #     -msse2
    #     -mfpmath=sse
    #     -fno-fast-math
    #     -fno-unsafe-math-optimizations
    #     -fno-tree-vectorize
    #     -fno-strict-aliasing
    # )

    # set_property(TARGET ${LIB_NAME} PROPERTY COMPILE_OPTIONS
    #     "-O2"
    #     "-msse2"
    #     "-mfpmath=sse"
    #     "-fno-fast-math"
    #     "-fno-unsafe-math-optimizations"
    #     "-fno-tree-vectorize"
    #     "-fno-strict-aliasing"
    # )

    # // Disable -lto
    # set_property(TARGET ${LIB_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)

    # target_compile_options(${LIB_NAME} PRIVATE -O2 -msse2 -mfpmath=sse)

    target_compile_definitions(${LIB_NAME} PRIVATE

        HAVE_CONFIG_H=1

        USE_FAST_MATH=1

        LAME_USE_FAST_FLOATS=1

        TAKEHIRO_IEEE754_HACK=0

        HAVE_NASM=1

        HAVE_SSE2_NASM=1

        # # --- Disable NASM completely ---
        # HAVE_NASM=0
        # HAVE_MMX=0
        # HAVE_3DNOW=0
        # HAVE_MMX_NASM=0
        # HAVE_3DNOW_NASM=0
        # HAVE_SSE_NASM=0
        # HAVE_SSE2_NASM=0

        # # --- Enable pure C + SSE2 paths ---
        # HAVE_SSE=1
        # HAVE_SSE2=1
        # HAVE_XMM=1

        # # --- Float encoder correctness ---
        # LAME_USE_FLOATS=1
        # LAME_USE_FAST_FLOATS=0
        # TAKEHIRO_IEEE754_HACK=1

        # # --- Disable fast math inside LAME only ---
        # USE_FAST_MATH=0
        # NO_FAST_MATH=1

        # # --- Match config.h behavior ---
        # USE_FAST_LOG=1

        # Windows vs Linux mpglib behavior
        # $<$<PLATFORM_ID:Windows>:HAVE_MPGLIB=0>
        # $<$<NOT:$<PLATFORM_ID:Windows>>:HAVE_MPGLIB=1>

        # # Linux-only replaygain accuracy
        # $<$<NOT:$<PLATFORM_ID:Windows>>:DECODE_ON_THE_FLY=1>
    )

    if (WIN32)
        target_compile_definitions(${LIB_NAME} PRIVATE
            # HAVE_MPGLIB=0
            # DECODE_ON_THE_FLY=0
        )
    else()
        target_compile_definitions(${LIB_NAME} PRIVATE
            HAVE_MPGLIB=1
            DECODE_ON_THE_FLY=1
        )

    endif()

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

endif()

add_subdirectory(test)
