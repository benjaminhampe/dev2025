set(LIB_NAME DarkSIMD)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    # set(LIB_HEADERS
        # ${LIB_HEADER_DIR}/bhampe/bhampe_fft.h
        # ${LIB_HEADER_DIR}/bhampe/bhampe_fft_avx2.h
        # ${LIB_HEADER_DIR}/bhampe/bhampe_fft_avx2_fma3.h
    # )
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/bhampe_fft.cpp
        # ${LIB_SOURCE_DIR}/bhampe_fft_avx2.cpp
        # ${LIB_SOURCE_DIR}/bhampe_fft_avx2_fma3.cpp
    # )

    file(GLOB_RECURSE LIB_HEADERS ${LIB_HEADER_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES ${LIB_SOURCE_DIR}/*.cpp)

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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBDARKSIMD=1)

    target_link_libraries(${LIB_NAME} PRIVATE DarkImage_2504)
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
