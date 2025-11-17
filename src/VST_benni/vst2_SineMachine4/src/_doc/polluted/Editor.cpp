#include "Editor.h"
#include "Synth.h"
#include "fonts/fonts_ShareTechMonoRegular_ttf.h"

#include <GL/glew.h>
#include <GL/wglew.h>

// ------------------ Editor Implementation ------------------

// Editor::Editor(Synth* effect)

// Editor::~Editor()
// {}



bool
Editor::create(void* parent)
{
    HWND parentHwnd = (HWND)parent;

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"MyPluginEditor";
    RegisterClassW(&wc);

    hwnd = CreateWindowExW( 0,
                            wc.lpszClassName,
                            L"My Plugin",
                            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
                            0, 0, m_screenWidth, m_screenHeight,
                            parentHwnd,
                            nullptr,
                            wc.hInstance,
                            this);

    hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32
    };
    int pf = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pf, &pfd);
    glrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, glrc);

    glewExperimental = GL_TRUE;
    glewInit();

    m_vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    m_fontShareTechMonoRegular = nvgCreateFontMem(m_vg, "ShareTechMonoRegular",
                                                  const_cast<unsigned char*>(fonts_ShareTechMonoRegular_ttf),
                                                  fonts_ShareTechMonoRegular_ttf_len, 0);

    updateLayout();

    return true;
}


