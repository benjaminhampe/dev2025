set(LIB_NAME smtg_sdk_common)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/public.sdk/source/common)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/public.sdk/source/common)
    set(LIB_HEADERS
		${LIB_HEADER_DIR}/commonstringconvert.h
		${LIB_HEADER_DIR}/memorystream.h
		${LIB_HEADER_DIR}/openurl.h
		#${LIB_HEADER_DIR}/pluginview.h
		${LIB_HEADER_DIR}/readfile.h
		${LIB_HEADER_DIR}/systemclipboard.h
		${LIB_HEADER_DIR}/threadchecker.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/commoniids.cpp
        ${LIB_SOURCE_DIR}/commonstringconvert.cpp
        ${LIB_SOURCE_DIR}/memorystream.cpp
        ${LIB_SOURCE_DIR}/openurl.cpp
        #${LIB_SOURCE_DIR}/pluginview.cpp
        ${LIB_SOURCE_DIR}/readfile.cpp
        #${LIB_SOURCE_DIR}/systemclipboard_linux.cpp
        #${LIB_SOURCE_DIR}/systemclipboard_mac.mm
        ${LIB_SOURCE_DIR}/systemclipboard_win32.cpp
        #${LIB_SOURCE_DIR}/threadchecker_linux.cpp
        #${LIB_SOURCE_DIR}/threadchecker_mac.mm
        ${LIB_SOURCE_DIR}/threadchecker_win32.cpp
    )
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	# "$<$<CONFIG:Debug>:-DDEVELOPMENT=1>"
	# "$<$<CONFIG:Release>:-DRELEASE=1>"
	# "$<$<CONFIG:RelWithDebInfo>:-DRELEASE=1>"
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_compile_definitions(${LIB_NAME} PRIVATE DEVELOPMENT=1)
	else()
		target_compile_definitions(${LIB_NAME} PRIVATE RELEASE=1)
	endif()
	
	target_link_libraries(${LIB_NAME} PUBLIC smtg_base)
	
endif()


