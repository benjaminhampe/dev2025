set(LIB_NAME de_soxr)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/samplerate.h
        ${LIB_HEADER_DIR}/soxr-lsr.h
        ${LIB_HEADER_DIR}/soxr.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/aliases.h
        # ${LIB_SOURCE_DIR}/avfft32.c     #include <libavcodec/avfft.h>
        # ${LIB_SOURCE_DIR}/avfft32s.c    #include <libavcodec/avfft.h>
        ${LIB_SOURCE_DIR}/ccrw2.h
        ${LIB_SOURCE_DIR}/cr-core.c
        ${LIB_SOURCE_DIR}/cr.c
        ${LIB_SOURCE_DIR}/cr.h
        ${LIB_SOURCE_DIR}/cr32.c
        ${LIB_SOURCE_DIR}/cr32s.c
        ${LIB_SOURCE_DIR}/cr64.c
        ${LIB_SOURCE_DIR}/cr64s.c
        ${LIB_SOURCE_DIR}/data-io.c
        ${LIB_SOURCE_DIR}/data-io.h
        ${LIB_SOURCE_DIR}/dbesi0.c
        ${LIB_SOURCE_DIR}/dev32s.h
        ${LIB_SOURCE_DIR}/dev64s.h
        ${LIB_SOURCE_DIR}/fft4g.c
        ${LIB_SOURCE_DIR}/fft4g.h
        ${LIB_SOURCE_DIR}/fft4g32.c
        ${LIB_SOURCE_DIR}/fft4g32s.c
        ${LIB_SOURCE_DIR}/fft4g64.c
        ${LIB_SOURCE_DIR}/fft4g_cache.h
        ${LIB_SOURCE_DIR}/fifo.h
        ${LIB_SOURCE_DIR}/filter.c
        ${LIB_SOURCE_DIR}/filter.h
        ${LIB_SOURCE_DIR}/half-coefs.h
        ${LIB_SOURCE_DIR}/half-fir.h
        ${LIB_SOURCE_DIR}/internal.h
        ${LIB_SOURCE_DIR}/math-wrap.h
        ${LIB_SOURCE_DIR}/poly-fir.h
        ${LIB_SOURCE_DIR}/poly-fir0.h
        ${LIB_SOURCE_DIR}/rdft.h
        ${LIB_SOURCE_DIR}/rdft_t.h
        ${LIB_SOURCE_DIR}/rint-clip.h
        ${LIB_SOURCE_DIR}/rint.h
        ${LIB_SOURCE_DIR}/soxr-config.h
        ${LIB_SOURCE_DIR}/soxr-lsr.c
        ${LIB_SOURCE_DIR}/soxr.c
        ${LIB_SOURCE_DIR}/std-types.h
        ${LIB_SOURCE_DIR}/util-simd.c
        ${LIB_SOURCE_DIR}/util32s.c
        ${LIB_SOURCE_DIR}/util32s.h
        ${LIB_SOURCE_DIR}/util64s.c
        ${LIB_SOURCE_DIR}/util64s.h
        ${LIB_SOURCE_DIR}/vr-coefs.c
        ${LIB_SOURCE_DIR}/vr-coefs.h
        ${LIB_SOURCE_DIR}/vr32.c

        ${LIB_SOURCE_DIR}/pffft-avx.h
        ${LIB_SOURCE_DIR}/pffft-wrap.c
        ${LIB_SOURCE_DIR}/pffft.c
        ${LIB_SOURCE_DIR}/pffft.h
        ${LIB_SOURCE_DIR}/pffft32.c
        ${LIB_SOURCE_DIR}/pffft32s.c
        ${LIB_SOURCE_DIR}/pffft64s.c
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lame.rc ${LIB_SOURCE_DIR}/logoe.ico )
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        target_compile_definitions(${LIB_NAME} PUBLIC SOXR_DLL=1)
        target_compile_definitions(${LIB_NAME} PRIVATE soxr_EXPORTS=1)        
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}) # for user.
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SAMPLERATE=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBSAMPLERATE=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SOXR=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBSOXR=1)

    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    target_compile_definitions(${LIB_NAME} PRIVATE WITH_OPENMP=1)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_LSR_BINDINGS=1)

    target_compile_definitions(${LIB_NAME} PRIVATE AVCODEC_FOUND=0)
    target_compile_definitions(${LIB_NAME} PRIVATE AVUTIL_FOUND=0)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_PFFFT=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_FENV_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_STDBOOL_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_STDINT_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LRINT=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_BIGENDIAN=0)

    target_compile_definitions(${LIB_NAME} PRIVATE WITH_CR32=1)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_CR32S=1)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_CR64=1)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_CR64S=1)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_VR32=1)

    target_compile_definitions(${LIB_NAME} PRIVATE WITH_HI_PREC_CLOCK=1)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_FLOAT_STD_PREC_CLOCK=0)
    target_compile_definitions(${LIB_NAME} PRIVATE WITH_DEV_TRACE=1)

    # target_link_libraries(${LIB_NAME} PUBLIC de_sndfile)
    # target_link_libraries(${LIB_NAME} PUBLIC de_fftw3)

    if (MSVC)
    else()
        target_link_libraries(${LIB_NAME} PRIVATE m)
    endif()

    # file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)

endif()

# add_subdirectory(test)
