set(LIB_NAME vst3_syncdelay)

project(${LIB_NAME}
		VERSION 3.73.0
		DESCRIPTION "Steinberg VST 3 Sync Delay example")

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_SOURCES 
		${LIB_SOURCE_DIR}/sync.h
		${LIB_SOURCE_DIR}/syncdelaycontroller.cpp
		${LIB_SOURCE_DIR}/syncdelaycontroller.h
		${LIB_SOURCE_DIR}/syncdelayfactory.cpp
		${LIB_SOURCE_DIR}/syncdelayids.h
		${LIB_SOURCE_DIR}/syncdelayprocessor.cpp
		${LIB_SOURCE_DIR}/syncdelayprocessor.h
		${LIB_SOURCE_DIR}/syncdelayversion.h
	)

    if (WIN32)
        enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
		list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/info.rc)
    endif()

    # message("${BoldGreen}[benni] APP: ${LIB_NAME}${ColourReset}")
    # add_executable(${LIB_NAME} "")

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")	
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PRIVATE ${LIB_SOURCES})
	
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-variable)
    endif()

    target_link_libraries(${LIB_NAME} PRIVATE de_vst3sdk)

endif()
