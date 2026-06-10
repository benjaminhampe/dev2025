#include "Backend_WGL.h"

#ifdef _WIN32

#include <de_opengl.h>

bool Backend_WGL::createWindow(void* parentHandle, int x, int y, int w, int h)
{
    HWND parent = (HWND) parentHandle;

    static const wchar_t* cls = L"WGL_CHILD";
    static bool reg = false;

    if (!reg)
    {
        WNDCLASSW wc = {};
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = DefWindowProcW;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = cls;
        RegisterClassW(&wc);
        reg = true;
    }

    hwnd = CreateWindowExW(
        0, cls, L"", WS_CHILD | WS_VISIBLE,
        x, y, w, h,
        parent, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!hwnd) { DE_ERROR("No hwnd") return false; }

    hdc = GetDC(hwnd);
    if (!hdc) { DE_ERROR("No hdc") return false; }

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;

    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);

    hgl = wglCreateContext(hdc);
    if (!hgl) { DE_ERROR("No hgl") return false; }

    return true;
}

void Backend_WGL::destroy()
{
    if (hgl) { wglDeleteContext(hgl); hgl = nullptr; }
    if (hdc && hwnd) { ReleaseDC(hwnd, hdc); hdc = nullptr; }
    if (hwnd) { DestroyWindow(hwnd); hwnd = nullptr; }
}

void Backend_WGL::resize(int x, int y, int w, int h)
{
    if (!hwnd) { DE_ERROR("No hwnd") return; }
    MoveWindow(hwnd, x, y, w, h, TRUE);
}

void Backend_WGL::makeCurrent()
{
    if (!hdc) { DE_ERROR("No hdc") return; }
    if (!hgl) { DE_ERROR("No hgl") return; }
    wglMakeCurrent(hdc, hgl);
}

void Backend_WGL::doneCurrent()
{
    wglMakeCurrent(nullptr, nullptr);
}

void Backend_WGL::swapBuffers()
{
    if (!hdc) { DE_ERROR("No hdc") return; }
    SwapBuffers(hdc);
}

/*
void Backend_WGL::render()
{
    makeCurrent();

    RECT rc;
    GetClientRect(hwnd, &rc);
    glViewport(0, 0, rc.right, rc.bottom);

    glClearColor(0.1f, 0.1f, 0.15f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Dein Rendering

    swapBuffers();
    doneCurrent();
}
*/

#endif
