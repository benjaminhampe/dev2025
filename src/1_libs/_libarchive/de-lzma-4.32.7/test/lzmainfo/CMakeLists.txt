set(APP_NAME lzmainfo)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
    set(APP_SOURCES ${APP_SOURCE_DIR}/lzmainfo.c)

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/resource.rc)
    endif()
    
    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    target_compile_definitions(${APP_NAME} PRIVATE HAVE_CONFIG_H)
    target_compile_definitions(${APP_NAME} PRIVATE HAVE_ERRNO_H)

    # target_compile_options(${APP_NAME} PRIVATE -Wno-implicit-fallthrough)

    target_link_libraries(${APP_NAME} PRIVATE de_lzma)

endif()
