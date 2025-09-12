set(LIB_NAME de_theora) # We dont like to clash with system libs.
project(${LIB_NAME}-dev-2.10.0 C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # set(FT_WITH_ZLIB 1)
    # set(THEORA_VERSION_MAJOR "2")
    # set(THEORA_VERSION_MINOR "10")
    # set(THEORA_VERSION_PATCH "0")
    # set(THEORA_LIBRARY_VERSION "6.16.0")

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/theora)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/codec.h
        ${LIB_HEADER_DIR}/theora.h
        ${LIB_HEADER_DIR}/theoradec.h
        ${LIB_HEADER_DIR}/theoraenc.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/analyze.c
        ${LIB_SOURCE_DIR}/apiwrapper.c
        ${LIB_SOURCE_DIR}/apiwrapper.h
        ${LIB_SOURCE_DIR}/bitpack.c
        ${LIB_SOURCE_DIR}/bitpack.h
        ${LIB_SOURCE_DIR}/cpu.c
        ${LIB_SOURCE_DIR}/cpu.h
        ${LIB_SOURCE_DIR}/dct.h
        ${LIB_SOURCE_DIR}/decapiwrapper.c
        ${LIB_SOURCE_DIR}/decinfo.c
        ${LIB_SOURCE_DIR}/decint.h
        ${LIB_SOURCE_DIR}/decode.c
        ${LIB_SOURCE_DIR}/dequant.c
        ${LIB_SOURCE_DIR}/dequant.h
        ${LIB_SOURCE_DIR}/encapiwrapper.c
        ${LIB_SOURCE_DIR}/encfrag.c
        ${LIB_SOURCE_DIR}/encinfo.c
        ${LIB_SOURCE_DIR}/encint.h
        ${LIB_SOURCE_DIR}/encode.c
        # ${LIB_SOURCE_DIR}/encoder_disabled.c # dummy
        ${LIB_SOURCE_DIR}/enquant.c
        ${LIB_SOURCE_DIR}/enquant.h
        ${LIB_SOURCE_DIR}/fdct.c
        ${LIB_SOURCE_DIR}/fragment.c
        ${LIB_SOURCE_DIR}/huffdec.c
        ${LIB_SOURCE_DIR}/huffdec.h
        ${LIB_SOURCE_DIR}/huffenc.c
        ${LIB_SOURCE_DIR}/huffenc.h
        ${LIB_SOURCE_DIR}/huffman.h
        ${LIB_SOURCE_DIR}/idct.c
        ${LIB_SOURCE_DIR}/info.c
        ${LIB_SOURCE_DIR}/internal.c
        ${LIB_SOURCE_DIR}/internal.h
        ${LIB_SOURCE_DIR}/mathops.c
        ${LIB_SOURCE_DIR}/mathops.h
        ${LIB_SOURCE_DIR}/mcenc.c
        ${LIB_SOURCE_DIR}/modedec.h
        ${LIB_SOURCE_DIR}/ocintrin.h
        ${LIB_SOURCE_DIR}/quant.c
        ${LIB_SOURCE_DIR}/quant.h
        ${LIB_SOURCE_DIR}/rate.c
        ${LIB_SOURCE_DIR}/state.c
        ${LIB_SOURCE_DIR}/tokenize.c
        #${LIB_SOURCE_DIR}/x86_vc/
        ${LIB_SOURCE_DIR}/x86/mmxencfrag.c
        ${LIB_SOURCE_DIR}/x86/mmxfdct.c
        ${LIB_SOURCE_DIR}/x86/mmxfrag.c
        ${LIB_SOURCE_DIR}/x86/mmxfrag.h
        ${LIB_SOURCE_DIR}/x86/mmxidct.c
        ${LIB_SOURCE_DIR}/x86/mmxloop.h
        ${LIB_SOURCE_DIR}/x86/mmxstate.c
        ${LIB_SOURCE_DIR}/x86/sse2fdct.c
        ${LIB_SOURCE_DIR}/x86/x86enc.c
        ${LIB_SOURCE_DIR}/x86/x86enc.h
        ${LIB_SOURCE_DIR}/x86/x86int.h
        ${LIB_SOURCE_DIR}/x86/x86state.c
    )

    # if (WIN32)
    # enable_language(RC)
    # list(APPEND LIB_SOURCES
    # ${LIB_SOURCE_DIR}/common/win32thread.c
    # ${LIB_SOURCE_DIR}/common/win32thread.h )
    # endif ()

    if(WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_OGG_THEORA=1)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE 
            -Wno-parentheses
            -Wno-implicit-fallthrough
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-sign-compare
            -Wno-unused-but-set-variable
            -Wno-shift-negative-value
            -Wno-aggressive-loop-optimizations
        )
    endif()
    
    target_link_libraries(${LIB_NAME} PUBLIC de_ogg) # publishes <ogg/ogg.h> to others.

endif()

# add_subdirectory(test)
