set(LIB_NAME de_alac)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # option(FAAD2_BUILD_MP4FF "Build mp4ff demuxer" OFF)
    # option(FAAD2_ENABLE_DRM "Enable DRM AAC decoding" OFF)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/alac/aglib.h
        ${LIB_HEADER_DIR}/alac/ALACAudioTypes.h
        ${LIB_HEADER_DIR}/alac/ALACBitUtilities.h
        ${LIB_HEADER_DIR}/alac/ALACDecoder.h
        ${LIB_HEADER_DIR}/alac/ALACEncoder.h
        ${LIB_HEADER_DIR}/alac/dplib.h
        ${LIB_HEADER_DIR}/alac/EndianPortable.h
        ${LIB_HEADER_DIR}/alac/matrixlib.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/ag_dec.c
        ${LIB_SOURCE_DIR}/ag_enc.c
        ${LIB_SOURCE_DIR}/ALACBitUtilities.c
        ${LIB_SOURCE_DIR}/ALACDecoder.cpp
        ${LIB_SOURCE_DIR}/ALACEncoder.cpp
        ${LIB_SOURCE_DIR}/dp_dec.c
        ${LIB_SOURCE_DIR}/dp_enc.c
        ${LIB_SOURCE_DIR}/EndianPortable.c
        ${LIB_SOURCE_DIR}/matrix_dec.c
        ${LIB_SOURCE_DIR}/matrix_enc.c
    )

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")

        # # if(WIN32)
            # # enable_language(RC)
            # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
            # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # # endif()
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/alac)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ALAC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC FAAD2_STATIC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_CONFIG_H=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC SBR_DEC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC PS_DEC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC LTP_DEC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC MAIN_DEC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC LD_DEC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC ERROR_RESILIENCE=1)

    # if(FAAD2_ENABLE_DRM)
        # target_compile_definitions(${LIB_NAME} PUBLIC DRM)
    # endif()


endif()