void Editor::destroy()
{
    nvgDeleteGL3(m_vg);
    // glfwDestroyWindow(m_window);
    // glfwTerminate();
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(glrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
    hwnd = nullptr;
}

void Editor::render()
{
    int32_t w = m_screenWidth;
    int32_t h = m_screenHeight;

    //int winW, winH;
    //glfwGetFramebufferSize(m_window, &winW, &winH);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

    m_preview.update( m_synth->m_partials );
    m_preview.updatePoints( m_rPreview );
    m_preview.draw( m_vg, m_rPreview );

    // Draw overtone bars
    for (int i = 0; i < NUM_PARTIALS; ++i)
    {
        const auto & partial = m_synth->m_partials[i];
        float amp = partial.amplitude;
        float barW = float(m_rPartial.w) / float(NUM_PARTIALS);
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

LRESULT CALLBACK Editor::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Editor* self = reinterpret_cast<Editor*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (msg == WM_CREATE)
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return 0;
    }

    if (!self) return DefWindowProc(hwnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_DESTROY:
    {    //glwin->_d->m_receiver = nullptr;
        //PostQuitMessage(0);
        return 0;
    }
    case WM_ERASEBKGND:
    {
        return 0;
    }
    case WM_PAINT:
    {
        // BeginPaint validates automatically, but if you skip it:
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);

        // Do OpenGL rendering here
        self->render();
        SwapBuffers(self->hdc);
        // Mark the client area as valid (no more WM_PAINT until you invalidate)
        ValidateRect(hwnd, nullptr);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_SIZE:
    {
        int w = LOWORD(lParam);
        int h = HIWORD(lParam);
        self->resizeEvent(w,h);
        return 0;
    }
    case WM_MOVE:
    {
        //MoveEvent moveEvent;
        //moveEvent.x = GET_X_LPARAM( lParam );
        //moveEvent.y = GET_Y_LPARAM( lParam );
        //glwin->onEvent( moveEvent );
        return 0;
    }
    // case WM_KEYDOWN:
    //     if (wParam == VK_F11) self->toggleFullscreen();
    //     return 0;

    // Mouse events:
    case WM_LBUTTONDBLCLK: {
        const int mx = GET_X_LPARAM(lParam);
        const int my = GET_Y_LPARAM(lParam);
        const bool bCtrl = (wParam & MK_CONTROL) != 0;
        const bool bShift = (wParam & MK_SHIFT) != 0;
        const bool bAlt = (wParam & MK_ALT) != 0;

        if ( self )
        {
            de::MouseDblClickEvent myEvt;
            myEvt.x = mx;
            myEvt.y = my;
            myEvt.buttons = de::MouseButton::Left;
            myEvt.flags = de::MouseFlag::DoubleClick;
            if (bCtrl) { myEvt.flags |= de::MouseFlag::WithCtrl; }
            if (bShift) { myEvt.flags |= de::MouseFlag::WithShift; }
            if (bAlt) { myEvt.flags |= de::MouseFlag::WithAlt; }
            self->mouseDblClickEvent( myEvt );
        }
        return 0;
    }
    case WM_RBUTTONDBLCLK: {
        const int mx = GET_X_LPARAM(lParam);
        const int my = GET_Y_LPARAM(lParam);
        const bool bCtrl = (wParam & MK_CONTROL) != 0;
        const bool bShift = (wParam & MK_SHIFT) != 0;
        const bool bAlt = (wParam & MK_ALT) != 0;

        if ( self )
        {
            de::MouseDblClickEvent myEvt;
            myEvt.x = mx;
            myEvt.y = my;
            myEvt.buttons = de::MouseButton::Right;
            myEvt.flags = de::MouseFlag::DoubleClick;
            if (bCtrl) { myEvt.flags |= de::MouseFlag::WithCtrl; }
            if (bShift) { myEvt.flags |= de::MouseFlag::WithShift; }
            if (bAlt) { myEvt.flags |= de::MouseFlag::WithAlt; }
            self->mouseDblClickEvent( myEvt );
        }
        return 0;
    }
    case WM_MBUTTONDBLCLK: {
        const int mx = GET_X_LPARAM(lParam);
        const int my = GET_Y_LPARAM(lParam);
        const bool bCtrl = (wParam & MK_CONTROL) != 0;
        const bool bShift = (wParam & MK_SHIFT) != 0;
        const bool bAlt = (wParam & MK_ALT) != 0;

        if ( self )
        {
            de::MouseDblClickEvent myEvt;
            myEvt.x = mx;
            myEvt.y = my;
            myEvt.buttons = de::MouseButton::Middle;
            myEvt.flags = de::MouseFlag::DoubleClick;
            if (bCtrl) { myEvt.flags |= de::MouseFlag::WithCtrl; }
            if (bShift) { myEvt.flags |= de::MouseFlag::WithShift; }
            if (bAlt) { myEvt.flags |= de::MouseFlag::WithAlt; }
            self->mouseDblClickEvent( myEvt );
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        int mx = int( LOWORD( lParam ) );
        int my = int( HIWORD( lParam ) );

        if ( self )
        {
            de::MouseMoveEvent myEvt;
            myEvt.x = mx;
            myEvt.y = my;
            self->mouseMoveEvent( myEvt );
        }
        return 0;
    }
    case WM_MOUSEWHEEL: {
        if ( self )
        {
            float mouseWheelY = float( int16_t( HIWORD( wParam ) ) ) / float( WHEEL_DELTA );

            de::MouseWheelEvent myEvt;
            myEvt.x = 0.0f;
            myEvt.y = mouseWheelY;
            self->mouseWheelEvent( myEvt );
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        if ( self )
        {
            de::MousePressEvent myEvt;
            myEvt.x = LOWORD(lParam);
            myEvt.y = HIWORD(lParam);
            myEvt.buttons = de::MouseButton::Left;
            myEvt.flags = de::MouseFlag::Pressed;
            //myEvt.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //myEvt.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            self->mousePressEvent( myEvt );
        }
        return 0;
    }
    case WM_RBUTTONDOWN: {
        if ( self )
        {
            de::MousePressEvent myEvt;
            myEvt.x = LOWORD(lParam);
            myEvt.y = HIWORD(lParam);
            myEvt.buttons = de::MouseButton::Right;
            myEvt.flags = de::MouseFlag::Pressed;
            //myEvt.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //myEvt.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //DE_OK("MousePressEvent = ", myEvt.str())
            self->mousePressEvent( myEvt );
        }
        return 0;
    }
    case WM_MBUTTONDOWN: {
        if ( self )
        {
            de::MousePressEvent myEvt;
            myEvt.x = LOWORD(lParam);
            myEvt.y = HIWORD(lParam);
            myEvt.buttons = de::MouseButton::Middle;
            myEvt.flags = de::MouseFlag::Pressed;
            //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //DE_OK("MousePressEvent = ", mousePressEvent.str())
            self->mousePressEvent( myEvt );
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        if ( self )
        {
            de::MouseReleaseEvent myEvt;
            myEvt.x = LOWORD(lParam);
            myEvt.y = HIWORD(lParam);
            myEvt.buttons = de::MouseButton::Left;
            myEvt.flags = de::MouseFlag::Released;
            //myEvt.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //myEvt.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //DE_OK("MouseReleaseEvent = ", myEvt.str())
            self->mouseReleaseEvent( myEvt );
        }
        return 0;
    }
    case WM_RBUTTONUP: {
        if ( self )
        {
            de::MouseReleaseEvent myEvt;
            myEvt.x = LOWORD(lParam);
            myEvt.y = HIWORD(lParam);
            myEvt.buttons = de::MouseButton::Right;
            myEvt.flags = de::MouseFlag::Released;
            //myEvt.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //myEvt.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //DE_OK("MouseReleaseEvent = ", myEvt.str())
            self->mouseReleaseEvent( myEvt );
        }
        return 0;
    }
    case WM_MBUTTONUP: {
        if ( self )
        {
            de::MouseReleaseEvent myEvt;
            myEvt.x = LOWORD(lParam);
            myEvt.y = HIWORD(lParam);
            myEvt.buttons = de::MouseButton::Middle;
            myEvt.flags = de::MouseFlag::Released;
            //myEvt.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //myEvt.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //DE_OK("MouseReleaseEvent = ", myEvt.str())
            self->mouseReleaseEvent( myEvt );
        }
        return 0;
    }

        //case WM_XBUTTONDOWN:
        //case WM_XBUTTONUP:

        // === KeyboardEvents: ===

    case WM_INPUTLANGCHANGE:
    {
        if ( self )
        {
            auto hkl = GetKeyboardLayout( 0 ); // get the new codepage used for keyboard input
            self->_d->m_KEYBOARD_INPUT_HKL = hkl; // get the new codepage used for keyboard input
            self->_d->m_KEYBOARD_INPUT_CODEPAGE = convertLocaleIdToCodepage( LOWORD( hkl ) );
        }
        return 0;
    }
    case WM_SYSKEYDOWN:{
        if ( self )
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

            de::KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyPressEvent );
        }

        return 0;
        //         if (message == WM_SYSKEYDOWN)
        //            return DefWindowProc(hwnd, message, wParam, lParam);
        //         else
        //            return 0;
        //         break;
    }
    case WM_KEYDOWN: {
        if ( glwin )
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
                                               glwin->_d->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
                WORD unicodeChar;
                ::MultiByteToWideChar( glwin->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
                singleChar = unicodeChar;
                unicode = unicodeChar;
            }

            de::KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyPressEvent );
        }

        //         if (message == WM_SYSKEYDOWN)
        //            return DefWindowProc(hwnd, message, wParam, lParam);
        //         else
        //            return 0;

        return 0;
    }

    case WM_SYSKEYUP:
    {
        if ( glwin )
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
                                               glwin->_d->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
                WORD unicodeChar;
                ::MultiByteToWideChar( glwin->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
                singleChar = unicodeChar;
                unicode = unicodeChar;
            }

            de::KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyReleaseEvent );
        }
        //         if (message == WM_SYSKEYUP)
        //            return DefWindowProc(hwnd, message, wParam, lParam);
        //         else
        //            return 0;
        //         break;
        return 0;
    }
    case WM_KEYUP: {
        if ( glwin )
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
                                               glwin->_d->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
                WORD unicodeChar;
                ::MultiByteToWideChar( glwin->_d->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
                singleChar = unicodeChar;
                unicode = unicodeChar;
            }

            de::KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= KeyModifier::Ctrl;
            glwin->onEvent( keyReleaseEvent );
        }
        return 0;
        //         if (message == WM_SYSKEYUP)
        //            return DefWindowProc(hwnd, message, wParam, lParam);
        //         else
        //            return 0;
    }

    case WM_SYSCOMMAND:
        //      {
        //         if ( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
        //              ( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
        //              ( wParam & 0xFFF0 ) == SC_KEYMENU )
        //         {
        //            return 0; // prevent screensaver or monitor powersave mode from starting
        //         }
        //      }
        //
        break;
    case WM_USER:
        //event.UserEvent.UserData1 = (irr::s32)wParam;
        //event.UserEvent.UserData2 = (irr::s32)lParam;
        return 0;
    case WM_SETCURSOR:
