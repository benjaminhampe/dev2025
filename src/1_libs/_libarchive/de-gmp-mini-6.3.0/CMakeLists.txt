set(LIB_NAME de_gmp_mini)

project(${LIB_NAME}-6.3.0)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/mini-gmp/mini-gmp.h
        ${LIB_HEADER_DIR}/mini-gmp/mini-mpq.h
    )
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/mini-gmp.c
        ${LIB_SOURCE_DIR}/mini-mpq.c		
    )
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
		# target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/mini-gmp)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GMP_MINI=1)
	
    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
	# target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
	# target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)
		
	# check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

	# target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)
    
	# target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

endif()

# add_subdirectory(test)
