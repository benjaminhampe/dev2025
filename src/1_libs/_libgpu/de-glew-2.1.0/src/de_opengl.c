#include <de_opengl.h>

static int g_isDesktopOpenGLInitialized = 0;

void ensureDesktopOpenGL()
{
    if ( g_isDesktopOpenGLInitialized > 0 ) return;

    // Use glewExperimental = GL_TRUE, when...
    // ✅ You're using OpenGL 3.0+ or core profile contexts.
    // ✅ You're working with libraries like NanoVG, ImGui, or modern shaders.
    // ✅ You're targeting cross-platform builds.

    glewExperimental = GL_TRUE;

    glewInit();

    //printf("Ensured Desktop OpenGL ok.\n"); fflush( stdout );

    g_isDesktopOpenGLInitialized = 1;
}

long glGetInteger( unsigned long query )
{
    GLint value[4] = { 0,0,0,0 };
    glGetIntegerv( query, value );
    return value[0];
}

/*
void ensureDesktopOpenGL()
{
   if ( g_isDesktopOpenGLInitialized ) return;
   glewInit();
   //printf("Ensured Desktop OpenGL ok.\n"); fflush( stdout );
   g_isDesktopOpenGLInitialized = 1;
}

int32_t glGetInteger( uint32_t query )
{
    int32_t value[4] = { 0,0,0,0 };
    glGetIntegerv( query, value );
    return value[0];
}
*/
