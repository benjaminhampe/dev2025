set(LIB_NAME de_miniaudio) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/miniaudio.h)
    set(LIB_SOURCES ${LIB_SOURCE_DIR}/miniaudio.c)

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/RtAudio)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIAUDIO=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC RTAUDIO_DUMMY=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC MACOSX_CORE=1)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)
    else()
		target_link_libraries(${LIB_NAME} PUBLIC m)
        target_link_libraries(${LIB_NAME} PUBLIC pthread)
	endif()
	
    if (WIN32)
        target_compile_definitions(${LIB_NAME} PUBLIC _WIN32_WINNT=0x0602)
        target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_ASIO__=1)
        target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_DS__=1)
        target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_WASAPI__=1)
        target_link_libraries(${LIB_NAME} PUBLIC mfplat mf mfuuid wmcodecdspuuid)
        #target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        target_link_libraries(${LIB_NAME} PUBLIC dsound winmm ksuser uuid) # ks ksguid
        target_link_libraries(${LIB_NAME} PUBLIC advapi32 ole32 oleaut32 comctl32 kernel32 user32)
        # target_link_libraries(${LIB_NAME} PUBLIC de_asiosdk)
    else()
        target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_ALSA__=1)
        target_compile_definitions(${LIB_NAME} PUBLIC __UNIX_JACK__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_PULSE__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_OSS__=1)
        target_link_libraries(${LIB_NAME} PUBLIC asound)
        target_link_libraries(${LIB_NAME} PUBLIC jack)
        # target_link_libraries(${LIB_NAME} PUBLIC de_sndfile)        
    endif()



endif()

add_subdirectory(test)
