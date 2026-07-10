set(APP_NAME plutovg_smiley)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES ${APP_SOURCE_DIR}/smiley.c)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
      # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${APP_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${APP_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

    target_link_libraries(${APP_NAME} PRIVATE de_plutovg_131)
    # target_link_libraries(${APP_NAME} PRIVATE de_sokol)
    # target_link_libraries(${APP_NAME} PRIVATE de_implot_0_17)
    # target_link_libraries(${APP_NAME} PRIVATE de_glfw)
    # target_link_libraries(${APP_NAME} PRIVATE de_glew)
    # target_link_libraries(${APP_NAME} PRIVATE de_nfd_130)
    # target_link_libraries(${APP_NAME} PRIVATE de_stb_image_write)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    # if (CMAKE_BUILD_TYPE STREQUAL "Release")
        # message(STATUS "Building in Release mode, strip symbols")
        # add_custom_command(TARGET ${APP_NAME} POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )
    # endif()

    # file(COPY media/plutovg_smiley DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
