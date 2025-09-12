set(APP_NAME tutorial_AntonOpenGL_00_hello_triangle_gl2.1)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    #add_subdirectory(lib)
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    set(APP_SOURCES ${APP_SOURCE_DIR}/main.c)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/../res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/../res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    target_link_libraries(${APP_NAME} PRIVATE de_glm)
    target_link_libraries(${APP_NAME} PRIVATE de_glfw_glew)
    target_link_libraries(${APP_NAME} PRIVATE de_assimp)

endif()
