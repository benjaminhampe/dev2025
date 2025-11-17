#pragma once
#include "Globals.h"
#include "Preview.h"

#include <de/os/Window_WGL.h>

class Synth;
class Plugin;

class Editor : de::IEventReceiver
{
public:
    Editor(Plugin* plugin);
    ~Editor() override;

    ERect* getEditorRect() { return &m_erect; }

    bool create(void* parent);
    void destroy();

    // void toggleFullscreen();
    // void run();
    // void requestClose();

    void updateLayout();
    void doPartialDrawing();

    void timerEvent( const de::TimerEvent& event ) override;
    void resizeEvent( const de::ResizeEvent& event ) override;
    void moveEvent( const de::MoveEvent& event ) override;
    void paintEvent( const de::PaintEvent& event ) override;
    void mouseDblClickEvent( const de::MouseDblClickEvent& event ) override;
    //void mouseButtonEvent( const de::MouseButtonEvent& event ) override;
    void mousePressEvent( const de::MousePressEvent& event ) override;
    void mouseReleaseEvent( const de::MouseReleaseEvent& event ) override;
    void mouseMoveEvent( const de::MouseMoveEvent& event ) override;
    void mouseWheelEvent( const de::MouseWheelEvent& event ) override;
    //void keyEvent( const de::KeyEvent& event ) override;
    void keyPressEvent( const de::KeyPressEvent& event ) override;
    void keyReleaseEvent( const de::KeyReleaseEvent& event ) override;

private:
    Plugin* m_plugin;
    de::Window_WGL* m_window;
    NVGcontext* m_vg;

    uint32_t m_updateTimerId;

    ERect m_erect;

    int32_t m_screenWidth;
    int32_t m_screenHeight;
    int32_t m_mouseX;
    int32_t m_mouseY;

    bool m_paintEventEnabled;
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
