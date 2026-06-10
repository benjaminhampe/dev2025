#pragma once
#include <DarkImage.h>

// ==========================================================
struct TestRenderer
// ==========================================================
{
    uint32_t program = 0;

    static uint32_t compile(uint32_t type, const char* src);

    void initializeGL();

    void uninitGL();

    void resizeGL(int w, int h);

    void paintGL();
};

