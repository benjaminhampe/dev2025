set(LIB_NAME de_lilv) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/lilv/lilv.h
    )
    
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/collections.c
        ${LIB_SOURCE_DIR}/dylib.c
        ${LIB_SOURCE_DIR}/instance.c
        ${LIB_SOURCE_DIR}/lib.c
        ${LIB_SOURCE_DIR}/load_skimmer.c
        ${LIB_SOURCE_DIR}/node.c
        ${LIB_SOURCE_DIR}/node_hash.c
        ${LIB_SOURCE_DIR}/node_skimmer.c
        ${LIB_SOURCE_DIR}/plugin.c
        ${LIB_SOURCE_DIR}/pluginclass.c
        ${LIB_SOURCE_DIR}/port.c
        ${LIB_SOURCE_DIR}/query.c
        ${LIB_SOURCE_DIR}/scalepoint.c
        ${LIB_SOURCE_DIR}/state.c
        ${LIB_SOURCE_DIR}/string_util.c
        ${LIB_SOURCE_DIR}/syntax_skimmer.c
        ${LIB_SOURCE_DIR}/sys_util.c
        ${LIB_SOURCE_DIR}/type_skimmer.c
        ${LIB_SOURCE_DIR}/ui.c
        ${LIB_SOURCE_DIR}/uris.c
        ${LIB_SOURCE_DIR}/world.c        
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
        target_compile_definitions(${LIB_NAME} PUBLIC LILV_STATIC=1)    
        # target_compile_definitions(${LIB_NAME} PRIVATE LILV_DYN_MANIFEST=1)
    # endif()

    target_compile_definitions(${LIB_NAME} PRIVATE LILV_INTERNAL=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_link_libraries(${LIB_NAME} PUBLIC de_zix)
    target_link_libraries(${LIB_NAME} PUBLIC de_sord)
    target_link_libraries(${LIB_NAME} PUBLIC de_serd)
    target_link_libraries(${LIB_NAME} PUBLIC de_lv2)
    target_link_libraries(${LIB_NAME} PUBLIC de_sratom)
endif()
