#pragma once
#include "IOpenGLBackend.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

class Backend_WGL : public IOpenGLBackend
{
public:
    HWND hwnd = nullptr;
    HDC  hdc  = nullptr;
    HGLRC hgl = nullptr;

    bool createWindow(void* parentHandle, int x, int y, int w, int h) override;
    void destroy() override;
    void resize(int x, int y, int w, int h) override;
    void makeCurrent() override;
    void doneCurrent() override;
    void swapBuffers() override;

    // void render() override;
};

#endif
