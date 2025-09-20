set(LIB_NAME de_rack2_glew)
project(${LIB_NAME}-3.2.2)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(GLEW_DEBUG_MODE 0)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(GLEW_DEBUG_MODE 1)
    endif()

    set(LIB_HEADER_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
    set(LIB_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
    set(LIB_HEADERS
        "${LIB_HEADER_DIR}/de_opengl.h"
        "${LIB_HEADER_DIR}/GL/glew.h"
    )
    set(LIB_SOURCES
        "${LIB_SOURCE_DIR}/de_opengl.c"
        "${LIB_SOURCE_DIR}/glew.c"
    )

#   if (WIN32)
#      enable_language(RC)
#      list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
#      list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
#   endif()
#
    if (BUILD_SHARED)
        set(BUILD_SHARED_LIBS 1)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        #target_compile_definitions(${LIB_NAME} PUBLIC GLEW_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        target_compile_definitions(${LIB_NAME} PUBLIC GLEW_STATIC=1)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS}
                               PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GLEW=1)
    target_compile_definitions(${LIB_NAME} PUBLIC GLEW_BUILD=1)
    target_compile_definitions(${LIB_NAME} PUBLIC GLEW_NO_GLU=1)
    #target_compile_definitions(${LIB_NAME} PUBLIC GLEW_EGL=1)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -fno-builtin)
        target_compile_options(${LIB_NAME} PRIVATE -fno-stack-protector)        
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare )
        # -Wno-implicit-fallthrough
        # -Wmaybe-uninitialized
        #target_link_libraries(${LIB_NAME} LINK_PRIVATE -nostdlib)
    endif()
    
    # Desktop OpenGL ( apple osx standard is 3.2, everybody else can go up to 4.5 or so )
    if (WIN32)
        target_link_libraries(${LIB_NAME} PUBLIC glmf32 opengl32) # glu32
    else()
        target_link_libraries(${LIB_NAME} PUBLIC GL GLU Xxf86vm)
    endif()
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
