// main.cpp
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/wgl.h>
#include <shellapi.h>

#pragma comment(lib, "opengl32.lib")

class MainWindow {
public:
    MainWindow()
        : m_hwnd(nullptr),
          m_hdc(nullptr),
          m_hglrc(nullptr),
          m_width(800),
          m_height(600),
          m_running(true),
          m_frameMs(16),
          m_resizing(false),
          m_memDC(nullptr),
          m_memBmp(nullptr)
    {}

    bool create(HINSTANCE hInst) {
        WNDCLASSEXW wc = {};
        wc.cbSize        = sizeof(wc);
        wc.style         = CS_OWNDC;
        wc.lpfnWndProc   = &MainWindow::WndProcStatic;
        wc.hInstance     = hInst;
        wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"EngineStyleGLWindow";

        if (!RegisterClassExW(&wc))
            return false;

        DWORD style = WS_POPUP; // borderless
        DWORD ex    = WS_EX_APPWINDOW;

        m_hwnd = CreateWindowExW(
            ex,
            wc.lpszClassName,
            L"Engine-style GL Window",
            style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            m_width, m_height,
            nullptr, nullptr,
            hInst,
            this
        );
        if (!m_hwnd)
            return false;

        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);

        DragAcceptFiles(m_hwnd, TRUE);

        initGL();
        createBackbuffer(m_width, m_height);
        return true;
    }

    void run() {
        while (m_running) {
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    m_running = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (!m_resizing) {
                renderGL();
                captureToBackbuffer();
            } else {
                blitBackbuffer();
            }

            Sleep(m_frameMs);
        }
    }

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

        if (self)
            return self->WndProc(hwnd, msg, wParam, lParam);

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_NCCALCSIZE:
            if (wParam) return 0; // borderless
            break;

        case WM_NCHITTEST: {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            RECT rc;
            GetWindowRect(hwnd, &rc);

            const int border = 6;

            bool left   = pt.x >= rc.left   && pt.x < rc.left + border;
            bool right  = pt.x <  rc.right  && pt.x >= rc.right - border;
            bool top    = pt.y >= rc.top    && pt.y < rc.top + border;
            bool bottom = pt.y <  rc.bottom && pt.y >= rc.bottom - border;

            if (left && top)    return HTTOPLEFT;
            if (right && top)   return HTTOPRIGHT;
            if (left && bottom) return HTBOTTOMLEFT;
            if (right && bottom)return HTBOTTOMRIGHT;
            if (left)           return HTLEFT;
            if (right)          return HTRIGHT;
            if (top)            return HTTOP;
            if (bottom)         return HTBOTTOM;

            return HTCAPTION; // drag anywhere else
        }

        case WM_ENTERSIZEMOVE:
            m_resizing = true;
            return 0;

        case WM_EXITSIZEMOVE:
            m_resizing = false;
            return 0;

        case WM_SIZE: {
            m_width  = LOWORD(lParam);
            m_height = HIWORD(lParam);
            resizeBackbuffer(m_width, m_height);
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
            if (wParam == VK_ESCAPE) {
                m_running = false;
                DestroyWindow(hwnd);
                return 0;
            }
            onKeyDown(wParam);
            return 0;
        }

        case WM_CLOSE:
            m_running = false;
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            destroyGL();
            destroyBackbuffer();
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    void initGL() {
        m_hdc = GetDC(m_hwnd);

        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize      = sizeof(pfd);
        pfd.nVersion   = 1;
        pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
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

    void renderGL() {
        if (!m_hdc || !m_hglrc)
            return;

        wglMakeCurrent(m_hdc, m_hglrc);

        glViewport(0, 0, m_width, m_height);
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
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

    void createBackbuffer(int w, int h) {
        HDC hdc = GetDC(m_hwnd);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = w;
        bmi.bmiHeader.biHeight      = -h; // top-down
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        m_memBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        m_memDC  = CreateCompatibleDC(hdc);
        SelectObject(m_memDC, m_memBmp);

        ReleaseDC(m_hwnd, hdc);
    }

    void destroyBackbuffer() {
        if (m_memDC) {
            DeleteDC(m_memDC);
            m_memDC = nullptr;
        }
        if (m_memBmp) {
            DeleteObject(m_memBmp);
            m_memBmp = nullptr;
        }
    }

    void resizeBackbuffer(int w, int h) {
        destroyBackbuffer();
        createBackbuffer(w, h);
    }

    void captureToBackbuffer() {
        if (!m_memDC || !m_memBmp || !m_hdc)
            return;

        BitBlt(m_memDC, 0, 0, m_width, m_height, m_hdc, 0, 0, SRCCOPY);
    }

    void blitBackbuffer() {
        if (!m_memDC || !m_memBmp || !m_hdc)
            return;

        RECT rc;
        GetClientRect(m_hwnd, &rc);
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;

        StretchBlt(m_hdc, 0, 0, w, h, m_memDC, 0, 0, m_width, m_height, SRCCOPY);
    }

    // --- event handlers you can extend ---
    void onMouseDown(int x, int y) {}
    void onMouseMove(int x, int y) {}
    void onMouseUp(int x, int y) {}

    void onDropFiles(HDROP hDrop) {
        wchar_t path[MAX_PATH];
        if (DragQueryFileW(hDrop, 0, path, MAX_PATH)) {
            // handle dropped file
        }
        DragFinish(hDrop);
    }

    void onKeyDown(WPARAM vk) {
        if (vk == VK_ADD || vk == VK_OEM_PLUS) {
            if (m_frameMs > 1) m_frameMs -= 1;
        } else if (vk == VK_SUBTRACT || vk == VK_OEM_MINUS) {
            m_frameMs += 1;
        }
    }

    HWND   m_hwnd;
    HDC    m_hdc;
    HGLRC  m_hglrc;
    int    m_width;
    int    m_height;
    bool   m_running;
    int    m_frameMs;
    bool   m_resizing;

    HDC    m_memDC;
    HBITMAP m_memBmp;
};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    MainWindow app;
    if (!app.create(hInst))
        return -1;

    app.run();
    return 0;
}
