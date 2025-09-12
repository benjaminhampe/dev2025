set(LIB_NAME de_OpenEXR_Util)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/OpenEXR)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/ImfCheckFile.h
		${LIB_HEADER_DIR}/ImfDeepImage.h
		${LIB_HEADER_DIR}/ImfDeepImageChannel.h
		${LIB_HEADER_DIR}/ImfDeepImageIO.h
		${LIB_HEADER_DIR}/ImfDeepImageLevel.h
		${LIB_HEADER_DIR}/ImfFlatImage.h
		${LIB_HEADER_DIR}/ImfFlatImageChannel.h
		${LIB_HEADER_DIR}/ImfFlatImageIO.h
		${LIB_HEADER_DIR}/ImfFlatImageLevel.h
		${LIB_HEADER_DIR}/ImfImage.h
		${LIB_HEADER_DIR}/ImfImageChannel.h
		${LIB_HEADER_DIR}/ImfImageChannelRenaming.h
		${LIB_HEADER_DIR}/ImfImageDataWindow.h
		${LIB_HEADER_DIR}/ImfImageIO.h
		${LIB_HEADER_DIR}/ImfImageLevel.h
		${LIB_HEADER_DIR}/ImfSampleCountChannel.h
		${LIB_HEADER_DIR}/ImfUtilExport.h
	)
	
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/ImfCheckFile.cpp
		${LIB_SOURCE_DIR}/ImfDeepImage.cpp
		${LIB_SOURCE_DIR}/ImfDeepImageChannel.cpp
		${LIB_SOURCE_DIR}/ImfDeepImageIO.cpp
		${LIB_SOURCE_DIR}/ImfDeepImageLevel.cpp
		${LIB_SOURCE_DIR}/ImfFlatImage.cpp
		${LIB_SOURCE_DIR}/ImfFlatImageChannel.cpp
		${LIB_SOURCE_DIR}/ImfFlatImageIO.cpp
		${LIB_SOURCE_DIR}/ImfFlatImageLevel.cpp
		${LIB_SOURCE_DIR}/ImfImage.cpp
		${LIB_SOURCE_DIR}/ImfImageChannel.cpp
		${LIB_SOURCE_DIR}/ImfImageDataWindow.cpp
		${LIB_SOURCE_DIR}/ImfImageIO.cpp
		${LIB_SOURCE_DIR}/ImfImageLevel.cpp
		${LIB_SOURCE_DIR}/ImfSampleCountChannel.cpp
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

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Core)
    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
