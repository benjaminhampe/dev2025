set(LIB_NAME de_celt) # We dont like to clash with system libs.
project(${LIB_NAME}-0.11 C ASM_NASM)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/celt/arch.h
        ${LIB_HEADER_DIR}/celt/bands.h
        ${LIB_HEADER_DIR}/celt/celt.h
        ${LIB_HEADER_DIR}/celt/celt_header.h
        ${LIB_HEADER_DIR}/celt/celt_types.h
        ${LIB_HEADER_DIR}/celt/cwrs.h
        ${LIB_HEADER_DIR}/celt/entcode.h
        ${LIB_HEADER_DIR}/celt/entdec.h
        ${LIB_HEADER_DIR}/celt/entenc.h
        ${LIB_HEADER_DIR}/celt/ecintrin.h
        ${LIB_HEADER_DIR}/celt/laplace.h
        ${LIB_HEADER_DIR}/celt/mathops.h
        ${LIB_HEADER_DIR}/celt/mfrngcod.h
        ${LIB_HEADER_DIR}/celt/modes.h
        ${LIB_HEADER_DIR}/celt/pitch.h
        ${LIB_HEADER_DIR}/celt/plc.h
        ${LIB_HEADER_DIR}/celt/quant_bands.h
        ${LIB_HEADER_DIR}/celt/rate.h
        ${LIB_HEADER_DIR}/celt/stack_alloc.h
        ${LIB_HEADER_DIR}/celt/vq.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/float_cast.h
        ${LIB_SOURCE_DIR}/mdct.h
        ${LIB_SOURCE_DIR}/kiss_fft.c
        ${LIB_SOURCE_DIR}/kiss_fft.h
        ${LIB_SOURCE_DIR}/_kiss_fft_guts.h	
        ${LIB_SOURCE_DIR}/bands.c
        # ${LIB_SOURCE_DIR}/c64_fft.c
        # ${LIB_SOURCE_DIR}/c64_fft.h
        ${LIB_SOURCE_DIR}/celt.c
        ${LIB_SOURCE_DIR}/cwrs.c
        # ${LIB_SOURCE_DIR}/dump_modes.c
        ${LIB_SOURCE_DIR}/entcode.c
        ${LIB_SOURCE_DIR}/entdec.c
        ${LIB_SOURCE_DIR}/entenc.c
        ${LIB_SOURCE_DIR}/fixed_c5x.h
        ${LIB_SOURCE_DIR}/fixed_c6x.h
        ${LIB_SOURCE_DIR}/fixed_debug.h
        ${LIB_SOURCE_DIR}/fixed_generic.h
        ${LIB_SOURCE_DIR}/header.c
        ${LIB_SOURCE_DIR}/laplace.c
        # ${LIB_SOURCE_DIR}/libcelt.vcxproj
        # ${LIB_SOURCE_DIR}/libcelt.vcxproj.filters
        # ${LIB_SOURCE_DIR}/Makefile.am
        # ${LIB_SOURCE_DIR}/match-test.sh
        ${LIB_SOURCE_DIR}/mathops.c
        ${LIB_SOURCE_DIR}/mdct.c
        ${LIB_SOURCE_DIR}/modes.c
        ${LIB_SOURCE_DIR}/os_support.h
        ${LIB_SOURCE_DIR}/pitch.c
        ${LIB_SOURCE_DIR}/plc.c
        ${LIB_SOURCE_DIR}/quant_bands.c
        ${LIB_SOURCE_DIR}/rate.c
        # ${LIB_SOURCE_DIR}/static_modes_fixed.c
        # ${LIB_SOURCE_DIR}/static_modes_float.c
		${LIB_SOURCE_DIR}/static_modes_float.h
        # ${LIB_SOURCE_DIR}/testcelt.c
		${LIB_SOURCE_DIR}/vq.c	
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

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/celt)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC CELT_BUILD=1)
    target_compile_definitions(${LIB_NAME} PUBLIC USE_ALLOCA=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_CELT=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_CELT_API_0_11=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    # target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

    if (MSVC)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC m)
        # target_link_libraries(${LIB_NAME} PUBLIC pthread)
    endif()
endif()

# add_subdirectory(test)
