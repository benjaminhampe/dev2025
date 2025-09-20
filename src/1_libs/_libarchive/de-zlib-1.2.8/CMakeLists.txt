set(LIB_NAME de_zlib)
project(${LIB_NAME}-1.28-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/zlib/zlib.h
        ${LIB_HEADER_DIR}/zlib/crc32.h
        ${LIB_HEADER_DIR}/zlib/deflate.h
        ${LIB_HEADER_DIR}/zlib/gzguts.h
        ${LIB_HEADER_DIR}/zlib/inffast.h
        ${LIB_HEADER_DIR}/zlib/inffixed.h
        ${LIB_HEADER_DIR}/zlib/inflate.h
        ${LIB_HEADER_DIR}/zlib/inftrees.h
        ${LIB_HEADER_DIR}/zlib/trees.h
        ${LIB_HEADER_DIR}/zlib/zconf.h
        ${LIB_HEADER_DIR}/zlib/zutil.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/adler32.c
        ${LIB_SOURCE_DIR}/compress.c
        ${LIB_SOURCE_DIR}/crc32.c
        ${LIB_SOURCE_DIR}/deflate.c
        ${LIB_SOURCE_DIR}/gzclose.c
        ${LIB_SOURCE_DIR}/gzlib.c
        ${LIB_SOURCE_DIR}/gzread.c
        ${LIB_SOURCE_DIR}/gzwrite.c
        ${LIB_SOURCE_DIR}/infback.c
        ${LIB_SOURCE_DIR}/inffast.c
        ${LIB_SOURCE_DIR}/inflate.c
        ${LIB_SOURCE_DIR}/inftrees.c
        ${LIB_SOURCE_DIR}/trees.c
        ${LIB_SOURCE_DIR}/uncompr.c
        ${LIB_SOURCE_DIR}/zutil.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        target_compile_definitions(${LIB_NAME} PUBLIC ZLIB_DLL=1)
        target_compile_definitions(${LIB_NAME} PRIVATE ZLIB_INTERNAL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/zlib)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBZ=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ZLIB=1)
    target_compile_definitions(${LIB_NAME} PRIVATE LIB_VERNUM=0x1280)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libarchive)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
    endif()

endif()
add_subdirectory(test)