#if 0
   dev = getDeviceFromHWnd(hWnd); // because Windows forgot about that in the meantime
      if (dev)
      {
         dev->getCursorControl()->setActiveIcon( dev->getCursorControl()->getActiveIcon() );
         dev->getCursorControl()->setVisible( dev->getCursorControl()->isVisible() );
      }
#endif
        break;
    case WM_COMMAND:
    {
        /*
      switch( wParam )
      {
      case ID_FILE_EXIT:
         {
            DestroyWindow( hwnd );
         }
         break;
      case ID_FILE_LOAD:
         {
            std::string uri = dbOpenFileA();
         }
         break;
      case ID_FILE_SAVE:
         {
         }
         break;
      case ID_HELP_ABOUT:
         {
            // doModalAboutDialog(m_hWindow);
         }
         break;
      default:
         break;
      }
*/
    } break;

    case WM_SETFOCUS:
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void Editor::toggleFullscreen()
{
    fullscreen = !fullscreen;
    SetWindowLong(hwnd, GWL_STYLE, fullscreen ? WS_POPUP : WS_CHILD | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, fullscreen ? 1920 : 800, fullscreen ? 1080 : 600, SWP_FRAMECHANGED);
}


void Editor::updateLayout()
{
    int32_t w = m_screenWidth;
    int32_t h = m_screenHeight;

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

    float scale = float(NUM_PARTIALS) / float(m_rPartial.w);
    int bar = (m_mouseX - m_rPartial.x) * scale;
    if (bar >= 0 && bar < NUM_PARTIALS)
    {
        float t = (float(m_mouseY) - float(m_rPartial.y)) / float(m_rPartial.h);
        float A = std::clamp(1.0f - t, 0.0f, 1.0f);
#if 0
        m_synth->setParameter(bar, A);
#else
        m_synth->setPartial(bar, A);
#endif
    }
}

void Editor::resizeEvent( const de::ResizeEvent& event )
{
    int w = event.w;
    int h = event.h;

    m_screenWidth = w;
    m_screenHeight = h;

    // Optional: update NanoVG or layout logic
    //glViewport(0, 0, w, h);
    updateLayout();
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

void Editor::keyPressEvent(const de::KeyPressEvent& event)
{
    if (event.key == de::KEY_ESCAPE)
    {
        //glfwSetWindowShouldClose(window, true); // Close window on ESC
    }

    if (event.key == de::KEY_1)
    {
        m_synth->setPartialsToRect();
    }

    if (event.key == de::KEY_2)
    {
        m_synth->setPartialsToSaw();
    }

    if (event.key == de::KEY_3)
    {
        m_synth->setPartialsToSawRev();
    }

    if (event.key == de::KEY_4)
    {
        m_synth->setPartialsToTriangle();
    }
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
