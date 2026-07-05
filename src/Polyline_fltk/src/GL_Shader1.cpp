#include "GL_Shader1.h"
#include <de_opengl.h>
#include "GL_Util.h"

void GL_Shader1::draw(float x, float y)
{
    if (!shaderProgram)
    {
        auto vs = R"(#version 330
        uniform vec2 p;
        in vec4 position;
        in vec4 colour;
        out vec4 colourV;

        void main()
        {
            colourV = colour;
            gl_Position = vec4(p, 0.0, 0.0) + position;
        })";

        auto fs = R"(#version 330
        in vec4 colourV;
        out vec4 fragColour;
        void main()
        {
            fragColour = colourV;
        })";

        shaderProgram = GL_Util::createShader("GL_Shader1",vs,fs);
        positionUniform = glGetUniformLocation(shaderProgram, "p");
        colourAttribute = glGetAttribLocation(shaderProgram, "colour");
        positionAttribute = glGetAttribLocation(shaderProgram, "position");
        // Upload vertices (1st four values in a row) and colours (following four values)
        GLfloat vertexData[]= { -0.5,-0.5,0.0,1.0,   1.0,0.0,0.0,1.0,
        -0.5, 0.5,0.0,1.0,   0.0,1.0,0.0,1.0,
        0.5, 0.5,0.0,1.0,   0.0,0.0,1.0,1.0,
        0.5,-0.5,0.0,1.0,   1.0,1.0,1.0,1.0};
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4*8*sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

        glEnableVertexAttribArray((GLuint)positionAttribute);
        glEnableVertexAttribArray((GLuint)colourAttribute  );
        glVertexAttribPointer((GLuint)positionAttribute, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
        glVertexAttribPointer((GLuint)colourAttribute  , 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (char*)0+4*sizeof(GLfloat));
    }
    glUseProgram(shaderProgram);
    GLfloat p[]={x,y};
    glUniform2fv(positionUniform, 1, (const GLfloat *)&p);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GL_Shader1::update()
{
    static float factor = 1.1f;

    if (shaderProgram)
    {
        GLfloat data[4];
        glGetBufferSubData(GL_ARRAY_BUFFER, 0, 4*sizeof(GLfloat), data);
        if (data[0] < -0.88 || data[0] > -0.5)
        factor = 1/factor;
        data[0] *= factor;
        glBufferSubData(GL_ARRAY_BUFFER, 0, 4*sizeof(GLfloat), data);
        glGetBufferSubData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), 4*sizeof(GLfloat), data);
        data[0] *= factor;
        glBufferSubData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), 4*sizeof(GLfloat), data);
    }
}
