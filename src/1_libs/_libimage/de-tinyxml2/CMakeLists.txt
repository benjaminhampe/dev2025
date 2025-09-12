set(LIB_NAME de_tinyxml2) # We dont like to clash with system libs.
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/tinyxml2)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/tinyxml2)
	set(LIB_HEADERS ${LIB_HEADER_DIR}/tinyxml2.h)
	set(LIB_SOURCES ${LIB_SOURCE_DIR}/tinyxml2.cpp)

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
    
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..) # For the libuser
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}) # For the libuser
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}) # For the sources
	
    if (MSVC)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
endif()
