set(APP_NAME Retropad)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/main_retropad.c
        ${APP_SOURCE_DIR}/file_io.c
        ${APP_SOURCE_DIR}/file_io.h
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    # target_compile_definitions(${APP_NAME} PRIVATE )

    target_link_options(${APP_NAME} PRIVATE -mwindows)

    # if (MSVC)
    # else()
        # #

        # if (WIN32)
            # # target_link_libraries(${APP_NAME} PRIVATE shcore) # for DPI‑Awareness
            # # target_link_libraries(${APP_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # # target_link_libraries(${APP_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    # endif()

    # target_link_libraries(${APP_NAME} PRIVATE DarkImage_2604)
    target_link_libraries(${APP_NAME} PRIVATE comctl32)
    target_link_libraries(${APP_NAME} PRIVATE kernel32)
    target_link_libraries(${APP_NAME} PRIVATE user32)
    target_link_libraries(${APP_NAME} PRIVATE gdi32)
    target_link_libraries(${APP_NAME} PRIVATE winspool)
    target_link_libraries(${APP_NAME} PRIVATE shell32)
    target_link_libraries(${APP_NAME} PRIVATE ole32)
    target_link_libraries(${APP_NAME} PRIVATE oleaut32)
    target_link_libraries(${APP_NAME} PRIVATE uuid)
    target_link_libraries(${APP_NAME} PRIVATE comdlg32)
    target_link_libraries(${APP_NAME} PRIVATE advapi32)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Building in Release mode, strip symbols")
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            COMMENT "Stripping debug symbols"
        )
    endif()

    # file(COPY media/earcut_viz DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
