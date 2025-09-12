set(LIB_NAME de_OpenEXR_Iex)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/Iex.h
		${LIB_HEADER_DIR}/IexBaseExc.h
		${LIB_HEADER_DIR}/IexErrnoExc.h
		${LIB_HEADER_DIR}/IexExport.h
		${LIB_HEADER_DIR}/IexForward.h
		${LIB_HEADER_DIR}/IexMacros.h
		${LIB_HEADER_DIR}/IexMathExc.h
		${LIB_HEADER_DIR}/IexMathFloatExc.h
		${LIB_HEADER_DIR}/IexMathFpu.h
		${LIB_HEADER_DIR}/IexMathIeeeExc.h
		${LIB_HEADER_DIR}/IexNamespace.h
		${LIB_HEADER_DIR}/IexThrowErrnoExc.h
	)
	
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/IexBaseExc.cpp
		${LIB_SOURCE_DIR}/IexMathFloatExc.cpp
		${LIB_SOURCE_DIR}/IexMathFpu.cpp
		${LIB_SOURCE_DIR}/IexThrowErrnoExc.cpp
	)

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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
		target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
    endif()

    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
