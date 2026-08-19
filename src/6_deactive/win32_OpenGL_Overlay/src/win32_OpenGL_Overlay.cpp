// main.cpp
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/wgl.h>
#include <shellapi.h> // Drag&Drop

#pragma comment(lib, "opengl32.lib")

class MainWindow {
public:
    MainWindow()
        : m_hwnd(nullptr),
          m_hdc(nullptr),
          m_hglrc(nullptr),
          m_refreshMs(16), // ~60 FPS
          m_timerId(1),
          m_button(nullptr),
          m_label(nullptr)
    {}

    bool create(HINSTANCE hInst, int width, int height) {
        WNDCLASSW wc = {};
        wc.style         = CS_OWNDC;
        wc.lpfnWndProc   = &MainWindow::WndProcStatic;
        wc.hInstance     = hInst;
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"RawGLWinClass";

        if (!RegisterClassW(&wc))
            return false;

        m_hwnd = CreateWindowExW(
            0,
            wc.lpszClassName,
            L"Win32 + WGL + Controls",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            width, height,
            nullptr,
            nullptr,
            hInst,
            this // pass this pointer to WM_NCCREATE
        );

        if (!m_hwnd)
            return false;

        DragAcceptFiles(m_hwnd, TRUE);

        initGL();
        createControls();
        startTimer();

        return true;
    }

    void destroy() {
        stopTimer();
        destroyGL();
        if (m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
    }

    void onResize(int w, int h) {
        m_width  = w;
        m_height = h;
    }

    void onMouseDown(int x, int y) {
        // your logic here
    }

    void onMouseMove(int x, int y) {
        // your logic here
    }

    void onMouseUp(int x, int y) {
        // your logic here
    }

    void onDropFiles(HDROP hDrop) {
        wchar_t path[MAX_PATH];
        if (DragQueryFileW(hDrop, 0, path, MAX_PATH)) {
            // your logic here (e.g. print path)
        }
        DragFinish(hDrop);
    }

    void onClose() {
        PostQuitMessage(0);
    }

    void onKeyDown(WPARAM vk) {
        if (vk == VK_ADD || vk == VK_OEM_PLUS) {
            if (m_refreshMs > 1) m_refreshMs -= 1;
            restartTimer();
        } else if (vk == VK_SUBTRACT || vk == VK_OEM_MINUS) {
            m_refreshMs += 1;
            restartTimer();
        }
    }

    void render() {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, m_width, m_height);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.f, 1.f, 0.f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.f, 0.f, 1.f); glVertex2f( 0.0f,  0.5f);
        glEnd();

        SwapBuffers(m_hdc);
        wglMakeCurrent(nullptr, nullptr);
    }

    HWND hwnd() const { return m_hwnd; }

private:
    static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        MainWindow* self = nullptr;

        if (msg == WM_NCCREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            self = reinterpret_cast<MainWindow*>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);
        } else {
            self = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (self) {
            return self->WndProc(hwnd, msg, wParam, lParam);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_SIZE: {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            onResize(w, h);
            layoutControls(w, h);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            onMouseDown(x, y);
            return 0;
        }
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            onMouseMove(x, y);
            return 0;
        }
        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            onMouseUp(x, y);
            return 0;
        }
        case WM_DROPFILES: {
            onDropFiles((HDROP)wParam);
            return 0;
        }
        case WM_KEYDOWN: {
            onKeyDown(wParam);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == m_timerId) {
                render();
            }
            return 0;
        }
        case WM_CLOSE: {
            onClose();
            return 0;
        }
        case WM_DESTROY: {
            destroy();
            return 0;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }

    void initGL() {
        m_hdc = GetDC(m_hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL; //  | PFD_DOUBLEBUFFER
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pf = ChoosePixelFormat(m_hdc, &pfd);
        SetPixelFormat(m_hdc, pf, &pfd);

        m_hglrc = wglCreateContext(m_hdc);
        wglMakeCurrent(m_hdc, m_hglrc);

        RECT rc;
        GetClientRect(m_hwnd, &rc);
        m_width  = rc.right - rc.left;
        m_height = rc.bottom - rc.top;

        wglMakeCurrent(nullptr, nullptr);
    }

    void destroyGL() {
        if (m_hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hglrc);
            m_hglrc = nullptr;
        }
        if (m_hdc) {
            ReleaseDC(m_hwnd, m_hdc);
            m_hdc = nullptr;
        }
    }

    void startTimer() {
        SetTimer(m_hwnd, m_timerId, m_refreshMs, nullptr);
    }

    void stopTimer() {
        KillTimer(m_hwnd, m_timerId);
    }

    void restartTimer() {
        stopTimer();
        startTimer();
    }

    void createControls() {
        m_button = CreateWindowExW(
            0, L"BUTTON", L"Click",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, 10, 80, 30,
            m_hwnd,
            (HMENU)1001,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            nullptr
        );

        m_label = CreateWindowExW(
            0, L"STATIC", L"Refresh: +/-",
            WS_CHILD | WS_VISIBLE,
            100, 15, 120, 20,
            m_hwnd,
            (HMENU)1002,
            (HINSTANCE)GetWindowLongPtr(m_hwnd, GWLP_HINSTANCE),
            nullptr
        );
    }

    void layoutControls(int w, int h) {
        if (m_button) {
            SetWindowPos(m_button, nullptr, 10, 10, 80, 30, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (m_label) {
            SetWindowPos(m_label, nullptr, 100, 15, 120, 20, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    HWND   m_hwnd;
    HDC    m_hdc;
    HGLRC  m_hglrc;
    int    m_width;
    int    m_height;

    UINT   m_refreshMs;
    UINT   m_timerId;

    HWND   m_button;
    HWND   m_label;
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    MainWindow app;
    if (!app.create(hInst, 800, 600))
        return -1;

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
