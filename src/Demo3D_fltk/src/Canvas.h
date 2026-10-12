#pragma once
#include <de/Core.h>
#include <FL/Fl_Gl_Window.H>

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
// Note: GLEW_STATIC is defined by CMake if the static lib is linked
#  include <GL/glew.h>
#endif
#include <FL/gl.h> // for gl_texture_reset()

class Canvas : public Fl_Gl_Window
{
    GLuint shaderProgram;
    GLuint vertexArrayObject;
    GLuint vertexBuffer;
    GLint positionUniform;
    GLint colourAttribute;
    GLint positionAttribute;
    int gl_version_major;

public:
    Canvas(int x, int y, int w, int h);

    void resize(int X, int Y, int W, int H) override;

    void draw(void) override;

    int handle(int event) override;

    void reset();


    void initGL();

    void drawGL();

    void destroyGL();
};

