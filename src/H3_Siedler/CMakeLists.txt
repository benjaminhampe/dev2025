set(APP_NAME H3_Siedler)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES_H ${APP_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE APP_SOURCES_CPP ${APP_SOURCE_DIR}/*.cpp)
    #file(GLOB_RECURSE APP_SOURCES_H3_H ${APP_SOURCE_DIR}/H3/*.h)
    #file(GLOB_RECURSE APP_SOURCES_H3_CPP ${APP_SOURCE_DIR}/H3/*.cpp)
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_CPP})
    # ${APP_SOURCES_H3_H} ${APP_SOURCES_H3_CPP}

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
	  # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

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
        target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE DarkImage_2504)
	# target_link_libraries(${APP_NAME} PRIVATE de_gui_2504)
	
    if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # message(STATUS "Building in Release mode, strip symbols")

        # 1. Stripping Debug Symbols from an Executable
        #  objcopy --strip-debug myprogram
        #   - Removes debugging symbols but keeps function names.
        #
        # 2. Creating a Separate Debug Symbol File
        # If you want to keep debugging symbols in a separate file for later use:
        #  objcopy --only-keep-debug myprogram myprogram.debug
        #  objcopy --strip-debug myprogram
        #  objcopy --add-gnu-debuglink=myprogram.debug myprogram

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
