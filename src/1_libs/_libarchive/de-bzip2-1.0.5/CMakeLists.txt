set(LIB_NAME de_bzip2)
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/bzip2)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
    ${LIB_HEADER_DIR}/bzlib.h)
    set(LIB_SOURCES
    ${LIB_SOURCE_DIR}/blocksort.c
    ${LIB_SOURCE_DIR}/bzcompress.c
    ${LIB_SOURCE_DIR}/bzlib.c
    ${LIB_SOURCE_DIR}/bzlib_private.h
    ${LIB_SOURCE_DIR}/crctable.c
    ${LIB_SOURCE_DIR}/decompress.c
    ${LIB_SOURCE_DIR}/huffman.c
    ${LIB_SOURCE_DIR}/randtable.c)

    if (WIN32)
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

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})    

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_BZIP2=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBBZ2=1)
	
    if(MSVC)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-maybe-uninitialized)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
    endif()
    
    # file(COPY ${LIB_HEADERS} DESTINATION ${LIB_INCLUDE_DIR}) # For the SDK

endif()

add_subdirectory(test)
