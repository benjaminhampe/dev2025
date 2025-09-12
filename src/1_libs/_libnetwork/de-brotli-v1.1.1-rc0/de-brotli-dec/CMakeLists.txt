set(LIB_NAME de_brotlidec)

project(${LIB_NAME}-1.1.1-rc0)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/brotli/dec/bit_reader.h
        ${LIB_HEADER_DIR}/brotli/dec/huffman.h
        ${LIB_HEADER_DIR}/brotli/dec/prefix.h
        ${LIB_HEADER_DIR}/brotli/dec/state.h
    )
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/dec/bit_reader.c
        ${LIB_SOURCE_DIR}/dec/decode.c
        ${LIB_SOURCE_DIR}/dec/huffman.c
        ${LIB_SOURCE_DIR}/dec/state.c
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/brotli/dec)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_BROTLIDEC=1)
	
	target_link_libraries(${LIB_NAME} PUBLIC de_brotlicommon)

endif()

# add_subdirectory(test)
