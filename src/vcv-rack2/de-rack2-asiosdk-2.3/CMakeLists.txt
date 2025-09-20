set(LIB_NAME de_rack2_asiosdk)
project(${LIB_NAME}-2.3-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/asio.h
        ${LIB_HEADER_DIR}/ASIOConvertSamples.h
        ${LIB_HEADER_DIR}/asiosys.h
        ${LIB_HEADER_DIR}/iasiodrv.h)

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/asio.cpp
        ${LIB_SOURCE_DIR}/ASIOConvertSamples.cpp
        ${LIB_SOURCE_DIR}/asiodrivers.cpp
        ${LIB_SOURCE_DIR}/asiolist.cpp)

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

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ASIOSDK=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ASIO=1)

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_compile_definitions(${LIB_NAME} PUBLIC -DRTAUDIO_DUMMY=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DMACOSX_CORE=1)

    if (MSVC)
		set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)	
	else()	
        target_link_libraries(${LIB_NAME} PUBLIC m)
		
		if (WIN32)
			target_link_libraries(${LIB_NAME} PUBLIC ks ksguid dsound winmm ksuser uuid advapi32 ole32 comctl32 kernel32)
		else()
			# target_link_libraries(${LIB_NAME} PUBLIC de_sndfile)
			# target_link_libraries(${LIB_NAME} PUBLIC asound)
			# target_link_libraries(${LIB_NAME} PUBLIC jack)
			# target_link_libraries(${LIB_NAME} PUBLIC pulse)
			# target_compile_definitions(${LIB_NAME} PUBLIC -D__LINUX_OSS__=1)
		endif()
    endif()
	
endif()
