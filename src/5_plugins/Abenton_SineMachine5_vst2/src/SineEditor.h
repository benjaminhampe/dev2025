#pragma once
#include <de/Common.h>
#include "Preview.h"

// ===================================================================
// INCLUDE: VST2SDK
// ===================================================================
#include <pluginterfaces/vst2.x/audioeffectx.h>

// ===================================================================
// INCLUDE: Nanovg
// ===================================================================
#include <gui/NVG.h>
#include <gui/NVG_Widget.h>
#include <gui/NVG_Button.h>

class Plugin;
class EditorImpl;

class Editor : de::IEventReceiver
{
public:
    Editor(Plugin* plugin);
    ~Editor() override;

    ERect* getEditorRect() { return &m_erect; }
    int getScreenWidth() const { return m_erect.right - m_erect.left; }
    int getScreenHeight() const { return m_erect.bottom - m_erect.top; }
    bool create(void* parent);
    void destroy();

    void toggleFullscreen();
    // void run();
    // void requestClose();

    void updateLayout(int32_t w, int32_t h);
    void doPartialDrawing();

    void zoomIn();
    void zoomOut();
    bool resizeCommand(int newW, int newH);

    void timerEvent( const de::TimerEvent& event ) override;
    void resizeEvent( const de::ResizeEvent& event ) override;
    void paintEvent( const de::PaintEvent& event ) override;
    //void moveEvent( const de::MoveEvent& event ) override;

    void drawPreview();

    void mouseDblClickEvent( const de::MouseDblClickEvent& event ) override;
    //void mouseButtonEvent( const de::MouseButtonEvent& event ) override;
    void mousePressEvent( const de::MousePressEvent& event ) override;
    void mouseReleaseEvent( const de::MouseReleaseEvent& event ) override;
    void mouseMoveEvent( const de::MouseMoveEvent& event ) override;
    void mouseWheelEvent( const de::MouseWheelEvent& event ) override;
    //void keyEvent( const de::KeyEvent& event ) override;
    void keyPressEvent( const de::KeyPressEvent& event ) override;
    void keyReleaseEvent( const de::KeyReleaseEvent& event ) override;


public:
    EditorImpl* _d;
private:
    Plugin* m_plugin;
    NVGcontext* m_vg;
    NVG_4ColorRect m_bg1;
    // NVG_Widget* m_rootWidget;
    ERect m_erect;

    int32_t m_mouseX;
    int32_t m_mouseY;

    bool m_paintEventEnabled;
    bool m_doPartialDawing;

    int m_fontNotoEmojiMedium;
    int m_fontShareTechMonoRegular;

struct HeaderData
{
    de::Recti rc;
    de::Recti rcZoomOut;
    de::Recti rcZoomIn;

    void setRect(int x, int y, int w, int h)
    {
        rc = de::Recti(x,y,w,h);
        int bh = 3 * h / 4;
        int by = h / 8;
        x = by;
        y = by;
        rcZoomOut = de::Recti( x, y, bh, bh ); x += bh + by;
        rcZoomIn = de::Recti( x, y, bh, bh ); x += bh + by;
    }
};
    HeaderData m_header;

    de::Recti m_rFooter;
    de::Recti m_rPreview;
    de::Recti m_rButtons;
    de::Recti m_rPartial;
    de::Recti m_rVolume;



    Preview m_preview;
};
