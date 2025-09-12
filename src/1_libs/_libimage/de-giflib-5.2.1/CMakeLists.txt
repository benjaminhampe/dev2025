set(LIB_NAME de_gif5)
project(${LIB_NAME}-5.2.1-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/gif5)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/gif5)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/gif_lib.h
        ${LIB_HEADER_DIR}/gif_hash.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/dgif_lib.c
        ${LIB_SOURCE_DIR}/egif_lib.c
        ${LIB_SOURCE_DIR}/gif_err.c
        ${LIB_SOURCE_DIR}/gif_font.c
        ${LIB_SOURCE_DIR}/gif_hash.c
        ${LIB_SOURCE_DIR}/gif_lib_private.h
        ${LIB_SOURCE_DIR}/gifalloc.c
        ${LIB_SOURCE_DIR}/openbsd-reallocarray.c
        ${LIB_SOURCE_DIR}/qprintf.c
        ${LIB_SOURCE_DIR}/quantize.c
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
    
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBGIF=1)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare )
            # -Wno-implicit-fallthrough
            # -Wmaybe-uninitialized

        # target_compile_definitions(${LIB_NAME} PUBLIC -DLIB_BUILD_LIBRARY)
        # target_compile_options(${LIB_NAME} PRIVATE -O3)
    endif()

    # target_link_libraries(${LIB_NAME} PRIVATE de_zlib) # delib_harfbuzz
    
    ### add_custom_command(TARGET foo POST_BUILD
    ###    COMMAND ${CMAKE_COMMAND} -E copy
    ###            ${CMAKE_SOURCE_DIR}/test/input.txt
    ###            ${CMAKE_CURRENT_BINARY_DIR}/input.txt)
endif()

# add_subdirectory(test)
# set(LIB_APP_SOURCES
# ${LIB_SOURCE_DIR}/getarg.c
# ${LIB_SOURCE_DIR}/getarg.h
# ${LIB_SOURCE_DIR}/gif2rgb.c
# ${LIB_SOURCE_DIR}/gifbg.c
# ${LIB_SOURCE_DIR}/gifbuild.c
# ${LIB_SOURCE_DIR}/gifclrmp.c
# ${LIB_SOURCE_DIR}/gifcolor.c
# ${LIB_SOURCE_DIR}/gifecho.c
# ${LIB_SOURCE_DIR}/giffilter.c
# ${LIB_SOURCE_DIR}/giffix.c
# ${LIB_SOURCE_DIR}/gifhisto.c
# ${LIB_SOURCE_DIR}/gifinto.c
# ${LIB_SOURCE_DIR}/gifsponge.c
# ${LIB_SOURCE_DIR}/giftext.c
# ${LIB_SOURCE_DIR}/giftool.c
# ${LIB_SOURCE_DIR}/gifwedge.c
# )

## Packaging
#set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
#set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The FreeType font rendering library.")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/docs/LICENSE.TXT")

#set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
#set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
#set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
#set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

#if (WIN32)
#  set(CPACK_GENERATOR ZIP)
#else()
#  set(CPACK_GENERATOR TGZ)
#endif()

#set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Libraries")
#set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C/C++ Headers")
#set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION
#  "Library used to build programs which use FreeType")
#set(CPACK_COMPONENT_HEADERS_DESCRIPTION
#  "C/C++ header files for use with FreeType")
#set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)
#set(CPACK_COMPONENT_LIBRARIES_GROUP "Development")
#set(CPACK_COMPONENT_HEADERS_GROUP "Development")

#include(CPack)
