if (NOT WIN32)
    return()
endif()

set(APP_NAME AzimutProjection-wgl)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES_H   "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_HPP "${APP_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
    set(APP_SOURCES ${APP_SOURCES_H} ${APP_SOURCES_HPP} ${APP_SOURCES_CPP})

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()
    
    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    # #target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/window)
    target_compile_definitions(${APP_NAME} PRIVATE UNICODE=1)
    target_compile_definitions(${APP_NAME} PRIVATE _UNICODE=1)
    # target_compile_definitions(${APP_NAME} PRIVATE OEMRESOURCE=1) # needed by winuser.h ODC
    # target_compile_definitions(${APP_NAME} PRIVATE GL_EXT_PROTOTYPES=1)
    # target_compile_definitions(${APP_NAME} PRIVATE GL_GLEXT_PROTOTYPES=1)
    # target_compile_definitions(${APP_NAME} PRIVATE WGL_WGLEXT_PROTOTYPES=1)

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()
    
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE DarkGDK_2024)
endif()



# add_subdirectory(test)
