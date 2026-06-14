#include "SineEditor.h"
#include "SinePlugin.h"

#include <vector> // for iAttributes in CreateContext
#include <de_opengl.h>


#include <dwmapi.h>
#include <tchar.h>
#include <mmsystem.h> // For JOYCAPS

// ===================================================================
// INCLUDE: WGL
// ===================================================================
#ifndef WGL_WGLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES
#endif
#include <GL/glew.h>
#include <GL/wglext.h>
#include "../res/resource.h"
#include "fonts/fonts_ShareTechMonoRegular_ttf.h"

// 📊
void Preview::init( const SynthCfg& cfg, int n )
{
    const auto & partials = cfg.m_partials.m_partials;

    DE_DEBUG("n = ",n)
    DE_DEBUG("partials.size() = ",partials.size())

    m_curves.resize( partials.size() );

    m_accum.color = nvgRGBA(255,255,255,255);
    m_accum.normalized.resize( n );
    m_accum.points.resize( n );

    // Fill colors and amplitudeSums:
    float Asum = 0.0f;
    for (size_t c = 0; c < partials.size(); c++)
    {
        float A = partials.at(c).A();
        Asum += A;
        m_curves.at(c).partial = partials.at(c).iPartial;
        m_curves.at(c).color = partials.at(c).color;
        m_curves.at(c).amplitude = A;
        m_curves.at(c).amplitudeSum = Asum;
        m_curves.at(c).original.resize(n);
        m_curves.at(c).scaled.resize(n);
        m_curves.at(c).accum.resize(n);
        m_curves.at(c).normalized.resize(n);
        m_curves.at(c).points.resize(n);
    }

    // Init original samples: Build unscaled sin() lookuptable

    for (Curve & curve : m_curves)
    {
        const float phaseInc = float(de::TWO_PI * curve.partial) / float(n-1); // 1Hz base for preview.
        float phase = 0.0;
        for (int i = 0; i < n; i++)
        {
            curve.original.at(i) = sinf(phase);
            phase += phaseInc;
        }
    }
}

void Preview::update( const SynthCfg & cfg )
{
    const auto & partials = cfg.m_partials.m_partials;
    const size_t nPartials = partials.size();
    const size_t nSamples = m_curves.at(0).original.size();

    // Fill colors and amplitudeSums:
    float Asum = 0.0f;
    for (size_t c = 0; c < nPartials; c++)
    {
        float A = partials.at(c).A();
        Asum += A;
        m_curves.at(c).amplitude = A;
        m_curves.at(c).amplitudeSum = Asum;
    }

    // Scale:
    for (Curve & curve : m_curves)
    {
        const float A = curve.amplitude;

        for (size_t i = 0; i < nSamples; i++) // For all samples
        {
            curve.scaled.at(i) = curve.original.at(i) * A;
        }
    }

    // Accumulate:
    std::copy(m_curves.at(0).scaled.begin(),   // src
              m_curves.at(0).scaled.end(),     // src
              m_curves.at(0).accum.begin());   // dst

    // Accumulate:
    for (size_t c = 1; c < nPartials; c++)
    {
        for (size_t i = 0; i < nSamples; i++) // For all samples
        {
            m_curves.at(c).accum.at(i) = m_curves.at(c).scaled.at(i)
                                       + m_curves.at(c-1).accum.at(i);
        }
    }

    // Normalize:
    std::copy(m_curves.at(0).original.begin(),   // src
              m_curves.at(0).original.end(),     // src
              m_curves.at(0).normalized.begin());// dst

    // Normalize:
    for (size_t c = 1; c < nPartials; c++)
    {
        Curve & curve = m_curves.at(c);

        const float Asum = curve.amplitudeSum;
        if (Asum > 1.e-10f)
        {
            const float invA = 1.0f / Asum;
            for (size_t i = 0; i < nSamples; i++) // For all samples
            {
                curve.normalized.at(i) = curve.accum.at(i) * invA;
            }
        }
        else
        {
            std::memset(curve.normalized.data(), 0,
                        curve.normalized.size() * sizeof(float));
            //std::fill(curve.normalized.begin(), curve.normalized.end(), 0.0f);
        }
    }

    // Copy to white curve m_accum
    std::copy(m_curves.back().normalized.begin(),   // src
              m_curves.back().normalized.end(),     // src
              m_accum.normalized.begin());          // dst
}

