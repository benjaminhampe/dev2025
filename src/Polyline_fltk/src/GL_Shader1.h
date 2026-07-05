#pragma once
#include <cstdint>

struct GL_Shader1
{
    uint32_t shaderProgram = 0;
    uint32_t m_vao = 0;
    uint32_t vertexBuffer = 0;
    int32_t positionUniform = -1;
    int32_t colourAttribute = -1;
    int32_t positionAttribute = -1;

    void draw(float x, float y);

    void update();
};
