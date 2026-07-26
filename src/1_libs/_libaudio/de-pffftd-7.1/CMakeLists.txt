set(LIB_NAME de_pffftd)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/pffft_double.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/pffft_double.c
        ${LIB_SOURCE_DIR}/pffft_priv_impl.h        
        ${LIB_SOURCE_DIR}/simd/pf_avx_double.h
        ${LIB_SOURCE_DIR}/simd/pf_double.h
        ${LIB_SOURCE_DIR}/simd/pf_neon_double.h
        ${LIB_SOURCE_DIR}/simd/pf_neon_double_from_avx.h
        ${LIB_SOURCE_DIR}/simd/pf_scalar_double.h
        ${LIB_SOURCE_DIR}/simd/pf_sse2_double.h       
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED_LIBS)
    #     message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    #     add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PFFFTD=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBPFFFTD=1)

    # Prevent FFTPack duplicate symbol with libfaad2.
    # Pro: Uses internal SIMD for FFT
    # Con: Only multiple of 2 FFT sizes allowed now.
    # target_compile_definitions(${LIB_NAME} PRIVATE PFFFT_SIMD_DISABLE_FFTPACK=1)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libAudio)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

    # target_link_libraries(${LIB_NAME} PUBLIC de_rack2_nanovg)


    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
