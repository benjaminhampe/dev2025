set(APP_NAME sfml_demo_win32)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    file(GLOB_RECURSE APP_HEADERS ${APP_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE APP_SOURCES ${APP_SOURCE_DIR}/*.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/../res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/../res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    # target_compile_features(${APP_NAME} PRIVATE cxx_std_20) # Uses <span>

    target_sources(${APP_NAME} PRIVATE ${APP_HEADERS} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    # target_compile_definitions(${APP_NAME} PRIVATE )

    # target_compile_options(${APP_NAME} PRIVATE -fopenmp)
    # target_link_libraries(${APP_NAME} PRIVATE omp)

    target_link_libraries(${APP_NAME} PRIVATE de_sfml_system_310)
    target_link_libraries(${APP_NAME} PRIVATE de_sfml_window_310)
    target_link_libraries(${APP_NAME} PRIVATE de_sfml_graphics_310)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Building in Release mode, strip symbols")
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            COMMENT "Stripping debug symbols"
        )
    endif()

    file(COPY ../media/sfml_demo DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
