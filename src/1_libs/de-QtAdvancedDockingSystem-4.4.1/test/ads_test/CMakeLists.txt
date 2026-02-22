set(APP_NAME advanced_docking_system_test_qt6)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES 
        ${APP_SOURCE_DIR}/main.cpp
        ${APP_SOURCE_DIR}/MainWindow.cpp
        ${APP_SOURCE_DIR}/MainWindow.h
    )
    # list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc)
    # if (WIN32)
        # enable_language(RC)
        # list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    # set(CMAKE_AUTOUIC ON)
    # find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Gui Widgets) # Quick QuickWidgets
    #find_package(Qt5 REQUIRED COMPONENTS Widgets Svg)
    find_package(Qt6Core)
    find_package(Qt6Gui)
    find_package(Qt6Widgets)
    # find_package(Qt6Quick)
    # find_package(Qt6QuickWidgets)
    # find_package(Qt6OpenGL)
    # find_package(Qt6OpenGLWidgets)
    
    find_package(Qt6 COMPONENTS Core Gui Widgets) # Quick QuickWidgets
    include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
    add_definitions(${Qt6Widgets_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (WIN32)
        target_link_libraries(${APP_NAME} PRIVATE comctl32 user32 gdi32 kernel32)
        target_link_libraries(${APP_NAME} PRIVATE ole32 oleaut32)
    else()
        target_link_libraries(${APP_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)
        # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        target_link_libraries(${APP_NAME} PRIVATE pthread)
        target_link_libraries(${APP_NAME} PRIVATE m)
    endif()
    
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Core)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Gui)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets)
    # target_link_libraries(${APP_NAME} PRIVATE Qt6::Quick)
    # target_link_libraries(${APP_NAME} PRIVATE Qt6::QuickWidgets)

    target_link_libraries(${APP_NAME} PRIVATE de_ads_441)
    
endif()
