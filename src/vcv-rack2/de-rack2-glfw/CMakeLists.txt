set(LIB_NAME de_rack2_glfw)
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(GLFW_DEBUG_MODE 0)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(GLFW_DEBUG_MODE 1)
    endif()

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/GLFW/glfw3.h
        ${LIB_HEADER_DIR}/GLFW/glfw3native.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/internal.h 
        ${LIB_SOURCE_DIR}/platform.h 
        ${LIB_SOURCE_DIR}/mappings.h
        ${LIB_SOURCE_DIR}/context.c 
        ${LIB_SOURCE_DIR}/init.c 
        ${LIB_SOURCE_DIR}/input.c 
        ${LIB_SOURCE_DIR}/monitor.c 
        ${LIB_SOURCE_DIR}/platform.c 
        ${LIB_SOURCE_DIR}/vulkan.c 
        ${LIB_SOURCE_DIR}/window.c
        ${LIB_SOURCE_DIR}/egl_context.c 
        ${LIB_SOURCE_DIR}/osmesa_context.c 
        ${LIB_SOURCE_DIR}/null_platform.h 
        ${LIB_SOURCE_DIR}/null_joystick.h
        ${LIB_SOURCE_DIR}/null_init.c 
        ${LIB_SOURCE_DIR}/null_monitor.c 
        ${LIB_SOURCE_DIR}/null_window.c 
        ${LIB_SOURCE_DIR}/null_joystick.c
    )

    if (WIN32)
        list(APPEND LIB_SOURCES 
            ${LIB_SOURCE_DIR}/win32_time.h 
            ${LIB_SOURCE_DIR}/win32_thread.h 
            ${LIB_SOURCE_DIR}/win32_module.c
            ${LIB_SOURCE_DIR}/win32_time.c 
            ${LIB_SOURCE_DIR}/win32_thread.c
            ${LIB_SOURCE_DIR}/win32_platform.h 
            ${LIB_SOURCE_DIR}/win32_joystick.h 
            ${LIB_SOURCE_DIR}/win32_init.c
            ${LIB_SOURCE_DIR}/win32_joystick.c 
            ${LIB_SOURCE_DIR}/win32_monitor.c 
            ${LIB_SOURCE_DIR}/win32_window.c
            ${LIB_SOURCE_DIR}/wgl_context.c
        )
    endif()

    # if (BUILD_SHARED)
    # if (WIN32)
    # enable_language(RC)
    # list(APPEND IRRXML_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
    # list(APPEND IRRXML_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()
    # message("${BoldBlue}[benni] SHARED_LIBRARY: ${LIB_NAME}${ColourReset}")
    # add_library(${LIB_NAME} SHARED "")
    # target_compile_definitions(${LIB_NAME} PUBLIC -DGLFW_DLL=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)
    # # target_compile_definitions(${LIB_NAME} PUBLIC -D_GLFW_BUILD_DLL=1)
    # else()
    message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} STATIC "")
    # endif()

    target_compile_definitions(${LIB_NAME} PUBLIC _GLFW_USE_CONFIG_H=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC GLFW_INCLUDE_NONE=1)
    target_compile_definitions(${LIB_NAME} PUBLIC OEMRESOURCE=1) # needed by winuser.h ODC
    
    # GLFW_LIBRARY_TYPE
    # GLFW_BUILD_SHARED_LIBRARY
    # GLFW_BUILD_WIN32
    
    #target_compile_definitions(${LIB_NAME} PUBLIC GLFW_INCLUDE_GLCOREARB=1)
    #target_compile_definitions(${LIB_NAME} PUBLIC GLFW_INCLUDE_ES32=1)
    #target_compile_definitions(${LIB_NAME} PUBLIC GLFW_INCLUDE_GLU=1)
    #target_compile_definitions(${LIB_NAME} PUBLIC GLFW_INCLUDE_GLEXT=1)

    if (WIN32)
        list(APPEND LIB_SOURCES ${LIB_SOURCES_WIN32})
        target_compile_definitions(${LIB_NAME} PUBLIC _GLFW_WIN32=1)
        target_compile_definitions(${LIB_NAME} PUBLIC GLFW_BUILD_WIN32=1)
        target_compile_definitions(${LIB_NAME} PRIVATE _GLFW_USE_VULKAN=0)
        # target_compile_definitions(${LIB_NAME} PUBLIC GLFW_EXPOSE_NATIVE_WIN32=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC GLFW_EXPOSE_NATIVE_WGL=1)
        # target_compile_definitions(${LIB_NAME} PRIVATE _GLFW_VULKAN_STATIC=1)
        #target_compile_definitions(${LIB_NAME} PUBLIC _GLFW_USE_HYBRID_HPG=1)
        # if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # target_compile_definitions(${LIB_NAME} PUBLIC -D_GLFW_EGL_LIBRARY=libEGLd.dll)
        # else()
        # # if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        # # target_compile_definitions(${LIB_NAME} PUBLIC -D_GLFW_EGL_LIBRARY=libEGL)
        # # endif()
        # target_compile_definitions(${LIB_NAME} PUBLIC -D_GLFW_EGL_LIBRARY=libEGL.dll)
        # endif()
    endif()
    if (UNIX)
        #define _GLFW_X11
        #define _GLFW_WAYLAND
        #define _GLFW_OSMESA
        #define HAVE_XKBCOMMON_COMPOSE_H
        #define HAVE_MEMFD_CREATE
    endif()

    if(MAC)
        #define _GLFW_COCOA
    endif()

    # target_compile_options(${LIB_NAME} PRIVATE -O3)

    if (MSVC)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-incompatible-pointer-types)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/GLFW)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    # target_link_libraries(${LIB_NAME} PUBLIC de_opengl)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glad_430)
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-incompatible-pointer-types)
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare )
    # -Wno-implicit-fallthrough
    # -Wmaybe-uninitialized

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
