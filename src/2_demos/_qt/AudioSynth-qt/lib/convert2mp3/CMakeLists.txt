set(APP_NAME convert2mp3)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file( GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/icon.rc)
    endif()

    message("${BoldYellow}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-value)
    endif()

    # target_link_libraries(${APP_NAME} PRIVATE DarkVLC)
    #target_link_libraries(${LIB_NAME} PUBLIC de_glm)         # Math
    #target_link_libraries(${LIB_NAME} PUBLIC de_fontawesome) # Font
    #target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)    # XML
    # target_link_libraries(${APP_NAME} PRIVATE AS_DarkImage)     # Logger
    target_link_libraries(${APP_NAME} PRIVATE de_2501)
    # target_link_libraries(${APP_NAME} PRIVATE DarkAudio)     # Logger
    target_link_libraries(${APP_NAME} PRIVATE de_ffmpeg)     # AudioDecoder + VideoDecoder
    target_link_libraries(${APP_NAME} PRIVATE de_lame)       # AudioEncoder MP3

endif()
