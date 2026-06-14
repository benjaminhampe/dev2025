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

    HKL m_KEYBOARD_INPUT_HKL = nullptr;
    uint32_t m_KEYBOARD_INPUT_CODEPAGE = 1252; // default: 1252 (Portuguese?)

    bool m_bFullscreen = false;
    bool m_bFocused = false;
    bool m_bWindowDestroyed = false;

    void toggleFullscreen() override;
    bool createWindow(void* parentHandle, int x, int y, int w, int h) override;
    void destroy() override;
    void resize(int x, int y, int w, int h) override;
    void makeCurrent() override;
    void doneCurrent() override;
    void swapBuffers() override;

    // void render() override;


};

LRESULT CALLBACK
Backend_WGL_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
