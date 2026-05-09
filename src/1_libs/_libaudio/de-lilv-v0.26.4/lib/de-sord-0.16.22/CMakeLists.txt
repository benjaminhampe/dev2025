set(LIB_NAME de_sord) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/sord/sord.h
        # ${LIB_HEADER_DIR}/sord/sordmm.hpp
    )
    
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/sord.c
        ${LIB_SOURCE_DIR}/sordi.c
        ${LIB_SOURCE_DIR}/sord_config.h
        ${LIB_SOURCE_DIR}/sord_internal.h
        ${LIB_SOURCE_DIR}/sord_validate.c
        ${LIB_SOURCE_DIR}/syntax.c
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
        target_compile_definitions(${LIB_NAME} PUBLIC SORD_STATIC=1)
        # target_compile_definitions(${LIB_NAME} PRIVATE LILV_DYN_MANIFEST=1)
    # endif()

    target_compile_definitions(${LIB_NAME} PRIVATE SORD_INTERNAL=1)
    target_compile_definitions(${LIB_NAME} PRIVATE SORD_VERSION="0.16.22")
    
    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_link_libraries(${LIB_NAME} PUBLIC de_serd)
    target_link_libraries(${LIB_NAME} PUBLIC de_zix)
endif()