void Preview::updatePoints( de::Recti pos, int n )
{
    const float ox = pos.x;
    const float oy = pos.y + pos.h/2; // Zero center line is middle of rect height.
    const float sx = float(pos.w) / float(n-1);
    const float sy = -float(pos.h) / 2.0f;

    for (int c = 0; c < m_curves.size(); c++)
    {
        Curve & curve = m_curves.at(c);

        for (int i = 0; i < curve.normalized.size(); i++)
        {
            const float y = curve.normalized.at(i);
            const float sample_x = sx * i + ox;
            const float sample_y = sy * y + oy;
            curve.points.at(i) = glm::vec2(sample_x, sample_y);
        }
    }

    Curve & curve = m_accum;

    for (int i = 0; i < curve.normalized.size(); i++)
    {
        const float y = curve.normalized.at(i);
        const float sample_x = sx * i + ox;
        const float sample_y = sy * y + oy;
        curve.points.at(i) = glm::vec2(sample_x, sample_y);
    }
}


void Preview::drawCurve(NVGcontext* vg, const Curve & curve, float strokeWidth)
{
    // Begin drawing
    nvgBeginPath(vg);
    nvgMoveTo(vg, curve.points.at(0).x, curve.points.at(0).y);
    for (size_t i = 1; i < curve.points.size(); i++)
    {
        nvgLineTo(vg, curve.points.at(i).x, curve.points.at(i).y);
    }

    // Set line style
    nvgStrokeColor(vg, curve.color);
    nvgStrokeWidth(vg, strokeWidth); // Thick line to emphasize joins
    nvgLineJoin(vg, NVG_ROUND); // Options: NVG_MITER, NVG_ROUND, NVG_BEVEL
    nvgStroke(vg);
}

void Preview::draw(NVGcontext* vg, de::Recti pos, int n )
{
    updatePoints( pos, n );

    for (int c = 0; c < m_curves.size(); c++)
    {
        drawCurve(vg, m_curves.at(c), 5.0f);
    }

    drawCurve(vg, m_accum, 3.0f);
}

// ------------------ Editor Implementation ------------------

class EditorImpl
{
public:
    HWND hwnd = nullptr;
    HDC hdc = nullptr;
    HGLRC glrc = nullptr;
    bool fullscreen = false;
    bool focused = false;
    int32_t m_screenWidth = 800;
    int32_t m_screenHeight = 600;

