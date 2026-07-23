project(DarkAudio)

if (TARGET ${PROJECT_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${PROJECT_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(PROJECT_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(PROJECT_HEADERS
        ${PROJECT_HEADER_DIR}/DarkAudio.h
        ${PROJECT_HEADER_DIR}/de/audio/DAC_LoopbackEngine_Miniaudio.h
        ${PROJECT_HEADER_DIR}/de/audio/DAC_PlaybackEngine_RtAudio.h
        ${PROJECT_HEADER_DIR}/de/audio/de_aligned_memory.h
        ${PROJECT_HEADER_DIR}/de/audio/de_fft_pffft.h
        ${PROJECT_HEADER_DIR}/de/audio/de_fft_generic.h
        ${PROJECT_HEADER_DIR}/de/audio/IDspChainElement.h
    )
    set(PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/DarkAudio.cpp
        ${PROJECT_SOURCE_DIR}/de/audio/DAC_LoopbackEngine_Miniaudio.cpp
        ${PROJECT_SOURCE_DIR}/de/audio/DAC_PlaybackEngine_RtAudio.cpp
        ${PROJECT_SOURCE_DIR}/de/audio/de_fft_pffft.cpp
        ${PROJECT_SOURCE_DIR}/de/audio/de_fft_generic.cpp
        ${PROJECT_SOURCE_DIR}/de/audio/de_kissfft.h
    )

    if (BUILD_SHARED_LIBS)
        message("${Blue}[benni] DLL: ${PROJECT_NAME}${ColourReset}")
        add_library(${PROJECT_NAME} SHARED "")
        if (WIN32)
            enable_language(RC)
            list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
            list(APPEND PROJECT_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        endif()
    else()
        message("${Blue}[benni] LIB: ${PROJECT_NAME}${ColourReset}")
        add_library(${PROJECT_NAME} STATIC "")
    endif()

    target_sources(${PROJECT_NAME} PUBLIC ${PROJECT_HEADERS} PRIVATE ${PROJECT_SOURCES})

    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_HEADER_DIR})
    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})

    if (MSVC)
    else()
        target_compile_options(${PROJECT_NAME} PUBLIC -Wno-deprecated-declarations)

        # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
        target_link_libraries(${PROJECT_NAME} PRIVATE m)
    endif()

    if (WIN32)
        target_link_libraries(${PROJECT_NAME} PRIVATE comctl32 user32 gdi32 kernel32)
        target_link_libraries(${PROJECT_NAME} PRIVATE ole32 oleaut32)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    target_link_libraries(${PROJECT_NAME} PUBLIC DarkImage_2504)
    # target_link_libraries(${PROJECT_NAME} PRIVATE de_taglib)

    target_link_libraries(${PROJECT_NAME} PRIVATE de_miniaudio)
    # target_link_libraries(${PROJECT_NAME} PRIVATE de_miniaudio_fs)

    target_link_libraries(${PROJECT_NAME} PUBLIC de_kissfft_cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE de_pffft)

    target_link_libraries(${PROJECT_NAME} PRIVATE de_rtaudio)
    # target_link_libraries(${PROJECT_NAME} PRIVATE de_rtmidi)

    # target_link_libraries(${PROJECT_NAME} PRIVATE de_glm)
    # target_link_libraries(${PROJECT_NAME} PRIVATE de_glew)

    # target_link_libraries(${LIB_NAME} PRIVATE de_rtaudio_601)
    # target_link_libraries(${LIB_NAME} PRIVATE de_sndfile)
    # target_link_libraries(${LIB_NAME} PRIVATE de_minimp3)
   # #target_link_libraries(${LIB_NAME} PRIVATE de_mpg123)
    # target_link_libraries(${LIB_NAME} PRIVATE de_soundtouch) # see GSoundTouch impl
    # target_link_libraries(${LIB_NAME} PRIVATE de_lame)    # Audio mp3 encoder
    # target_link_libraries(${LIB_NAME} PUBLIC de_kissfft) # What i use
    # target_link_libraries(${LIB_NAME} PUBLIC de_vst2sdk)
    # target_link_libraries(${LIB_NAME} PUBLIC de_rtmidi) # Need refactoring to make private.

    # target_link_libraries(${LIB_NAME} PRIVATE de_fftw3)   # Can be used
    # target_link_libraries(${LIB_NAME} PRIVATE de_ffmpeg)  # Audio + Video

    # set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    # file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    # set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_CPP})

    # list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc)

    # set(CMAKE_AUTOMOC ON)
    # set(CMAKE_AUTORCC ON)
    # set(CMAKE_AUTOUIC ON)
    # #find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Svg)
    # #find_package(Qt5 REQUIRED COMPONENTS Widgets Svg)
    # find_package(Qt6Core)
    # find_package(Qt6Widgets)
    # find_package(Qt6OpenGL)
    # find_package(Qt6OpenGLWidgets)
    # find_package(Qt6 COMPONENTS Core Widgets OpenGL OpenGLWidgets)
    # include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
    # add_definitions(${Qt6Widgets_DEFINITIONS})
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

    # target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
    # target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
    # target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::OpenGL)
    # target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::OpenGLWidgets)


endif()

# add_subdirectory(test)

