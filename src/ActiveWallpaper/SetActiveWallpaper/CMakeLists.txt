set(APP_NAME SetActiveWallpaper)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    message("${BoldRed}[benni] Benni asks __MINGW32__=${__MINGW32__} : ${APP_NAME}${ColourReset}")

    if (__MINGW32__)
    message("${BoldRed}[benni] Compiler gives __MINGW32__ : ${APP_NAME}${ColourReset}")
    endif()


    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    set(APP_SOURCES ${APP_SOURCE_DIR}/SetActiveWallpaper.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
		
    if (WIN32)
		target_compile_definitions(${APP_NAME} PRIVATE _UNICODE=1)
		target_compile_definitions(${APP_NAME} PRIVATE UNICODE=1)
        target_link_libraries(${APP_NAME} PRIVATE comctl32 user32 gdi32 kernel32 shell32)
        target_link_libraries(${APP_NAME} PRIVATE ole32 oleaut32)
    else()
        target_link_libraries(${APP_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    
    # if (CMAKE_BUILD_TYPE STREQUAL "Release")
        # message(STATUS "Building in Release mode, strip symbols")
        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        # add_custom_command(
            # TARGET ${APP_NAME}
            # POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )
    # endif()

    # file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
