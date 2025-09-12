set(APP_NAME demo_learnopengl_3_model_loading)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
    else()
    #add_subdirectory(lib)
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

    file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")

    #   set(APP_SOURCES
    #         ${APP_SOURCE_DIR}/main.cpp
    #         ${APP_SOURCE_DIR}/gl_utils.cpp
    #         ${APP_SOURCE_DIR}/gl_utils.h
    #         ${APP_SOURCE_DIR}/maths_funcs.cpp
    #         ${APP_SOURCE_DIR}/maths_funcs.h)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldYellow}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE learnopengl)

    # Copy media:
    set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/media)
    set(DST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    file(COPY ${SRC_DIR} DESTINATION ${DST_DIR})
endif()