    HKL m_KEYBOARD_INPUT_HKL = nullptr;
    uint32_t m_KEYBOARD_INPUT_CODEPAGE = 1252; // default: 1252 (Portuguese?)
};

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Editor* self = reinterpret_cast<Editor*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
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
        const bool bAlt = (wParam & MK_ALT) != 0;

        de::MouseDblClickEvent e;
        e.x = mx;
        e.y = my;

        e.flags = de::MouseFlag::DoubleClick;
        if (bCtrl) { e.flags |= de::MouseFlag::WithCtrl; }
        if (bShift) { e.flags |= de::MouseFlag::WithShift; }
        if (bAlt) { e.flags |= de::MouseFlag::WithAlt; }

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

    auto createKeyPressEvent = [](Editor* self, UINT msg, WPARAM wParam, LPARAM lParam)
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
                                           self->_d->m_KEYBOARD_INPUT_HKL );
        if (conversionResult == 1)
        {
            WORD unicodeChar;
            ::MultiByteToWideChar( self->_d->m_KEYBOARD_INPUT_CODEPAGE,
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

    auto createKeyReleaseEvent = [](Editor* self, UINT msg, WPARAM wParam, LPARAM lParam)
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
                                           self->_d->m_KEYBOARD_INPUT_HKL );
        if (conversionResult == 1)
        {
            WORD unicodeChar;
            ::MultiByteToWideChar( self->_d->m_KEYBOARD_INPUT_CODEPAGE,
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
            self->_d->focused = true;
            break;
        }
        case WM_KILLFOCUS:
        {
            DE_OK("WM_KILLFOCUS")
            self->_d->focused = false;
            break;
        }
        case WM_DESTROY:
        {
            DE_OK("WM_DESTROY")
            KillTimer(hwnd, 123);
            //PostQuitMessage(0);
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

            if ( self )
            {
                wglMakeCurrent(ps.hdc, self->_d->glrc);

                RECT r;
                GetClientRect(hwnd, &r);

                de::PaintEvent event;
                event.w = r.right - r.left;
                event.h = r.bottom - r.top;
                self->paintEvent(event);

                SwapBuffers( ps.hdc );
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_SIZE:
        {
            //int w = LOWORD(lParam);
            //int h = HIWORD(lParam);
            int w = GET_X_LPARAM( lParam );
            int h = GET_Y_LPARAM( lParam );
            DE_OK("WM_SIZE(",w,",",h,")");
            de::ResizeEvent event;
            event.w = w;
            event.h = h;
            self->resizeEvent(event);
            return 0;
        }
        case WM_LBUTTONDBLCLK:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mouseDblClickEvent( createMouseDblClickEvent(msg, wParam, lParam) );
            return 0;
        }
        case WM_RBUTTONDBLCLK:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mouseDblClickEvent( createMouseDblClickEvent(msg, wParam, lParam) );
            return 0;
        }
        case WM_MBUTTONDBLCLK:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mouseDblClickEvent( createMouseDblClickEvent(msg, wParam, lParam) );
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
            self->mouseMoveEvent( event );
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            de::MouseWheelEvent event;
            event.x = 0.0f;
            event.y = float( int16_t( HIWORD( wParam ) ) ) / float( WHEEL_DELTA );
            self->mouseWheelEvent( event );
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mousePressEvent( createMousePressEvent(msg, wParam, lParam) );
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mousePressEvent( createMousePressEvent(msg, wParam, lParam) );
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mousePressEvent( createMousePressEvent(msg, wParam, lParam) );
            return 0;
        }
        case WM_LBUTTONUP:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mouseReleaseEvent( createMouseReleaseEvent(msg, wParam, lParam) );
            return 0;
        }
        case WM_RBUTTONUP:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mouseReleaseEvent( createMouseReleaseEvent(msg, wParam, lParam) );
            return 0;
        }
        case WM_MBUTTONUP:
        {
            if (!self->_d->focused)
            {
                SetFocus(hwnd);
            }
            self->mouseReleaseEvent( createMouseReleaseEvent(msg, wParam, lParam) );
            return 0;
        }

        //case WM_XBUTTONDOWN:
        //case WM_XBUTTONUP:

        // === KeyboardEvents: ===

        case WM_INPUTLANGCHANGE:
        {
            auto hkl = GetKeyboardLayout( 0 ); // get the new codepage used for keyboard input
            self->_d->m_KEYBOARD_INPUT_HKL = hkl; // get the new codepage used for keyboard input
            self->_d->m_KEYBOARD_INPUT_CODEPAGE = de::convertLocaleIdToCodepage( LOWORD( hkl ) );
            return 0;
        }
        case WM_KEYDOWN:
        {
            //DE_OK("WM_KEYDOWN")
            self->keyPressEvent( createKeyPressEvent(self, msg, wParam, lParam) );
            return 0;
        }
        case WM_KEYUP:
        {
            //DE_OK("WM_KEYUP")
            self->keyReleaseEvent( createKeyReleaseEvent(self, msg, wParam, lParam) );
            return 0;
        }
        case WM_SYSKEYDOWN:
        {
            //DE_OK("WM_SYSKEYDOWN")
            self->keyPressEvent( createKeyPressEvent(self, msg, wParam, lParam) );
            return 0;
        }
        case WM_SYSKEYUP:
        {
            //DE_OK("WM_SYSKEYUP")
            self->keyReleaseEvent( createKeyReleaseEvent(self, msg, wParam, lParam) );
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

void Editor::toggleFullscreen()
{
    _d->fullscreen = !_d->fullscreen;
    SetWindowLong(_d->hwnd, GWL_STYLE, _d->fullscreen ? WS_POPUP : WS_CHILD | WS_VISIBLE);
    SetWindowPos(_d->hwnd, HWND_TOP, 0, 0, _d->fullscreen ? 1920 : 800, _d->fullscreen ? 1080 : 600, SWP_FRAMECHANGED);
}

Editor::Editor(Plugin* plugin)
    : m_plugin(plugin)
    , _d(new EditorImpl)
    , m_vg(nullptr)
    // , m_updateTimerId(0)
    , m_mouseX(0)
    , m_mouseY(0)
    , m_paintEventEnabled(false)
    , m_doPartialDawing(false)
{
    m_erect.left = 0;
    m_erect.top = 0;
    m_erect.right = 800;
    m_erect.bottom = 600;
}

Editor::~Editor()
{
    delete _d;
    _d = nullptr;
}

bool
Editor::create(void* parent)
{
    int desktopW = GetSystemMetrics( SM_CXSCREEN );
    int desktopH = GetSystemMetrics( SM_CYSCREEN );

    _d->m_screenWidth = desktopW / 2 - 100;
    _d->m_screenHeight = desktopH - 300;

    int w = _d->m_screenWidth;
    int h = _d->m_screenHeight;

    m_erect.left = 0;
    m_erect.top = 0;
    m_erect.right = w;
    m_erect.bottom = h;

    HWND parentHwnd = (HWND)parent;

    static const wchar_t* lpszClassName = L"SineMachine5_EditorClass";
    static bool reg = false;
    if (!reg)
    {
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = lpszClassName;
        RegisterClassW(&wc);
        reg = true;
    }

    _d->hwnd = CreateWindowExW(
        WS_EX_CONTROLPARENT,
        lpszClassName,
        L"SineMachine5",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP, //  | WS_CLIPCHILDREN
        0, 0, w, h,
        parentHwnd,
        nullptr,
        GetModuleHandleW(nullptr),
        this);

    SetFocus( _d->hwnd );

    _d->hdc = GetDC(_d->hwnd);
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32 };

    int pf = ChoosePixelFormat(_d->hdc, &pfd);
    SetPixelFormat(_d->hdc, pf, &pfd);

    _d->glrc = wglCreateContext(_d->hdc);
    wglMakeCurrent(_d->hdc, _d->glrc);

    glewExperimental = GL_TRUE;
    glewInit();

    glViewport(0, 0, w, h);
    glClearColor(0.11f, 0.03f, 0.12f, 1.0f);

    m_vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    m_fontShareTechMonoRegular = nvgCreateFontMem(m_vg, "ShareTechMonoRegular",
                                                  const_cast<unsigned char*>(fonts_ShareTechMonoRegular_ttf),
                                                  fonts_ShareTechMonoRegular_ttf_len, 0);

    updateLayout(w,h);

    m_preview.init( m_plugin->getSynth().getConfig() );

    m_paintEventEnabled = true;

    // m_updateTimerId = m_window->startTimer(13);

    return true;
}

