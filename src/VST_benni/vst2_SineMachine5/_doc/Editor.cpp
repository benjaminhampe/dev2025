#include "editor.h"
#include <GL/glew.h>
#include <GL/wglew.h>

bool Editor::open(void* parent) {
    HWND parentHwnd = (HWND)parent;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"MyPluginEditor";
    RegisterClass(&wc);

    hwnd = CreateWindowEx(0, wc.lpszClassName, L"My Plugin",
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        0, 0, 800, 600,
        parentHwnd, nullptr, wc.hInstance, this);

    hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = { 
        sizeof(PIXELFORMATDESCRIPTOR), 
        1, 
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 
        PFD_TYPE_RGBA, 
        32 };
    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);
    glrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, glrc);

    glewInit();
    initGL();

    return true;
}

void Editor::initGL() {
    glViewport(0, 0, 800, 600);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
}

void Editor::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(hdc);
}

void Editor::close() {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(glrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    hwnd = nullptr;
}

LRESULT CALLBACK Editor::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
    Editor* self = reinterpret_cast<Editor*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return 0;
    }

    if (!self) return DefWindowProc(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_PAINT:
            self->render();
            ValidateRect(hwnd, nullptr);
            return 0;
        case WM_SIZE:
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            glViewport(0, 0, w, h);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_F11) self->toggleFullscreen();
            return 0;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void Editor::toggleFullscreen() {
    fullscreen = !fullscreen;
    SetWindowLong(hwnd, GWL_STYLE, fullscreen ? WS_POPUP : WS_CHILD | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, fullscreen ? 1920 : 800, fullscreen ? 1080 : 600, SWP_FRAMECHANGED);
}
