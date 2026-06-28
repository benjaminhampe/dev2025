#pragma once
#include <cstdint>

struct GL_RoundRectShader
{
    uint32_t m_shaderId = 0;
    uint32_t m_vao = 0;
    int32_t u_screenSize = -1;
    int32_t u_fillColor = -1;
    int32_t u_rectPos = -1;
    int32_t u_rectSize = -1;
    int32_t u_rectRadius = -1;
    int32_t u_aaWidth = -1;

    void draw(
            int screenW = 1024,
            int screenH = 768,
            int x = 10,
            int y = 10,
            int w = 300,
            int h = 200,
            int radius = 8,
            float r = 0.2f,
            float g = 0.6f,
            float b = 0.3f,
            float a = 1.0f,
            float aaWidth = 2.0f );
};
