set(LIB_NAME de_serd) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/serd/serd.h
    )
    
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/attributes.h
        ${LIB_SOURCE_DIR}/base64.c
        ${LIB_SOURCE_DIR}/base64.h
        ${LIB_SOURCE_DIR}/byte_sink.h
        ${LIB_SOURCE_DIR}/byte_source.c
        ${LIB_SOURCE_DIR}/byte_source.h
        ${LIB_SOURCE_DIR}/env.c
        ${LIB_SOURCE_DIR}/n3.c
        ${LIB_SOURCE_DIR}/node.c
        ${LIB_SOURCE_DIR}/ntriples.h
        ${LIB_SOURCE_DIR}/reader.c
        ${LIB_SOURCE_DIR}/reader.h
        ${LIB_SOURCE_DIR}/read_utf8.c
        ${LIB_SOURCE_DIR}/read_utf8.h
        ${LIB_SOURCE_DIR}/serdi.c
        ${LIB_SOURCE_DIR}/serd_config.h
        ${LIB_SOURCE_DIR}/serd_internal.h
        ${LIB_SOURCE_DIR}/stack.h
        ${LIB_SOURCE_DIR}/string.c
        ${LIB_SOURCE_DIR}/string_utils.h
        ${LIB_SOURCE_DIR}/system.c
        ${LIB_SOURCE_DIR}/system.h
        ${LIB_SOURCE_DIR}/try.h
        ${LIB_SOURCE_DIR}/turtle.h
        ${LIB_SOURCE_DIR}/uri.c
        ${LIB_SOURCE_DIR}/uri_utils.h
        ${LIB_SOURCE_DIR}/warnings.h
        ${LIB_SOURCE_DIR}/writer.c
    )

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")

        # # if(WIN32)
            # # enable_language(RC)
            # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
            # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # # endif()
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        target_compile_definitions(${LIB_NAME} PUBLIC SERD_STATIC=1)
        # target_compile_definitions(${LIB_NAME} PRIVATE LILV_DYN_MANIFEST=1)
    # endif()

    target_compile_definitions(${LIB_NAME} PRIVATE SERD_INTERNAL=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

endif()
