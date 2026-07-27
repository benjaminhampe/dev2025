set(APP_NAME wav2mp3_fltk)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # set(APP_SOURCES
        # ${APP_SOURCE_DIR}/main_wav2mp3_fltk.cpp
        # ${APP_SOURCE_DIR}/de/Core.cpp
        # ${APP_SOURCE_DIR}/de/Core.h
    # )

    file(GLOB_RECURSE APP_HEADERS ${APP_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE APP_SOURCES ${APP_SOURCE_DIR}/*.cpp)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
      # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_HEADERS} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    # target_compile_definitions(${APP_NAME} PRIVATE )

    if (MSVC)
    else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${APP_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${APP_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

    target_link_libraries(${APP_NAME} PRIVATE DarkImage_2604)
    target_link_libraries(${APP_NAME} PRIVATE de_fltk_145)
    target_link_libraries(${APP_NAME} PRIVATE de_lame_3100)
    target_link_libraries(${APP_NAME} PRIVATE de_dr_wav)
    target_link_libraries(${APP_NAME} PRIVATE de_sndfile)
    target_link_libraries(${APP_NAME} PRIVATE de_opusfile)
    target_link_libraries(${APP_NAME} PRIVATE de_flac)

    # target_link_libraries(${APP_NAME} PRIVATE de_soxr) # This shit code is pure Hell, linker error _soxr_rdft. That shit fn does not exist but is used.
    target_link_libraries(${APP_NAME} PRIVATE de_speexdsp)
    target_link_libraries(${APP_NAME} PRIVATE de_resample)
    target_link_libraries(${APP_NAME} PRIVATE de_signalsmith)
    target_link_libraries(${APP_NAME} PRIVATE de_r8brain)

    # target_link_libraries(${APP_NAME} PRIVATE DarkSound)
    # target_link_libraries(${APP_NAME} PRIVATE DarkMP4)
    target_link_libraries(${APP_NAME} PRIVATE de_alac)      # MP4 ALAC
    target_link_libraries(${APP_NAME} PRIVATE de_faad2)     # MP4 AAC
    target_link_libraries(${APP_NAME} PRIVATE de_opus)      # MP4 opus

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    # if (CMAKE_BUILD_TYPE STREQUAL "Release")
        # message(STATUS "Building in Release mode, strip symbols")
        # add_custom_command(TARGET ${APP_NAME} POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )
    # endif()

    # file(COPY media/earcut_viz DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
