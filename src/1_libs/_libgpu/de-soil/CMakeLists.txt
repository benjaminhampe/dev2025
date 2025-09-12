set(LIB_NAME de_soil)
project(${LIB_NAME}-3.2.2-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/SOIL.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)   
    set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/SOIL.c
        ${LIB_SOURCE_DIR}/image_helper.h
        ${LIB_SOURCE_DIR}/image_helper.c
        ${LIB_SOURCE_DIR}/image_DXT.h
        ${LIB_SOURCE_DIR}/image_DXT.c
    )

    #   if (WIN32)
    #      enable_language(RC)
    #      list(APPEND IRRXML_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
    #      list(APPEND IRRXML_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    #   endif()
    #

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}) 

    # set(SOIL_DEBUG_MODE 0)
    # if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # set(SOIL_DEBUG_MODE 1)
    # endif()

    # target_compile_definitions(${LIB_NAME} PUBLIC SOIL_STATIC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC SOIL_BUILD=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC SOIL_NO_GLU=1)
    #target_compile_definitions(${LIB_NAME} PUBLIC SOIL_EGL=1)

    if (MSVC)

    else()
        # target_compile_options(${LIB_NAME} PRIVATE -fno-builtin -fno-stack-protector)
        # target_link_libraries(${LIB_NAME} LINK_PRIVATE -nostdlib)
        # target_link_libraries(${LIB_NAME} PRIVATE de_stb_image)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare )
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wmaybe-uninitialized)

        target_compile_options(${LIB_NAME} PRIVATE -Wno-misleading-indentation)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-incompatible-pointer-types)
    endif()   

    target_link_libraries(${LIB_NAME} PUBLIC de_glad)
    target_link_libraries(${LIB_NAME} PUBLIC de_stb_image_aug)
endif()

# add_subdirectory(test)

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
