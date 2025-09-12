set(LIB_NAME de_sqlite3) # We dont like to clash with system libs.
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

	set(LIB_HEADERS ${LIB_HEADER_DIR}/sqlite3.h)
	set(LIB_SOURCES ${LIB_SOURCE_DIR}/sqlite3.c)

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SQLITE3=1)
    
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

endif()

add_subdirectory(test)