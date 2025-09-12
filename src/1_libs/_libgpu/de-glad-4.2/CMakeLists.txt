set(LIB_NAME de_glad)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/glad/glad.h)
    set(LIB_SOURCES ${LIB_SOURCE_DIR}/glad/glad.c)

    if (WIN32)
    enable_language(RC)
    list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
    list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # list(APPEND LIB_HEADERS ${LIB_HEADER_DIR}/EGL/egl.h)
    # list(APPEND LIB_HEADERS ${LIB_HEADER_DIR}/EGL/eglext.h)
    # list(APPEND LIB_HEADERS ${LIB_HEADER_DIR}/EGL/eglplatform.h)
    # list(APPEND LIB_HEADERS ${LIB_HEADER_DIR}/de/EGL_Validate.hpp)
    # list(APPEND LIB_SOURCES ${LIB_SOURCE_DIR}/de/EGL_Validate.cpp)

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_GLFW} PUBLIC GLFW_DLL=1)
        # target_compile_definitions(${LIB_GLFW} PUBLIC _UNICODE=1)
        # target_compile_definitions(${LIB_GLFW} PUBLIC UNICODE=1)
        # # target_compile_definitions(${LIB_GLFW} PUBLIC _GLFW_BUILD_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # Desktop OpenGL ( apple osx standard is 3.2, everybody else can go up to 4.5 or so )
    if (WIN32)
        target_link_libraries(${LIB_NAME} PUBLIC glmf32 opengl32) # glu32
    else()
        target_link_libraries(${LIB_NAME} PUBLIC GL GLU Xxf86vm)
    endif()

endif()

# add_subdirectory(test)
