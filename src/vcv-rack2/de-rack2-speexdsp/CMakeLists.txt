set(LIB_NAME de_rack2_speexdsp)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/speex/speexdsp_config_types.h
        ${LIB_HEADER_DIR}/speex/speexdsp_config_types.h.in
        ${LIB_HEADER_DIR}/speex/speexdsp_types.h
        ${LIB_HEADER_DIR}/speex/speex_buffer.h
        ${LIB_HEADER_DIR}/speex/speex_echo.h
        ${LIB_HEADER_DIR}/speex/speex_jitter.h
        ${LIB_HEADER_DIR}/speex/speex_preprocess.h
        ${LIB_HEADER_DIR}/speex/speex_resampler.h

    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/arch.h
        ${LIB_SOURCE_DIR}/bfin.h
        ${LIB_SOURCE_DIR}/buffer.c
        ${LIB_SOURCE_DIR}/config.h
        ${LIB_SOURCE_DIR}/fftwrap.c
        ${LIB_SOURCE_DIR}/fftwrap.h
        ${LIB_SOURCE_DIR}/filterbank.c
        ${LIB_SOURCE_DIR}/filterbank.h
        ${LIB_SOURCE_DIR}/fixed_arm4.h
        ${LIB_SOURCE_DIR}/fixed_arm5e.h
        ${LIB_SOURCE_DIR}/fixed_bfin.h
        ${LIB_SOURCE_DIR}/fixed_debug.h
        ${LIB_SOURCE_DIR}/fixed_generic.h
        ${LIB_SOURCE_DIR}/jitter.c
        ${LIB_SOURCE_DIR}/kiss_fft.c
        ${LIB_SOURCE_DIR}/kiss_fft.h
        ${LIB_SOURCE_DIR}/kiss_fftr.c
        ${LIB_SOURCE_DIR}/kiss_fftr.h
        ${LIB_SOURCE_DIR}/math_approx.h
        ${LIB_SOURCE_DIR}/mdf.c
        ${LIB_SOURCE_DIR}/misc_bfin.h
        ${LIB_SOURCE_DIR}/os_support.h
        ${LIB_SOURCE_DIR}/preprocess.c
        ${LIB_SOURCE_DIR}/pseudofloat.h
        ${LIB_SOURCE_DIR}/resample.c
        ${LIB_SOURCE_DIR}/resample_neon.h
        ${LIB_SOURCE_DIR}/resample_sse.h
        ${LIB_SOURCE_DIR}/scal.c
        ${LIB_SOURCE_DIR}/smallft.c
        ${LIB_SOURCE_DIR}/smallft.h
        ${LIB_SOURCE_DIR}/vorbis_psy.h
        ${LIB_SOURCE_DIR}/_kiss_fft_guts.h
    )

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
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/nanovg)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBSPEEXDSP=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_CONFIG_H=1)

    # target_link_libraries(${LIB_NAME} PUBLIC de_rack2_glew)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)
    else()
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
        # target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()
	
    # file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
