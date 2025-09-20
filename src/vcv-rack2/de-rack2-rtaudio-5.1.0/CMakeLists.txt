set(LIB_NAME de_rack2_rtaudio) # We dont like to clash with system libs.
project(${LIB_NAME}-5.1.0-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(RTA_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(RTA_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(RTA_HEADERS ${RTA_HEADER_DIR}/RtAudio/RtAudio.h)
    set(RTA_SOURCES ${RTA_SOURCE_DIR}/RtAudio.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND RTA_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND RTA_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${RTA_HEADERS} PRIVATE ${RTA_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${RTA_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${RTA_HEADER_DIR}/RtAudio)
    target_include_directories(${LIB_NAME} PRIVATE ${RTA_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_RTAUDIO=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC RTAUDIO_DUMMY=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC MACOSX_CORE=1)


    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC m)	
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
        target_link_libraries(${LIB_NAME} PUBLIC de_rack2_asiosdk)
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

# add_subdirectory(test)
