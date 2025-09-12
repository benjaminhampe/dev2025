set(LIB_NAME de_unzip)
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/unzip)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/unzip/crypt.h
		${LIB_HEADER_DIR}/unzip/ioapi.h
		${LIB_HEADER_DIR}/unzip/unzip.h)
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/ioapi.c
		${LIB_SOURCE_DIR}/unzip.c)

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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/unzip)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-variable)
    endif()
    
    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)

    # file(COPY ${LIB_HEADERS} DESTINATION ${LIB_HEADER_DIR})
endif()