void Editor::destroy()
{
    DE_DEBUG("")

    m_paintEventEnabled = false;

    Sleep(100);

    if (m_vg)
    {
        wglMakeCurrent(_d->hdc, _d->glrc);
        nvgDeleteGL3(m_vg);
        m_vg = nullptr;
    }

    HGLRC current = wglGetCurrentContext();
    if (current == _d->glrc)
        wglMakeCurrent(nullptr, nullptr); // nur deinen Kontext entbinden

    wglDeleteContext(_d->glrc);
    _d->glrc = nullptr;

    ReleaseDC(_d->hwnd, _d->hdc);
    _d->hdc = nullptr;

    DestroyWindow(_d->hwnd);
    _d->hwnd = nullptr;
}

void Editor::timerEvent( const de::TimerEvent& event )
{
    // if (event.id == m_updateTimerId)
    // {
    //     if (m_window)
    //     {
    //         m_window->update();
    //     }
    // }
}

void Editor::resizeEvent( const de::ResizeEvent& event )
{
    int32_t w = event.w;
    int32_t h = event.h;
    DE_OK("w(",w,"), h(",h,")")

    _d->m_screenWidth = w;
    _d->m_screenHeight = h;

    // if (m_window)
    // {
    //     de::Recti clientRect = m_window->getClientRect();
    //     m_erect.left = clientRect.x;
    //     m_erect.top = clientRect.y;
    //     m_erect.right = clientRect.x + clientRect.w;
    //     m_erect.bottom = clientRect.y + clientRect.h;
    //     DE_OK("clientRect = {", clientRect.str(), "}")
    // }
    // else
    // {
    //     DE_OK("No clientRect")
    // }

    m_erect.left = 0;
    m_erect.top = 0;
    m_erect.right = w;
    m_erect.bottom = h;

    glViewport(0, 0, w, h);

    updateLayout(w,h);
}


void Editor::paintEvent( const de::PaintEvent& event )
{
    // DE_BENNI("")
    if (!m_paintEventEnabled)
    {
        DE_ERROR("No paint.")
        return;
    }

    if (!m_vg)
    {
        DE_ERROR("No nanovg.")
        return;
    }

    if (!m_plugin)
    {
        DE_ERROR("No plugin.")
        return;
    }

    int32_t w = _d->m_screenWidth;
    int32_t h = _d->m_screenHeight;

    // DE_BENNI("w = ",w, ", h = ",h)

    //int winW, winH;
    //glfwGetFramebufferSize(m_window, &winW, &winH);
    glViewport(0, 0, w, h);

    //glClearColor(0.5f, 0.1f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(m_vg, w, h, 1.0f);

    nvgFontFace(m_vg, "ShareTechMonoRegular");
    nvgFontSize(m_vg, 24.0f);  // in pixels
    nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    drawLineRect(m_vg, m_rHeader, nvgRGBA(255,0,255,255));
    drawLineRect(m_vg, m_rFooter, nvgRGBA(0,0,0,255));
    drawLineRect(m_vg, m_rPreview, nvgRGBA(255,128,0,255));
    drawLineRect(m_vg, m_rButtons, nvgRGBA(255,200,100,255));
    drawLineRect(m_vg, m_rPartial, nvgRGBA(255,100,100,255));
    drawLineRect(m_vg, m_rVolume, nvgRGBA(100,100,255,255));

    const auto & cfg = m_plugin->getSynth().getConfig();

    m_preview.update( cfg );
    m_preview.updatePoints( m_rPreview );
    m_preview.draw( m_vg, m_rPreview );

    // Draw overtone bars
    const auto & partials = cfg.m_partials.m_partials;
    for (int i = 0; i < partials.size(); ++i)
    {
        const auto & partial = partials.at(i);
        float amp = partial.fAmplitude;
        float barW = float(m_rPartial.w) / float(partials.size());
        float barH = amp * m_rPartial.h;
        float x = float(m_rPartial.x) + barW * i;
        float y = float(m_rPartial.y) + float(m_rPartial.h) - barH;

        nvgBeginPath(m_vg);
        nvgRect(m_vg, x + 2, y, barW - 4, barH);
        nvgFillColor(m_vg, partial.color);
        nvgFill(m_vg);

        // white text
        nvgFontSize(m_vg, 20.0f);  // in pixels
        nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(m_vg, nvgRGBA(0, 0, 0, 255));
        nvgText(m_vg, x + barW/2.0f, y + 16, partial.str().c_str(), nullptr);
    }

    // Draw mouse position as a small circle
    nvgBeginPath(m_vg);
    nvgCircle(m_vg, m_mouseX, m_mouseY, 5.0f);
    nvgFillColor(m_vg, nvgRGBA(255, 100, 100, 255));
    nvgFill(m_vg);

    nvgEndFrame(m_vg);
}

