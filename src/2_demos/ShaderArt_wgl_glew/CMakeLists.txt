if (NOT WIN32)
    return()
endif()

set(APP_NAME ShaderArt-wgl)
project(${APP_NAME}-dev)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/de/CreateParams.hpp
        ${APP_SOURCE_DIR}/de/GL_debug_layer.hpp
        ${APP_SOURCE_DIR}/de/IEventReceiver.hpp
        ${APP_SOURCE_DIR}/de/mathe/FPSComputer.cpp
        ${APP_SOURCE_DIR}/de/mathe/FPSComputer.hpp
        ${APP_SOURCE_DIR}/de/OpenGLWindow_WGL.cpp
        ${APP_SOURCE_DIR}/de/OpenGLWindow_WGL.hpp
        ${APP_SOURCE_DIR}/de/renderer/Align.hpp
        ${APP_SOURCE_DIR}/de/renderer/Font5x8.cpp
        ${APP_SOURCE_DIR}/de/renderer/Font5x8.hpp
        ${APP_SOURCE_DIR}/de/renderer/Font5x8Renderer.cpp
        ${APP_SOURCE_DIR}/de/renderer/Font5x8Renderer.hpp
        ${APP_SOURCE_DIR}/de/shader/GL_Shader.cpp
        ${APP_SOURCE_DIR}/de/shader/GL_Shader.hpp
        ${APP_SOURCE_DIR}/de/shader/GL_ShaderManager.cpp
        ${APP_SOURCE_DIR}/de/shader/GL_ShaderManager.hpp
        ${APP_SOURCE_DIR}/de/shader/IShader.hpp
        ${APP_SOURCE_DIR}/de/shader/IShaderManager.hpp
        ${APP_SOURCE_DIR}/de/w32_VideoModes.cpp
        ${APP_SOURCE_DIR}/de/w32_VideoModes.hpp
        ${APP_SOURCE_DIR}/de_opengl.hpp
        ${APP_SOURCE_DIR}/de_types.hpp
        ${APP_SOURCE_DIR}/main_WGL_002_ShaderArt.cpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_000.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_001.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_002.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_003.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_004.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_005.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_006.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_007.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_008.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_009.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F01.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F02.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F03.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F04.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F05.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F06.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F07.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F08.hpp
        ${APP_SOURCE_DIR}/WallpaperShaderArt_F09.hpp
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()
    
    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

    target_sources(${APP_NAME} PRIVATE ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/camera)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/mathe)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/geometry)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/renderer)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/shader)
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/texture)
    # target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/de/window)
    #target_compile_definitions(${APP_NAME} PRIVATE UNICODE=1)    # Windows says "Cant create Window" if defined and A versions is used?
    #target_compile_definitions(${APP_NAME} PRIVATE _UNICODE=1)   # Windows says "Cant create Window" if defined and A versions is used?
    target_compile_definitions(${APP_NAME} PRIVATE OEMRESOURCE=1) # needed by winuser.h ODC
    target_compile_definitions(${APP_NAME} PRIVATE GL_EXT_PROTOTYPES=1)
    target_compile_definitions(${APP_NAME} PRIVATE GL_GLEXT_PROTOTYPES=1)
    target_compile_definitions(${APP_NAME} PRIVATE WGL_WGLEXT_PROTOTYPES=1)

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
        
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-value)
        target_compile_options(${APP_NAME} PRIVATE -Wno-old-style-cast)
        target_compile_options(${APP_NAME} PRIVATE -Wno-deprecated-declarations)
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-variable)
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-function)
        target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)
    endif()
    
    if (WIN32)
        target_link_libraries(${APP_NAME} PUBLIC glmf32)
        target_link_libraries(${APP_NAME} PUBLIC opengl32)
        target_link_libraries(${APP_NAME} PUBLIC dwmapi)
        target_link_libraries(${APP_NAME} PUBLIC shell32)
        target_link_libraries(${APP_NAME} PUBLIC user32)
        target_link_libraries(${APP_NAME} PUBLIC gdi32)
        target_link_libraries(${APP_NAME} PUBLIC kernel32)
        # target_link_libraries(${APP_NAME} PUBLIC glu32)
        # target_link_libraries(${APP_NAME} PUBLIC SetupAPI)
        # target_link_libraries(${APP_NAME} PUBLIC wmcodecdspuuid)
        # target_link_libraries(${APP_NAME} PUBLIC mfuuid)
        # target_link_libraries(${APP_NAME} PUBLIC mf)
        # target_link_libraries(${APP_NAME} PUBLIC mfplat)
        # target_link_libraries(${APP_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        # target_link_libraries(${APP_NAME} PUBLIC uuid)
        # target_link_libraries(${APP_NAME} PUBLIC ks)
        # target_link_libraries(${APP_NAME} PUBLIC ksuser)
        # target_link_libraries(${APP_NAME} PUBLIC ksguid)
        # target_link_libraries(${APP_NAME} PUBLIC winmm)
        # target_link_libraries(${APP_NAME} PUBLIC dsound)
        # target_link_libraries(${APP_NAME} PUBLIC comctl32)
        # target_link_libraries(${APP_NAME} PUBLIC oleaut32)
        # target_link_libraries(${APP_NAME} PUBLIC ole32)
        # target_link_libraries(${APP_NAME} PUBLIC advapi32)
    else()
        target_link_libraries(${APP_NAME} PUBLIC X11 Xxf86vm Xi Xcursor)
    endif()

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE de_glm)
    target_link_libraries(${APP_NAME} PRIVATE de_glew)

endif()


