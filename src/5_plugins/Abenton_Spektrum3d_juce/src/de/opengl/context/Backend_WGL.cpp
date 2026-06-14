#include "Backend_WGL.h"
#include <DarkGPU.h>
#include "MyComponent.h"
#ifdef _WIN32

#include <de_opengl.h>
#include <windowsx.h>

void Backend_WGL::toggleFullscreen()
{
    m_bFullscreen = !m_bFullscreen;
    SetWindowLong(hwnd, GWL_STYLE,
                    m_bFullscreen ? WS_POPUP : WS_CHILD | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0,
                    m_bFullscreen ? 1920 : 800,
                    m_bFullscreen ? 1080 : 600,
                    SWP_FRAMECHANGED);
}


bool Backend_WGL::createWindow(void* parentHandle, int x, int y, int w, int h)
{
    HWND parentHwnd = (HWND) parentHandle;

    static const wchar_t* lpszClassName = L"Spektrum3D_WinWGL_Class";
    static bool reg = false;

    if (!reg)
    {
        WNDCLASSW wc = {};
        //wc.style = CS_OWNDC;
        wc.lpfnWndProc = Backend_WGL_WndProc; // DefWindowProcW;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.lpszClassName = lpszClassName;
        RegisterClassW(&wc);
        reg = true;
    }

    hwnd = CreateWindowExW(
        WS_EX_CONTROLPARENT,
        lpszClassName,
        L"Spektrum3D_WinWGL",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP,
        x, y, w, h,
        parentHwnd,
        nullptr,
        GetModuleHandleW(nullptr),
        this);

    if (!hwnd) { DE_ERROR("No hwnd") return false; }

    SetFocus(hwnd);

    hdc = GetDC(hwnd);
    if (!hdc) { DE_ERROR("No hdc") return false; }

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
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

    // Fix event passing to underlying window
    //SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);


    return true;
}