void Editor::updateLayout(int32_t w, int32_t h)
{
    int p = 20;
    int hHeader = 64; // PresetBar
    int hFooter = 64; // Active SynthNote Display
    int hBody = h - hHeader - hFooter;

    int h1 = hBody / 2; // Preview + Main Buttons
    int h2 = hBody - h1; // Partial editor + Volume ctrl
    int hButtons = 64;
    int hVolume = 64;
    int hPreview = h1 - hButtons -p-p-p;
    int hPartial = h2 - hVolume -p-p;
    m_rHeader = de::Recti(0,0,w,hHeader);
    m_rFooter = de::Recti(0,h-1-hFooter,w,h-hHeader);
    m_rPreview = de::Recti(p,hHeader+p, w-p-p, hPreview);
    m_rButtons = de::Recti(p,hHeader+p+hPreview+p, w-p-p,hButtons);
    m_rPartial = de::Recti(p,hHeader+p+hPreview+p+hButtons+p, w-p-p,hPartial);
    m_rVolume = de::Recti(p,hHeader+p+hPreview+p+hButtons+p+hPartial+p, w-p-p,hVolume);
}

void Editor::doPartialDrawing()
{
    if (!m_doPartialDawing)
    {
        return;
    }

    if (!m_plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    const auto & partials = m_plugin->getSynth().getConfig().m_partials.m_partials;

    float scale = float(partials.size()) / float(m_rPartial.w);
    int bar = (m_mouseX - m_rPartial.x) * scale;
    if (bar >= 0 && bar < partials.size())
    {
        float t = (float(m_mouseY) - float(m_rPartial.y)) / float(m_rPartial.h);
        float A = std::clamp(1.0f - t, 0.0f, 1.0f);
        m_plugin->setParameter(bar, A);
    }
}

void Editor::mousePressEvent( const de::MousePressEvent& event )
{
    if (event.isLeft())
    {
        m_doPartialDawing = true;
    }
    doPartialDrawing();
}

void Editor::mouseReleaseEvent( const de::MouseReleaseEvent& event )
{
    if (event.isLeft())
    {
        m_doPartialDawing = false;
    }
    doPartialDrawing();
}

void Editor::mouseMoveEvent( const de::MouseMoveEvent& event )
{
    m_mouseX = event.x;
    m_mouseY = event.y;

    doPartialDrawing();
}

void Editor::mouseWheelEvent( const de::MouseWheelEvent& event )
{
}

void Editor::mouseDblClickEvent( const de::MouseDblClickEvent& event )
{
}

void Editor::keyPressEvent(const de::KeyPressEvent& event)
{
    if (event.key == de::KEY_ESCAPE)
    {
        //glfwSetWindowShouldClose(window, true); // Close window on ESC
    }

    if (!m_plugin)
    {
        DE_ERROR("No plugin")
        return;
    }

    if (event.key == de::KEY_1)
    {
        m_plugin->getSynth().getConfig().m_partials.makeRect();
    }

    if (event.key == de::KEY_2)
    {
        m_plugin->getSynth().getConfig().m_partials.makeSaw();
    }

    if (event.key == de::KEY_3)
    {
        m_plugin->getSynth().getConfig().m_partials.makeSawRev();
    }

    if (event.key == de::KEY_4)
    {
        m_plugin->getSynth().getConfig().m_partials.makeTriangle();
    }
}


void Editor::keyReleaseEvent(const de::KeyReleaseEvent& event)
{
}

/*
void Editor::run()
{
    double timeInSecNow = glfwGetTime();
    double timeLastFrameBufferUpdate = 0;
    double timeLastWindowTitleUpdate = 0;
    while (!glfwWindowShouldClose(m_window) && !shouldClose)
    {
        timeInSecNow = glfwGetTime();

        if (timeInSecNow - timeLastFrameBufferUpdate >= 1./30.0)
        {
            draw();
            timeLastFrameBufferUpdate = glfwGetTime();
        }

        glfwPollEvents();
    }
    close();
}

void Editor::requestClose()
{
    shouldClose = true;
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}
*/
