#pragma once
#include "Globals.h"
#include "Preview.h"

#include <de/os/Events.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <dwmapi.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//#include <uxtheme.h>
//#include <vssym32.h>  // for DPI awareness? We use embedded resource .xml for that.
//#include <commctrl.h>
#include <wchar.h>
#include <versionhelpers.h>
//#include <commdlg.h>
//#include <shellapi.h>
//#include <shlobj.h>
//#include <wctype.h>
//#include <dinput.h>   // For JOYCAPS
//#include <xinput.h>   // For JOYCAPS
#include <mmsystem.h> // For JOYCAPS

class Synth;

class Editor
{
public:
    Editor(Synth* effect)
        : m_synth(effect)
        , m_vg(nullptr)
        , m_screenWidth(800)
        , m_screenHeight(600)
        , m_mouseX(0)
        , m_mouseY(0)
        , m_doPartialDawing(false)
    {
        m_preview.init( m_synth->m_partials );
    }

    ~Editor()
    {

    }
    bool create(void* parent);
    void destroy();
    void toggleFullscreen();

    // void run();
    // void requestClose();

    void render();
    void draw();
    void updateLayout();
    void doPartialDrawing();

    void resizeEvent( const de::ResizeEvent& event );
    void moveEvent( const de::MoveEvent& event );
    void mouseDblClickEvent( const de::MouseDblClickEvent& event );
    //void mouseButtonEvent( const de::MouseButtonEvent& event );
    void mousePressEvent( const de::MousePressEvent& event );
    void mouseReleaseEvent( const de::MouseReleaseEvent& event );
    void mouseMoveEvent( const de::MouseMoveEvent& event );
    void mouseWheelEvent( const de::MouseWheelEvent& event );
    //void keyEvent( const de::KeyEvent& event );
    void keyPressEvent( const de::KeyPressEvent& event );
    void keyReleaseEvent( const de::KeyReleaseEvent& event );

private:
    Synth* m_synth;
    NVGcontext* m_vg;
    int32_t m_screenWidth;
    int32_t m_screenHeight;
    int32_t m_mouseX;
    int32_t m_mouseY;

    HWND hwnd = nullptr;
    HDC hdc = nullptr;
    HGLRC glrc = nullptr;
    bool fullscreen = false;

    bool m_doPartialDawing;

    int m_fontNotoEmojiMedium;
    int m_fontShareTechMonoRegular;

    de::Recti m_rHeader;
    de::Recti m_rFooter;
    de::Recti m_rPreview;
    de::Recti m_rButtons;
    de::Recti m_rPartial;
    de::Recti m_rVolume;

    Preview m_preview;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

namespace {

    //🖌️ Drawing a Colored Rectangle Border
    inline void drawLineRect( NVGcontext* vg,
        int x, int y, int w, int h, const NVGcolor& color, float strokeWidth = 4.0f)
    {
        nvgBeginPath(vg);                     // Start a new path
        nvgRect(vg, x, y, w, h);              // Define the rectangle
        nvgStrokeWidth(vg, strokeWidth);     // Set border thickness
        nvgStrokeColor(vg, color); // Set border color (red)
        nvgStroke(vg);                        // Render the stroke
    }

    //🖌️ Drawing a Colored Rectangle Border
    inline void drawLineRect( NVGcontext* vg,
        const de::Recti& pos, const NVGcolor& color, float strokeWidth = 4.0f)
    {
        drawLineRect(vg,pos.x,pos.y,pos.w,pos.h,color,strokeWidth);
    }

} // end namespace.
