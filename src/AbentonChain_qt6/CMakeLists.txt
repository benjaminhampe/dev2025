set(APP_NAME AbentonChain_qt6)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_CPP})

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc)
    endif()

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    # set(CMAKE_AUTOUIC OFF)
    find_package(Qt6Widgets)
    find_package(Qt6 COMPONENTS Core Widgets Svg)
    include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
    add_definitions(${Qt6Widgets_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (WIN32)
        # target_link_libraries(${APP_NAME} PRIVATE comctl32 user32 gdi32 kernel32)
    else()
        # target_link_libraries(${APP_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)

        # target_compile_options(${APP_NAME} PRIVATE -Werror=maybe-uninitialized)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)

        # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        # target_link_libraries(${APP_NAME} PRIVATE pthread)
        # target_link_libraries(${APP_NAME} PRIVATE m)
    endif()

    target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Svg)

    target_link_libraries(${APP_NAME} PRIVATE DarkImage_2504)

    target_link_libraries(${APP_NAME} PRIVATE de_kissfft_cpp)
    target_link_libraries(${APP_NAME} PRIVATE de_pffft)
    target_link_libraries(${APP_NAME} PRIVATE de_rtaudio_601)
    target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)
    target_link_libraries(${APP_NAME} PRIVATE de_vst2sdk)

	target_link_options(${APP_NAME} PRIVATE
		-static
		-stdlib=libc++
		-Wl,-Bstatic -lc++ -lc++abi -lunwind -lwinpthread
		-Wl,-Bdynamic
	)

    # target_link_libraries(${APP_NAME} PRIVATE AudioDreamy64_DarkImage)
    # target_link_libraries(${APP_NAME} PRIVATE de_fontawesome)
    # target_link_libraries(${APP_NAME} PRIVATE de_tinyxml2)
    # target_link_libraries(${APP_NAME} PRIVATE de_rtaudio)
    # target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)


    # target_link_libraries(${APP_NAME} PRIVATE de_fftw3)   # Can be used
    # target_link_libraries(${APP_NAME} PRIVATE de_kissfft) # What i use
    # target_link_libraries(${APP_NAME} PRIVATE de_soundtouch) # see GSoundTouch impl
    # target_link_libraries(${APP_NAME} PRIVATE de_sndfile) # Audio
    # target_link_libraries(${APP_NAME} PRIVATE de_ffmpeg)  # Audio + Video
    # target_link_libraries(${APP_NAME} PRIVATE de_lame)    # Audio mp3 encoder
endif()