void Backend_WGL::destroy()
{
    if (hgl)
    {
        DE_OK("Destroy WGL")
        HGLRC current = wglGetCurrentContext();
        if (current == hgl)
            wglMakeCurrent(nullptr, nullptr); // nur deinen Kontext entbinden

        wglDeleteContext(hgl);
        hgl = nullptr;
    }
    if (hdc && hwnd)
    {
        DE_OK("Destroy DC")
        ReleaseDC(hwnd, hdc);
        hdc = nullptr;
    }
    if (hwnd)
    {
        DE_OK("Destroy Window")
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
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
    //wglMakeCurrent(nullptr, nullptr);
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


LRESULT CALLBACK
Backend_WGL_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto self = reinterpret_cast<Backend_WGL*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);

        DE_OK("WM_CREATE")
        SetTimer(hwnd, 123, 1000 / 60, NULL); // 1/10th-second timer
        return 0;
    }

    if (!self) return DefWindowProc(hwnd, msg, wParam, lParam);

    auto createMouseDblClickEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        const int mx = GET_X_LPARAM(lParam);
        const int my = GET_Y_LPARAM(lParam);
        const bool bCtrl = (wParam & MK_CONTROL) != 0;
        const bool bShift = (wParam & MK_SHIFT) != 0;
        //const bool bAlt = (wParam & MK_ALT) != 0;

        de::MouseDblClickEvent e;
        e.x = mx;
        e.y = my;

        e.flags = de::MouseFlag::DoubleClick;
        if (bCtrl) { e.flags |= de::MouseFlag::WithCtrl; }
        if (bShift) { e.flags |= de::MouseFlag::WithShift; }
        //if (bAlt) { e.flags |= de::MouseFlag::WithAlt; }

        switch (msg)
        {
        case WM_LBUTTONDBLCLK: e.buttons = de::MouseButton::Left; break;
        case WM_RBUTTONDBLCLK: e.buttons = de::MouseButton::Right; break;
        case WM_MBUTTONDBLCLK: e.buttons = de::MouseButton::Middle; break;
        default: DE_ERROR("Unsupported mouse button double click.") break;
        }

        return e;
    };

    auto createMousePressEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        de::MousePressEvent e;
        e.x = LOWORD(lParam);
        e.y = HIWORD(lParam);
        e.flags = de::MouseFlag::Pressed;
        //e.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
        //e.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);

        switch (msg)
        {
        case WM_LBUTTONDOWN: e.buttons = de::MouseButton::Left; break;
        case WM_RBUTTONDOWN: e.buttons = de::MouseButton::Right; break;
        case WM_MBUTTONDOWN: e.buttons = de::MouseButton::Middle; break;
        default: DE_ERROR("Unsupported mouse press event.") break;
        }
        return e;
    };

    auto createMouseReleaseEvent = [](UINT msg, WPARAM wParam, LPARAM lParam)
    {
        de::MouseReleaseEvent e;
        e.x = LOWORD(lParam);
        e.y = HIWORD(lParam);
        e.flags = de::MouseFlag::Released;
        //e.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
        //e.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);

        switch (msg)
        {
        case WM_LBUTTONUP: e.buttons = de::MouseButton::Left; break;
        case WM_RBUTTONUP: e.buttons = de::MouseButton::Right; break;
        case WM_MBUTTONUP: e.buttons = de::MouseButton::Middle; break;
        default: DE_ERROR("Unsupported mouse release event.") break;
        }
        return e;
    };

    auto createKeyPressEvent = [](Backend_WGL* self, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        BYTE allKeys[ 256 ];
        GetKeyboardState( allKeys );
        bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
        bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

        // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
        // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
        UINT32 unicode = 0;
        wchar_t singleChar = 0;
        WORD keyChars[ 2 ];
        UINT scanCode = HIWORD( lParam );
        int conversionResult = ::ToAsciiEx( UINT(wParam),
                                           scanCode,
                                           allKeys,
                                           keyChars,
                                           0,
                                           self->m_KEYBOARD_INPUT_HKL );
        if (conversionResult == 1)
        {
            WORD unicodeChar;
            ::MultiByteToWideChar( self->m_KEYBOARD_INPUT_CODEPAGE,
                                  MB_PRECOMPOSED, // default
                                  reinterpret_cast<LPCSTR>(keyChars),
                                  sizeof( keyChars ),
                                  reinterpret_cast<WCHAR*>(&unicodeChar),
                                  1 );
            singleChar = unicodeChar;
            unicode = unicodeChar;
        }
        else
        {
            // DE_ERROR("Conversion Error in keyPressEvent")
        }

        de::KeyPressEvent e;
        e.key = de::translateWinKey( UINT(wParam) );
        e.unicode = unicode;
        e.scancode = UINT(wParam);
        e.modifiers = 0;
        if ( isShift ) e.modifiers |= de::KeyModifier::Shift;
        if ( isCtrl ) e.modifiers |= de::KeyModifier::Ctrl;
        return e;
    };

    auto createKeyReleaseEvent = [](Backend_WGL* self, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        BYTE allKeys[ 256 ];
        GetKeyboardState( allKeys );
        bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
        bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

        // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
        // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
        UINT32 unicode = 0;
        wchar_t singleChar = 0;
        WORD keyChars[ 2 ];
        UINT scanCode = HIWORD( lParam );
        int conversionResult = ::ToAsciiEx( UINT(wParam),
                                           scanCode,
                                           allKeys,
                                           keyChars,
                                           0,
                                           self->m_KEYBOARD_INPUT_HKL );
        if (conversionResult == 1)
        {
            WORD unicodeChar;
            ::MultiByteToWideChar( self->m_KEYBOARD_INPUT_CODEPAGE,
                                  MB_PRECOMPOSED, // default
                                  reinterpret_cast<LPCSTR>(keyChars),
                                  sizeof( keyChars ),
                                  reinterpret_cast<WCHAR*>(&unicodeChar),
                                  1 );
            singleChar = unicodeChar;
            unicode = unicodeChar;
        }
        else
        {
            // DE_ERROR("Conversion Error in keyReleaseEvent")
        }

        de::KeyReleaseEvent e;
        e.key = de::translateWinKey( UINT(wParam) );
        e.unicode = unicode;
        e.scancode = UINT(wParam);
        e.modifiers = 0;
        if ( isShift ) e.modifiers |= de::KeyModifier::Shift;
        if ( isCtrl ) e.modifiers |= de::KeyModifier::Ctrl;
        return e;
    };

    switch (msg)
    {
        // case WM_CREATE:
        // {
        //     return 0;
        // }
        case WM_SETFOCUS:
        {
            DE_OK("WM_SETFOCUS")
            self->m_bFocused = true;
            break;
        }
        case WM_KILLFOCUS:
        {
            DE_OK("WM_KILLFOCUS")
            self->m_bFocused = false;
            break;
        }
        case WM_DESTROY:
        {
            DE_OK("WM_DESTROY")
            //KillTimer(hwnd, 123);
            //PostQuitMessage(0);


            if (self && !self->m_bWindowDestroyed)
            {
                // self->destroyed = true;
                self->hwnd = nullptr; // Prevent another call to DestroyWindow -> WM_DESTROY message recursive loop
                // self->notifyOwnerComponentWindowIsGone();
                if (self->getMyComponent())
                    self->getMyComponent()->shutdownBackend();
            }
            return 0;
        }
        case WM_TIMER:
        {
            if (wParam == 123)
            {
                InvalidateRect(hwnd, NULL, TRUE); // force redraw
            }
            return 0;
        }
        case WM_ERASEBKGND:
        {
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
/*
            if ( self )
            {
                wglMakeCurrent(ps.hdc, self->hgl);

                RECT r;
                GetClientRect(hwnd, &r);

                de::PaintEvent event;
                event.w = r.right - r.left;
                event.h = r.bottom - r.top;
                //self->paintEvent(event);

                SwapBuffers( ps.hdc );
            }
*/
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE:
        {
            //int w = LOWORD(lParam);
            //int h = HIWORD(lParam);
            int w = GET_X_LPARAM( lParam );
            int h = GET_Y_LPARAM( lParam );
            //DE_OK("WM_SIZE(",w,",",h,")");
            de::ResizeEvent event;
            event.w = w;
            event.h = h;
            //self->resizeEvent(event);
            return 0;
        }
        case WM_LBUTTONDBLCLK:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mdce = createMouseDblClickEvent(msg, wParam, lParam);
            DE_OK("WM_LBUTTONDBLCLK ",mdce.str())
            //self->mouseDblClickEvent( mdce );
            return 0;
        }
        case WM_RBUTTONDBLCLK:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mdce = createMouseDblClickEvent(msg, wParam, lParam);
            DE_OK("WM_RBUTTONDBLCLK ",mdce.str())
            //self->mouseDblClickEvent( mdce );
            return 0;
        }
        case WM_MBUTTONDBLCLK:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mdce = createMouseDblClickEvent(msg, wParam, lParam);
            DE_OK("WM_MBUTTONDBLCLK ",mdce.str())
            //self->mouseDblClickEvent( mdce );
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            // if (!self->_d->focused)
            // {
            //     SetFocus(hwnd);
            // }
            de::MouseMoveEvent event;
            event.x = int( LOWORD( lParam ) );
            event.y = int( HIWORD( lParam ) );
            //DE_OK("WM_MOUSEMOVE ",event.str())
            //self->mouseMoveEvent( event );
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            de::MouseWheelEvent event;
            event.x = 0.0f;
            event.y = float( int16_t( HIWORD( wParam ) ) ) / float( WHEEL_DELTA );
            DE_OK("WM_MOUSEWHEEL ",event.str())
            //self->mouseWheelEvent( event );
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mpe = createMousePressEvent(msg, wParam, lParam);
            DE_OK("WM_LBUTTONDOWN ",mpe.str())
            //self->mousePressEvent( mpe );
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mpe = createMousePressEvent(msg, wParam, lParam);
            DE_OK("WM_RBUTTONDOWN ",mpe.str())
            //self->mousePressEvent( mpe );
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mpe = createMousePressEvent(msg, wParam, lParam);
            DE_OK("WM_MBUTTONDOWN ",mpe.str())
            //self->mousePressEvent( mpe );
            return 0;
        }
        case WM_LBUTTONUP:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mre = createMouseReleaseEvent(msg, wParam, lParam);
            DE_OK("WM_LBUTTONUP ",mre.str())
            //self->mouseReleaseEvent( mre );
            return 0;
        }
        case WM_RBUTTONUP:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mre = createMouseReleaseEvent(msg, wParam, lParam);
            DE_OK("WM_RBUTTONUP ",mre.str())
            //self->mouseReleaseEvent( mre );
            return 0;
        }
        case WM_MBUTTONUP:
        {
            if (!self->m_bFocused)
            {
                SetFocus(hwnd);
            }
            auto mre = createMouseReleaseEvent(msg, wParam, lParam);
            DE_OK("WM_MBUTTONUP ",mre.str())
            //self->mouseReleaseEvent( mre );
            return 0;
        }

        //case WM_XBUTTONDOWN:
        //case WM_XBUTTONUP:

        // === KeyboardEvents: ===

        case WM_INPUTLANGCHANGE:
        {
            auto hkl = GetKeyboardLayout( 0 ); // get the new codepage used for keyboard input
            self->m_KEYBOARD_INPUT_HKL = hkl; // get the new codepage used for keyboard input
            self->m_KEYBOARD_INPUT_CODEPAGE = de::convertLocaleIdToCodepage( LOWORD( hkl ) );
            return 0;
        }
        case WM_KEYDOWN:
        {
            DE_OK("WM_KEYDOWN")
            //self->keyPressEvent( createKeyPressEvent(self, msg, wParam, lParam) );
            return 0;
        }
        case WM_KEYUP:
        {
            DE_OK("WM_KEYUP")
            //self->keyReleaseEvent( createKeyReleaseEvent(self, msg, wParam, lParam) );
            return 0;
        }
        case WM_SYSKEYDOWN:
        {
            DE_OK("WM_SYSKEYDOWN")
            //self->keyPressEvent( createKeyPressEvent(self, msg, wParam, lParam) );
            return 0;
        }
        case WM_SYSKEYUP:
        {
            DE_OK("WM_SYSKEYUP")
            //self->keyReleaseEvent( createKeyReleaseEvent(self, msg, wParam, lParam) );
            return 0;
        }
/*
        case WM_SYSCOMMAND:
        {
            // if ( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
            //      ( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
            //      ( wParam & 0xFFF0 ) == SC_KEYMENU )
            // {
            //    return 0; // prevent screensaver or monitor powersave mode from starting
            // }
            break;
        }
*/
        default:
        {
            break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif
