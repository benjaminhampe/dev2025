set(APP_NAME face_detect_1)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES ${APP_SOURCE_DIR}/main_face_detect_1.cpp)

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
      # # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    # endif()

    # set(CMAKE_AUTOMOC ON)
    # set(CMAKE_AUTORCC ON)
    # set(CMAKE_AUTOUIC ON)
    #find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Svg)
    #find_package(Qt5 REQUIRED COMPONENTS Widgets Svg)
    # find_package(Qt6Core)
    # find_package(Qt6Widgets)
    # find_package(Qt6OpenGL)
    # find_package(Qt6OpenGLWidgets)
    # find_package(Qt6 COMPONENTS Core Widgets OpenGL OpenGLWidgets)
    # include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
    # add_definitions(${Qt6Widgets_DEFINITIONS})
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    target_link_libraries(${APP_NAME} PRIVATE de_opencv_core4130) # Mat‑Operationen
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_videoio4130) # Kamera öffnen
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_highgui4130) # Bilder anzeigen
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_objdetect4130) # Haar‑Cascade (Face Detection)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_imgproc4130) # Bildverarbeitung
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_ml4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_calib3d4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_dnn4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_features2d4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_flann4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_gapi4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_highgui4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_imgcodecs4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_photo4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_stitching4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_ts4130)
    target_link_libraries(${APP_NAME} PRIVATE de_opencv_video4130)

    # target_link_libraries(${APP_NAME} PRIVATE de_gui_2504)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # message(STATUS "Building in Release mode, strip symbols")

        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        add_custom_command(
            TARGET ${APP_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            COMMENT "Stripping debug symbols"
        )

    endif()

    # file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
