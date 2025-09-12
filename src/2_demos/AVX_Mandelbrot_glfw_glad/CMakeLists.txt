set(APP_NAME AVX_Mandelbrot_glfw_glad)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    add_subdirectory(libDeutschland_glfw_glad)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES "${APP_SOURCE_DIR}/*.[h,c]pp")

    enable_language(RC)
    list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
    list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    if (MSVC)
    else()
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-value)
        target_compile_options(${APP_NAME} PRIVATE -Wno-old-style-cast)
        target_compile_options(${APP_NAME} PRIVATE -O3 -march=native -mavx2 -mfma)

        # target_compile_definitions(${APP_NAME} PUBLIC GL_EXT_PROTOTYPES=1)
        # target_compile_definitions(${APP_NAME} PUBLIC GL_GLEXT_PROTOTYPES=1)
        # target_compile_definitions(${APP_NAME} PUBLIC WGL_WGLEXT_PROTOTYPES=1)

        target_link_options(${APP_NAME} PRIVATE -mwindows)

    endif()

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)    
    target_link_libraries(${APP_NAME} PRIVATE Deutschland_glfw_glad)

endif()

