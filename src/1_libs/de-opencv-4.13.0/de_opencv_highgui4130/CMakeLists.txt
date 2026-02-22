set(LIB_NAME de_opencv_highgui4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/highgui/highgui.hpp
        ${LIB_HEADER_DIR}/opencv2/highgui/highgui_c.h
        ${LIB_HEADER_DIR}/opencv2/highgui/highgui_winrt.hpp
        ${LIB_HEADER_DIR}/opencv2/highgui.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/_benni/opencv_highgui_config.hpp
        ${LIB_SOURCE_DIR}/backend.cpp
        ${LIB_SOURCE_DIR}/backend.hpp
        ${LIB_SOURCE_DIR}/factory.hpp
        # ${LIB_SOURCE_DIR}/files_Qt/Material/106.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/107.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/19.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/23.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/24.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/27.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/28.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/38.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/43.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/61.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/7.png
        # ${LIB_SOURCE_DIR}/files_Qt/Material/LICENSE
        # ${LIB_SOURCE_DIR}/files_Qt/Material/Readme.txt
        ${LIB_SOURCE_DIR}/plugin_api.hpp
        ${LIB_SOURCE_DIR}/plugin_wrapper.impl.hpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/registry.hpp
        ${LIB_SOURCE_DIR}/registry.impl.hpp
        ${LIB_SOURCE_DIR}/roiSelector.cpp
        ${LIB_SOURCE_DIR}/window.cpp
        # ${LIB_SOURCE_DIR}/window_cocoa.mm
        # ${LIB_SOURCE_DIR}/window_framebuffer.cpp
        # ${LIB_SOURCE_DIR}/window_framebuffer.hpp
        # ${LIB_SOURCE_DIR}/window_gtk.cpp
        # ${LIB_SOURCE_DIR}/window_QT.cpp
        # ${LIB_SOURCE_DIR}/window_QT.h
        # ${LIB_SOURCE_DIR}/window_QT.qrc
        ${LIB_SOURCE_DIR}/window_w32.cpp
        # ${LIB_SOURCE_DIR}/window_wayland.cpp
        # ${LIB_SOURCE_DIR}/window_winrt.cpp
        # ${LIB_SOURCE_DIR}/window_winrt_bridge.cpp
        # ${LIB_SOURCE_DIR}/window_winrt_bridge.hpp
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/_benni)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __OPENCV_BUILD=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_WIN32UI=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_OPENGL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE WITH_QT=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_core4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgcodecs4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_videoio4130)

    target_link_libraries(${LIB_NAME} PUBLIC user32)
    target_link_libraries(${LIB_NAME} PUBLIC gdi32)
    target_link_libraries(${LIB_NAME} PUBLIC comdlg32)
    target_link_libraries(${LIB_NAME} PUBLIC advapi32)
    target_link_libraries(${LIB_NAME} PUBLIC opengl32)

endif()

# add_subdirectory(test)
