set(LIB_NAME de_expat)
project(${LIB_NAME}-3.2.2-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/expat)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/ascii.h
        ${LIB_HEADER_DIR}/asciitab.h
        ${LIB_HEADER_DIR}/expat.h
        ${LIB_HEADER_DIR}/expat_external.h
        ${LIB_HEADER_DIR}/iasciitab.h
        ${LIB_HEADER_DIR}/internal.h
        ${LIB_HEADER_DIR}/latin1tab.h
        ${LIB_HEADER_DIR}/macconfig.h
        ${LIB_HEADER_DIR}/nametab.h
        ${LIB_HEADER_DIR}/utf8tab.h
        ${LIB_HEADER_DIR}/winconfig.h
        ${LIB_HEADER_DIR}/xmlrole.h
        ${LIB_HEADER_DIR}/xmltok.h
        ${LIB_HEADER_DIR}/xmltok_impl.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/xmlparse.c
        ${LIB_SOURCE_DIR}/xmlrole.c
        ${LIB_SOURCE_DIR}/xmltok.c
        ${LIB_SOURCE_DIR}/xmltok_impl.c
        ${LIB_SOURCE_DIR}/xmltok_ns.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()
    
    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC -DGLFW_DLL=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC -D_GLFW_BUILD_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PRIVATE XML_UNICODE=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_MEMMOVE=1)

    if (MSVC)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
    
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare )
    # -Wno-implicit-fallthrough
    # -Wmaybe-uninitialized

endif()
