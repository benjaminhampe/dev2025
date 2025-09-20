set(APP_NAME bsdtar)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/bsdtar.h
        ${APP_SOURCE_DIR}/bsdtar.c
        ${APP_SOURCE_DIR}/bsdtar_platform.h
        ${APP_SOURCE_DIR}/cmdline.c
        ${APP_SOURCE_DIR}/config_freebsd.h
        ${APP_SOURCE_DIR}/creation_set.c
        ${APP_SOURCE_DIR}/read.c
        ${APP_SOURCE_DIR}/subst.c
        ${APP_SOURCE_DIR}/util.c
        ${APP_SOURCE_DIR}/write.c
    )

    if (WIN32)
        list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/bsdtar_windows.c)
        list(APPEND APP_SOURCES ${APP_SOURCE_DIR}/bsdtar_windows.h)        
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    # target_include_directories(${APP_NAME} PRIVATE ${APP_HEADER_DIR}/archive)

	# target_compile_definitions(${APP_NAME} PUBLIC HAVE_LIBBSDTAR=1)
	
    target_link_libraries(${APP_NAME} PRIVATE de_archive)
    target_link_libraries(${APP_NAME} PRIVATE de_archive_fe)
    target_link_libraries(${APP_NAME} PRIVATE de_pcre2)
    # target_link_libraries(${APP_NAME} PRIVATE de_zlib)

    if(MSVC)
    else()
        # target_compile_options(${APP_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-maybe-uninitialized)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
    endif()
    
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # # message(STATUS "Building in Release mode, strip symbols")
        # # --strip-debug : Removes only debugging symbols.
        # # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        # add_custom_command(
            # TARGET ${APP_NAME}
            # POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )

    # endif()
    
    # file(COPY ${APP_HEADERS} DESTINATION ${APP_INCLUDE_DIR}) # For the SDK

endif()

# add_subdirectory(test)
