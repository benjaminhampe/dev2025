set(LIB_NAME de_minizip) # We dont like to clash with system libs.
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/minizip)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/minizip)
    set(LIB_HEADERS 
		${LIB_HEADER_DIR}/zip.h 
		${LIB_HEADER_DIR}/miniz.h
	)
    set(LIB_SOURCES 
		${LIB_SOURCE_DIR}/zip.c
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
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..) # For the SDK
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}) # For the sources

   #	file(COPY ${LIB_HEADERS} DESTINATION ${LIB_INCLUDE_DIR}) # For the SDK
endif()
# add_subdirectory(test)