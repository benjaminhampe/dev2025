set(LIB_NAME de_brotlicommon)

project(${LIB_NAME}-1.1.1-rc0)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/brotli/decode.h
        ${LIB_HEADER_DIR}/brotli/encode.h
        ${LIB_HEADER_DIR}/brotli/port.h
        ${LIB_HEADER_DIR}/brotli/shared_dictionary.h
        ${LIB_HEADER_DIR}/brotli/types.h
        ${LIB_HEADER_DIR}/brotli/common/constants.h
        ${LIB_HEADER_DIR}/brotli/common/context.h
        ${LIB_HEADER_DIR}/brotli/common/dictionary.h
        ${LIB_HEADER_DIR}/brotli/common/platform.h
        ${LIB_HEADER_DIR}/brotli/common/shared_dictionary_internal.h
        ${LIB_HEADER_DIR}/brotli/common/transform.h
        ${LIB_HEADER_DIR}/brotli/common/version.h		
    )
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/constants.c
        ${LIB_SOURCE_DIR}/context.c
        ${LIB_SOURCE_DIR}/dictionary.c
        ${LIB_SOURCE_DIR}/platform.c
        ${LIB_SOURCE_DIR}/shared_dictionary.c
        ${LIB_SOURCE_DIR}/transform.c
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
		target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/brotli)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/brotli/common)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_BROTLICOMMON=1)

endif()

# add_subdirectory(test)
