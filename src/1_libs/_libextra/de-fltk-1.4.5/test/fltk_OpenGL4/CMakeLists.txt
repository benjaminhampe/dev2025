set(APP_NAME fltk_OpenGL4)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES ${APP_SOURCE_DIR}/fltk_OpenGL4.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_HEADERS} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    target_link_libraries(${APP_NAME} PUBLIC de_fltk_145)
    target_link_libraries(${APP_NAME} PUBLIC de_glew)
    target_link_libraries(${APP_NAME} PUBLIC opengl32 glu32)
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
endif()
