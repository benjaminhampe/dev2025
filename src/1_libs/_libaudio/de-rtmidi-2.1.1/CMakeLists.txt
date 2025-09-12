set(LIB_NAME de_rtmidi) # We dont like to clash with system libs.
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(RTM_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(RTM_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(RTM_HEADERS ${RTM_HEADER_DIR}/RtMidi/RtMidi.h)
    set(RTM_SOURCES ${RTM_SOURCE_DIR}/RtMidi/RtMidi.cpp)

    if(WIN32)
        enable_language(RC)
        list(APPEND RTM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND RTM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${RTM_HEADERS} PRIVATE ${RTM_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${RTM_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${RTM_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_RTMIDI=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC __MACOSX_CORE__=1)


    if (WIN32)
        target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_MM__=1)
        target_link_libraries(${LIB_NAME} PUBLIC ole32 comctl32 uuid winmm m)
    else()
        target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_ALSA__=1)
        target_compile_definitions(${LIB_NAME} PUBLIC __UNIX_JACK__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_PULSE__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_OSS__=1)
        target_link_libraries(${LIB_NAME} PUBLIC asound)        
        target_link_libraries(${LIB_NAME} PUBLIC jack)
        # target_link_libraries(${LIB_NAME} PUBLIC pulse)
    endif()

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER lib/audio)
    endif()

endif()

# add_subdirectory(test)
