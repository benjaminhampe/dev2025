set(APP_NAME tutorial_AntonOpenGL_02_shaders)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    #add_subdirectory(lib)
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    set(APP_SOURCES
    ${APP_SOURCE_DIR}/main.c
    ${APP_SOURCE_DIR}/gl_utils.c
    ${APP_SOURCE_DIR}/gl_utils.h)

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

    # Copy media:
    set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/media)
    set(DST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${SRC_DIR} DESTINATION ${DST_DIR})
endif()
