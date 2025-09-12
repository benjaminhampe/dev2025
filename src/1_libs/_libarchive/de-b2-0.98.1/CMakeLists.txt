set(LIB_NAME de_b2)

project(${LIB_NAME}-0.98.1)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/blake2.h
        ${LIB_HEADER_DIR}/blake2-kat.h		
	)
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/blake2-config.h
        ${LIB_SOURCE_DIR}/blake2-dispatch.c
        ${LIB_SOURCE_DIR}/blake2-impl.h
        ${LIB_SOURCE_DIR}/blake2b-load-sse2.h
        ${LIB_SOURCE_DIR}/blake2b-load-sse41.h
        ${LIB_SOURCE_DIR}/blake2b-ref.c
        ${LIB_SOURCE_DIR}/blake2b-round.h
        ${LIB_SOURCE_DIR}/blake2b.c
        ${LIB_SOURCE_DIR}/blake2bp.c
        ${LIB_SOURCE_DIR}/blake2s-load-sse2.h
        ${LIB_SOURCE_DIR}/blake2s-load-sse41.h
        ${LIB_SOURCE_DIR}/blake2s-load-xop.h
        ${LIB_SOURCE_DIR}/blake2s-ref.c
        ${LIB_SOURCE_DIR}/blake2s-round.h
        ${LIB_SOURCE_DIR}/blake2s.c
        ${LIB_SOURCE_DIR}/blake2sp.c
        ${LIB_SOURCE_DIR}/config.h
    )
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
		# # target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBB2=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    target_compile_options(${LIB_NAME} PRIVATE -fopenmp)
    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
	# target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
	# target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)
		
	# check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

	# target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)
    
	# target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

endif()

# add_subdirectory(test)
