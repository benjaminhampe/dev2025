set(APP_NAME zstd) # We dont like to clash with system libs.
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/benchfn.c
        ${APP_SOURCE_DIR}/benchfn.h
        ${APP_SOURCE_DIR}/benchzstd.c
        ${APP_SOURCE_DIR}/benchzstd.h
        ${APP_SOURCE_DIR}/datagen.c
        ${APP_SOURCE_DIR}/datagen.h
        ${APP_SOURCE_DIR}/dibio.c
        ${APP_SOURCE_DIR}/dibio.h
        ${APP_SOURCE_DIR}/fileio.c
        ${APP_SOURCE_DIR}/fileio.h
        ${APP_SOURCE_DIR}/fileio_asyncio.c
        ${APP_SOURCE_DIR}/fileio_asyncio.h
        ${APP_SOURCE_DIR}/fileio_common.h
        ${APP_SOURCE_DIR}/fileio_types.h
        ${APP_SOURCE_DIR}/lorem.c
        ${APP_SOURCE_DIR}/lorem.h
        ${APP_SOURCE_DIR}/platform.h
        ${APP_SOURCE_DIR}/timefn.c
        ${APP_SOURCE_DIR}/timefn.h
        ${APP_SOURCE_DIR}/util.c
        ${APP_SOURCE_DIR}/util.h
        ${APP_SOURCE_DIR}/zstdcli.c
        ${APP_SOURCE_DIR}/zstdcli_trace.c
        ${APP_SOURCE_DIR}/zstdcli_trace.h
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

    # target_compile_definitions(${APP_NAME} PUBLIC -DNASM_BUILD_LIBRARY)

    # target_compile_options(${APP_NAME} PRIVATE -Wno-pedantic)

    target_link_libraries(${APP_NAME} PRIVATE de_zstd)
endif()
