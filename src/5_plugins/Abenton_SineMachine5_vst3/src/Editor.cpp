#include "Editor.h"

Editor::Editor()
    : m_refCount(1)
    , m_parentHWND(nullptr)
    , m_childHWND(nullptr)
    , m_hDC(nullptr)
    , m_hGL(nullptr)
    , m_plugFrame(nullptr)
    , m_controller(nullptr)
{
}

Editor::Editor(Steinberg::Vst::IEditController* controller)
    : m_refCount(1)
    , m_parentHWND(nullptr)
    , m_childHWND(nullptr)
    , m_hDC(nullptr)
    , m_hGL(nullptr)
    , m_plugFrame(nullptr)
    , m_controller(controller)
{
}

Editor::~Editor()
{
    if (m_hGL)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hGL);
        m_hGL = nullptr;
    }
    if (m_hDC && m_childHWND)
    {
        ReleaseDC(m_childHWND, m_hDC);
        m_hDC = nullptr;
    }
    if (m_childHWND)
    {
        DestroyWindow(m_childHWND);
        m_childHWND = nullptr;
    }
    m_parentHWND = nullptr;
}

Steinberg::tresult PLUGIN_API Editor::isPlatformTypeSupported(Steinberg::FIDString type)
{
    if (strcmp(type, Steinberg::kPlatformTypeHWND) == 0)
        return Steinberg::kResultTrue;
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Editor::attached(void* parentWindow,
                                               Steinberg::FIDString type)
{
    m_parentHWND = reinterpret_cast<HWND>(parentWindow);

    registerWindowClass();

    m_childHWND = CreateWindowExA(
        0,
        "SineVST3_EditorWindowClass",
        "",
        WS_CHILD | WS_VISIBLE,
        0, 0, 400, 300,
        m_parentHWND,
        nullptr,
        GetModuleHandleA(nullptr),
        this);

    if (!m_childHWND)
        return Steinberg::kResultFalse;

    m_hDC = GetDC(m_childHWND);
    if (!m_hDC)
        return Steinberg::kResultFalse;

    if (!setupPixelFormat(m_hDC))
        return Steinberg::kResultFalse;

    m_hGL = wglCreateContext(m_hDC);
    if (!m_hGL)
        return Steinberg::kResultFalse;

    wglMakeCurrent(m_hDC, m_hGL);

    glViewport(0, 0, 400, 300);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(m_hDC);

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::removed()
{
    if (m_hGL)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hGL);
        m_hGL = nullptr;
    }
    if (m_hDC && m_childHWND)
    {
        ReleaseDC(m_childHWND, m_hDC);
        m_hDC = nullptr;
    }
    if (m_childHWND)
    {
        DestroyWindow(m_childHWND);
        m_childHWND = nullptr;
    }
    m_parentHWND = nullptr;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onSize(Steinberg::ViewRect* newSize)
{
    if (!m_childHWND || !m_hDC || !m_hGL || !newSize)
        return Steinberg::kResultFalse;

    int w = newSize->right - newSize->left;
    int h = newSize->bottom - newSize->top;

    SetWindowPos(m_childHWND, nullptr, 0, 0, w, h,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    wglMakeCurrent(m_hDC, m_hGL);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(m_hDC);

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::getSize(Steinberg::ViewRect* size)
{
    if (!size)
        return Steinberg::kInvalidArgument;

    size->left   = 0;
    size->top    = 0;
    size->right  = 400;
    size->bottom = 300;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onFocus(Steinberg::TBool state)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::setFrame(Steinberg::IPlugFrame* frame)
{
    m_plugFrame = frame;
    return Steinberg::kResultOk;
}

// Required pure virtuals

Steinberg::tresult PLUGIN_API Editor::onWheel(float distance)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onKeyDown(Steinberg::char16 key,
                                                Steinberg::int16 keyCode,
                                                Steinberg::int16 modifiers)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::onKeyUp(Steinberg::char16 key,
                                              Steinberg::int16 keyCode,
                                              Steinberg::int16 modifiers)
{
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Editor::canResize()
{
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API Editor::checkSizeConstraint(Steinberg::ViewRect* rect)
{
    return Steinberg::kResultOk;
}

// FUnknown

Steinberg::tresult PLUGIN_API Editor::queryInterface(const Steinberg::TUID iid,
                                                     void** obj)
{
    if (Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::IPlugView::iid))
    {
        *obj = static_cast<Steinberg::IPlugView*>(this);
        addRef();
        return Steinberg::kResultOk;
    }

    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 PLUGIN_API Editor::addRef()
{
    return ++m_refCount;
}

Steinberg::uint32 PLUGIN_API Editor::release()
{
    Steinberg::uint32 r = --m_refCount;
    if (r == 0)
    {
        delete this;
        return 0;
    }
    return r;
}

// Win32 helpers

LRESULT CALLBACK Editor::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Editor* self = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCTA* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
        self = reinterpret_cast<Editor*>(cs->lpCreateParams);
        SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    }
    else
    {
        self = reinterpret_cast<Editor*>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));
    }

    if (!self)
        return DefWindowProcA(hWnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        if (self->m_hDC && self->m_hGL)
        {
            wglMakeCurrent(self->m_hDC, self->m_hGL);
            RECT rc;
            GetClientRect(hWnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;

            glViewport(0, 0, w, h);
            glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(-0.5f, -0.5f);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(0.5f, -0.5f);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2f(0.0f, 0.5f);
            glEnd();

            SwapBuffers(self->m_hDC);
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_SIZE:
    {
        if (self->m_hDC && self->m_hGL)
        {
            wglMakeCurrent(self->m_hDC, self->m_hGL);
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            glViewport(0, 0, w, h);
            glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            SwapBuffers(self->m_hDC);
        }
        return 0;
    }

    default:
        break;
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

void Editor::registerWindowClass()
{
    static bool registered = false;
    if (registered)
        return;

    WNDCLASSA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = &Editor::WndProc;
    wc.hInstance     = GetModuleHandleA(nullptr);
    wc.lpszClassName = "SineVST3_EditorWindowClass";

    RegisterClassA(&wc);
    registered = true;
}

bool Editor::setupPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(hdc, &pfd);
    if (pf == 0)
        return false;

    if (!SetPixelFormat(hdc, pf, &pfd))
        return false;

    return true;
}
