set(LIB_NAME de_aften) # We dont like to clash with system libs.
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/libaften/a52.h
        ${LIB_HEADER_DIR}/libaften/a52dec.h
        ${LIB_HEADER_DIR}/libaften/a52enc.h
        ${LIB_HEADER_DIR}/libaften/a52tab.h
        ${LIB_HEADER_DIR}/libaften/aften-types.h
        ${LIB_HEADER_DIR}/libaften/aften.h
        ${LIB_HEADER_DIR}/libaften/bitalloc.h
        ${LIB_HEADER_DIR}/libaften/bitio.h
        ${LIB_HEADER_DIR}/libaften/bswap.h
        ${LIB_HEADER_DIR}/libaften/common.h
        ${LIB_HEADER_DIR}/libaften/config.h
        ${LIB_HEADER_DIR}/libaften/convert.h
        ${LIB_HEADER_DIR}/libaften/cpu_caps.h
        ${LIB_HEADER_DIR}/libaften/crc.h
        ${LIB_HEADER_DIR}/libaften/dynrng.h
        ${LIB_HEADER_DIR}/libaften/exponent.h
        ${LIB_HEADER_DIR}/libaften/filter.h
        ${LIB_HEADER_DIR}/libaften/mdct.h
        ${LIB_HEADER_DIR}/libaften/mem.h
        ${LIB_HEADER_DIR}/libaften/threading.h
        ${LIB_HEADER_DIR}/libaften/window.h
    )

    set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/libaften/a52.c
        ${LIB_SOURCE_DIR}/libaften/a52enc.c
        ${LIB_SOURCE_DIR}/libaften/a52tab.c
        ${LIB_SOURCE_DIR}/libaften/bitalloc.c
        ${LIB_SOURCE_DIR}/libaften/bitio.c
        ${LIB_SOURCE_DIR}/libaften/convert.c
        ${LIB_SOURCE_DIR}/libaften/crc.c
        ${LIB_SOURCE_DIR}/libaften/dynrng.c
        ${LIB_SOURCE_DIR}/libaften/exponent.c
        ${LIB_SOURCE_DIR}/libaften/filter.c
        ${LIB_SOURCE_DIR}/libaften/mdct.c
        ${LIB_SOURCE_DIR}/libaften/util.c
        ${LIB_SOURCE_DIR}/libaften/window.c

        # ${LIB_SOURCE_DIR}/libaften/ppc/altivec_common.h
        # ${LIB_SOURCE_DIR}/libaften/ppc/cpu_caps.c
        # ${LIB_SOURCE_DIR}/libaften/ppc/cpu_caps.h
        # ${LIB_SOURCE_DIR}/libaften/ppc/mdct.h
        # ${LIB_SOURCE_DIR}/libaften/ppc/mdct_altivec.c

        ${LIB_SOURCE_DIR}/libaften/x86/asm_common.h
        ${LIB_SOURCE_DIR}/libaften/x86/asm_support.h
        ${LIB_SOURCE_DIR}/libaften/x86/cpu_caps.c
        ${LIB_SOURCE_DIR}/libaften/x86/cpu_caps.h
        ${LIB_SOURCE_DIR}/libaften/x86/exponent.h
        ${LIB_SOURCE_DIR}/libaften/x86/exponent_mmx.c
        ${LIB_SOURCE_DIR}/libaften/x86/exponent_sse2.c
        ${LIB_SOURCE_DIR}/libaften/x86/gas_support.h
        ${LIB_SOURCE_DIR}/libaften/x86/intelas_support.h
        ${LIB_SOURCE_DIR}/libaften/x86/mdct.h
        ${LIB_SOURCE_DIR}/libaften/x86/mdct_common_sse.c
        ${LIB_SOURCE_DIR}/libaften/x86/mdct_common_sse.h
        ${LIB_SOURCE_DIR}/libaften/x86/mdct_sse.c
        ${LIB_SOURCE_DIR}/libaften/x86/mdct_sse3.c
        ${LIB_SOURCE_DIR}/libaften/x86/simd_support.h
        ${LIB_SOURCE_DIR}/libaften/x86/window.h
        ${LIB_SOURCE_DIR}/libaften/x86/window_sse.c
    )

    # if(WIN32)
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
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
	
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
    target_compile_definitions(${LIB_NAME} PUBLIC AFTEN_BUILD_LIBRARY=1)

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/libaften)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/libaften)

endif()   
