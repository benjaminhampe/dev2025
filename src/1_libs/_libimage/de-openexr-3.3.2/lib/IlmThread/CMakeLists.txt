set(LIB_NAME de_OpenEXR_IlmThread)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/IlmThread.h
		${LIB_HEADER_DIR}/IlmThreadExport.h
		${LIB_HEADER_DIR}/IlmThreadForward.h
		${LIB_HEADER_DIR}/IlmThreadMutex.h
		${LIB_HEADER_DIR}/IlmThreadNamespace.h
		${LIB_HEADER_DIR}/IlmThreadPool.h
		${LIB_HEADER_DIR}/IlmThreadProcessGroup.h
		${LIB_HEADER_DIR}/IlmThreadSemaphore.h
	)
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/IlmThread.cpp
		${LIB_SOURCE_DIR}/IlmThreadPool.cpp
		${LIB_SOURCE_DIR}/IlmThreadSemaphore.cpp
		${LIB_SOURCE_DIR}/IlmThreadSemaphoreOSX.cpp
		${LIB_SOURCE_DIR}/IlmThreadSemaphorePosix.cpp
		${LIB_SOURCE_DIR}/IlmThreadSemaphorePosixCompat.cpp
		${LIB_SOURCE_DIR}/IlmThreadSemaphoreWin32.cpp
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
		if(OPENEXR_ENABLE_THREADING)
			# target_link_libraries(${LIB_NAME} PUBLIC Threads::Threads)
		endif()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
