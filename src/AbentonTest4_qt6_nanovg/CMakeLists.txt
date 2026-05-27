set(APP_NAME AbentonTest4_qt6_nanovg)
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
    find_package(Qt6Core)
    find_package(Qt6Widgets)
    find_package(Qt6Svg)
    # find_package(Qt6OpenGL)
    # find_package(Qt6OpenGLWidgets)
    # find_package(Qt6 COMPONENTS Core Widgets Svg OpenGL OpenGLWidgets)
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
        target_compile_options(${APP_NAME} PRIVATE -Wno-macro-redefined)
		target_compile_options(${APP_NAME} PRIVATE -Wno-pragma-pack)
		# target_compile_options(${APP_NAME} PRIVATE -Wno-macro)
        # target_compile_options(${APP_NAME} PRIVATE -Werror=maybe-uninitialized)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)

        # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        # target_link_libraries(${APP_NAME} PRIVATE pthread)
        # target_link_libraries(${APP_NAME} PRIVATE m)
    endif()

    target_link_libraries(${APP_NAME} PRIVATE Qt6::Core)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Svg)

    target_link_libraries(${APP_NAME} PRIVATE DarkImage_2604)
    target_link_libraries(${APP_NAME} PRIVATE DarkGPU_2604)
    target_link_libraries(${APP_NAME} PRIVATE de_glm)
    target_link_libraries(${APP_NAME} PRIVATE de_glew)
    
    # target_link_libraries(${APP_NAME} PRIVATE de_kissfft_cpp)
    # target_link_libraries(${APP_NAME} PRIVATE de_pffft)
    # target_link_libraries(${APP_NAME} PRIVATE de_rtaudio_601)
    # target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)
    # target_link_libraries(${APP_NAME} PRIVATE de_vst2sdk)
    # target_link_libraries(${APP_NAME} PRIVATE de_vst3sdk_380)
    # target_link_libraries(${APP_NAME} PRIVATE de_clap)

    # # target_link_libraries(${APP_NAME} PRIVATE Qt6::OpenGL)
    # # target_link_libraries(${APP_NAME} PRIVATE Qt6::OpenGLWidgets)

    # OpenMP
    target_compile_options(${APP_NAME} PRIVATE -fopenmp)
    target_link_options(${APP_NAME} PRIVATE -fopenmp)


	target_link_options(${APP_NAME} PRIVATE
		-static
		-stdlib=libc++
		-Wl,-Bstatic -lc++ -lc++abi -lunwind -lwinpthread
		-Wl,-Bdynamic
	)

    if (WIN32)
        target_link_libraries(${APP_NAME} PRIVATE kernel32 user32 gdi32 shell32)
        target_link_libraries(${APP_NAME} PRIVATE ole32 oleaut32 comdlg32 comctl32)
        target_link_libraries(${APP_NAME} PRIVATE shlwapi winmm d2d1 dxgi d3d11 dcomp)
        target_link_libraries(${APP_NAME} PRIVATE imm32 advapi32 dwmapi)

    else()
        # target_link_libraries(${APP_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

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

    if (CMAKE_BUILD_TYPE STREQUAL "Release")

        message(STATUS "Building in Release mode, strip symbols")
        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        add_custom_command(
            TARGET ${APP_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            COMMENT "Stripping debug symbols"
        )

    endif()
endif()
