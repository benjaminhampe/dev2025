#include "TestRenderer.h"
#include <de_opengl.h>

uint32_t
TestRenderer::compile(uint32_t type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[2048];
        glGetShaderInfoLog(s, 2048, nullptr, log);
        DE_ERROR("Shader error: ",log)
    }
    return s;
}

void
TestRenderer::initializeGL()
{
    ensureDesktopOpenGL();

    const char* vs = R"(
        #version 150 core
        const vec2 verts[3] = vec2[](
            vec2(-1.0, -1.0),
            vec2( 3.0, -1.0),
            vec2(-1.0,  3.0)
        );
        void main() {
            gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
        }
    )";

    const char* fs = R"(
        #version 150 core
        out vec4 fragColor;
        void main() {
            fragColor = vec4(0.2, 0.8, 0.3, 1.0);
        }
    )";

    GLuint v = compile(GL_VERTEX_SHADER, vs);
    GLuint f = compile(GL_FRAGMENT_SHADER, fs);

    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);

    glDeleteShader(v);
    glDeleteShader(f);
}

void
TestRenderer::uninitGL()
{
    glDeleteProgram(program);
    program = 0;
}

void
TestRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void
TestRenderer::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
