set(LIB_NAME de_ads_441)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    # add_subdirectory(lib)

    # set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    # file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    # set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_CPP})

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/ads_globals.h
        ${LIB_HEADER_DIR}/AutoHideDockContainer.h
        ${LIB_HEADER_DIR}/AutoHideSideBar.h
        ${LIB_HEADER_DIR}/AutoHideTab.h
        ${LIB_HEADER_DIR}/DockAreaTabBar.h
        ${LIB_HEADER_DIR}/DockAreaTitleBar.h
        ${LIB_HEADER_DIR}/DockAreaTitleBar_p.h
        ${LIB_HEADER_DIR}/DockAreaWidget.h
        ${LIB_HEADER_DIR}/DockComponentsFactory.h
        ${LIB_HEADER_DIR}/DockContainerWidget.h
        ${LIB_HEADER_DIR}/DockFocusController.h
        ${LIB_HEADER_DIR}/DockingStateReader.h
        ${LIB_HEADER_DIR}/DockManager.h
        ${LIB_HEADER_DIR}/DockOverlay.h
        ${LIB_HEADER_DIR}/DockSplitter.h
        ${LIB_HEADER_DIR}/DockWidget.h
        ${LIB_HEADER_DIR}/DockWidgetTab.h
        ${LIB_HEADER_DIR}/ElidingLabel.h
        ${LIB_HEADER_DIR}/FloatingDockContainer.h
        ${LIB_HEADER_DIR}/FloatingDragPreview.h
        ${LIB_HEADER_DIR}/IconProvider.h
        # ${LIB_HEADER_DIR}/linux/FloatingWidgetTitleBar.h
        ${LIB_HEADER_DIR}/PushButton.h
        ${LIB_HEADER_DIR}/ResizeHandle.h
    )
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)    
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/ads_globals.cpp
        ${LIB_SOURCE_DIR}/AutoHideDockContainer.cpp
        ${LIB_SOURCE_DIR}/AutoHideSideBar.cpp
        ${LIB_SOURCE_DIR}/AutoHideTab.cpp
        ${LIB_SOURCE_DIR}/CMakeLists.txt
        ${LIB_SOURCE_DIR}/DockAreaTabBar.cpp
        ${LIB_SOURCE_DIR}/DockAreaTitleBar.cpp
        ${LIB_SOURCE_DIR}/DockAreaWidget.cpp
        ${LIB_SOURCE_DIR}/DockComponentsFactory.cpp
        ${LIB_SOURCE_DIR}/DockContainerWidget.cpp
        ${LIB_SOURCE_DIR}/DockFocusController.cpp
        ${LIB_SOURCE_DIR}/DockingStateReader.cpp
        ${LIB_SOURCE_DIR}/DockManager.cpp
        ${LIB_SOURCE_DIR}/DockOverlay.cpp
        ${LIB_SOURCE_DIR}/DockSplitter.cpp
        ${LIB_SOURCE_DIR}/DockWidget.cpp
        ${LIB_SOURCE_DIR}/DockWidgetTab.cpp
        ${LIB_SOURCE_DIR}/ElidingLabel.cpp
        ${LIB_SOURCE_DIR}/FloatingDockContainer.cpp
        ${LIB_SOURCE_DIR}/FloatingDragPreview.cpp
        ${LIB_SOURCE_DIR}/IconProvider.cpp
        # ${LIB_SOURCE_DIR}/linux/FloatingWidgetTitleBar.cpp

        ${LIB_SOURCE_DIR}/PushButton.cpp
        # ${LIB_SOURCE_DIR}/qtadvanceddockingConfig.cmake
        ${LIB_SOURCE_DIR}/ResizeHandle.cpp
        # ${LIB_SOURCE_DIR}/src.pro

        ${LIB_SOURCE_DIR}/ads.qrc
        # ${LIB_SOURCE_DIR}/images/close-button-disabled.svg
        # ${LIB_SOURCE_DIR}/images/close-button-focused.svg
        # ${LIB_SOURCE_DIR}/images/close-button.svg
        # ${LIB_SOURCE_DIR}/images/detach-button-disabled.svg
        # ${LIB_SOURCE_DIR}/images/detach-button.svg
        # ${LIB_SOURCE_DIR}/images/maximize-button-focused.svg
        # ${LIB_SOURCE_DIR}/images/maximize-button.svg
        # ${LIB_SOURCE_DIR}/images/minimize-button-focused.svg
        # ${LIB_SOURCE_DIR}/images/minimize-button.svg
        # ${LIB_SOURCE_DIR}/images/pin-button-big.svg
        # ${LIB_SOURCE_DIR}/images/pin-button.svg
        # ${LIB_SOURCE_DIR}/images/restore-button-focused.svg
        # ${LIB_SOURCE_DIR}/images/restore-button.svg
        # ${LIB_SOURCE_DIR}/images/tabs-menu-button.svg
        # ${LIB_SOURCE_DIR}/images/vs-pin-button-disabled.svg
        # ${LIB_SOURCE_DIR}/images/vs-pin-button-pinned-focused.svg
        # ${LIB_SOURCE_DIR}/images/vs-pin-button-pinned.svg
        # ${LIB_SOURCE_DIR}/images/vs-pin-button.svg
        # ${LIB_SOURCE_DIR}/images/vs-pin-button_45.svg
        # ${LIB_SOURCE_DIR}/stylesheets/default.css
        # ${LIB_SOURCE_DIR}/stylesheets/default_linux.css
        # ${LIB_SOURCE_DIR}/stylesheets/focus_highlighting.css
        # ${LIB_SOURCE_DIR}/stylesheets/focus_highlighting_linux.css  
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
    #find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Svg)
    #find_package(Qt5 REQUIRED COMPONENTS Widgets Svg)
    find_package(Qt6Core)
    find_package(Qt6Widgets)
    # find_package(Qt6OpenGL)
    # find_package(Qt6OpenGLWidgets)
    find_package(Qt6 COMPONENTS Core Widgets) # OpenGL OpenGLWidgets
    include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
    add_definitions(${Qt6Widgets_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")


    if (BUILD_SHARED)
        message("${Blue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${Blue}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        target_compile_definitions(${LIB_NAME} PUBLIC ADS_STATIC=1)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
    else()    
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)

        # target_compile_options(${LIB_NAME} PRIVATE -Werror=maybe-uninitialized)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-deprecated-declarations)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)

        # target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
        target_link_libraries(${LIB_NAME} PRIVATE pthread)
        target_link_libraries(${LIB_NAME} PRIVATE m)
    endif()

    if (WIN32)
        target_link_libraries(${LIB_NAME} PRIVATE comctl32 user32 gdi32 kernel32)
        target_link_libraries(${LIB_NAME} PRIVATE ole32 oleaut32)
    else()
        target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    target_link_libraries(${LIB_NAME} PRIVATE Qt6::Core)
    target_link_libraries(${LIB_NAME} PRIVATE Qt6::Widgets)
    # target_link_libraries(${LIB_NAME} PRIVATE Qt6::OpenGL)
    # target_link_libraries(${LIB_NAME} PRIVATE Qt6::OpenGLWidgets)
    # target_link_libraries(${LIB_NAME} PUBLIC DarkImage_2504)
    # target_link_libraries(${LIB_NAME} PRIVATE de_taglib)

endif()

# add_subdirectory(test)

