set(LIB_NAME de_juce_opengl)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_opengl)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_opengl)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/juce_opengl.h
        ${LIB_HEADER_DIR}/juce_opengl_config.h
        ${LIB_HEADER_DIR}/geometry/juce_Draggable3DOrientation.h
        ${LIB_HEADER_DIR}/geometry/juce_Matrix3D.h
        ${LIB_HEADER_DIR}/geometry/juce_Quaternion.h
        ${LIB_HEADER_DIR}/geometry/juce_Vector3D.h
        ${LIB_HEADER_DIR}/native/juce_OpenGLExtensions.h
        #${LIB_HEADER_DIR}/native/juce_OpenGL_android.h
        #${LIB_HEADER_DIR}/native/juce_OpenGL_ios.h
        #${LIB_HEADER_DIR}/native/juce_OpenGL_linux_X11.h
        #${LIB_HEADER_DIR}/native/juce_OpenGL_osx.h
        ${LIB_HEADER_DIR}/native/juce_OpenGL_win32.h
        #${LIB_HEADER_DIR}/opengl/juce_gl.h
        #${LIB_HEADER_DIR}/opengl/juce_gles2.h
        #${LIB_HEADER_DIR}/opengl/juce_khrplatform.h
        ${LIB_HEADER_DIR}/opengl/juce_checkGLError.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLContext.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLFrameBuffer.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLGraphicsContext.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLHelpers.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLImage.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLPixelFormat.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLRenderer.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLShaderProgram.h
        ${LIB_HEADER_DIR}/opengl/juce_OpenGLTexture.h
        #${LIB_HEADER_DIR}/opengl/juce_wgl.h
        ${LIB_HEADER_DIR}/utils/juce_OpenGLAppComponent.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/juce_opengl.cpp
        ${LIB_SOURCE_DIR}/native/juce_OpenGLExtensions.cpp
        #${LIB_SOURCE_DIR}/opengl/juce_gl.cpp
        #${LIB_SOURCE_DIR}/opengl/juce_gles2.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_checkGLError.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLContext.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLFrameBuffer.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLGraphicsContext.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLHelpers.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLImage.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLPixelFormat.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLShaderProgram.cpp
        ${LIB_SOURCE_DIR}/opengl/juce_OpenGLTexture.cpp
        ${LIB_SOURCE_DIR}/utils/juce_OpenGLAppComponent.cpp
    )

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

    # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)

    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_FLAC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_OGGVORBIS=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDBURNER=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDREADER=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CAMERA=0) # need #include <dshow.h><qedit.h><dshowasf.h>

    # target_link_libraries(${LIB_NAME} PUBLIC opengl32 glu32)
    target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    target_link_libraries(${LIB_NAME} PUBLIC de_juce_gui_extra)

endif()

# add_subdirectory(test)
