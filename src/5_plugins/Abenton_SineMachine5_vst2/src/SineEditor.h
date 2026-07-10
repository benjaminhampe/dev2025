#pragma once
#include <de/Common.h>
#include "Preview.h"

class Synth;
class Plugin;
class EditorImpl;

class Editor : de::IEventReceiver
{
public:
    Editor(Plugin* plugin);
    ~Editor() override;

    ERect* getEditorRect() { return &m_erect; }

    bool create(void* parent);
    void destroy();

    void toggleFullscreen();
    // void run();
    // void requestClose();

    void updateLayout(int32_t w, int32_t h);
    void doPartialDrawing();

    void timerEvent( const de::TimerEvent& event ) override;
    void resizeEvent( const de::ResizeEvent& event ) override;
    void paintEvent( const de::PaintEvent& event ) override;
    //void moveEvent( const de::MoveEvent& event ) override;

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

public:
    EditorImpl* _d;

private:
    // void initGL();
    // void render();

//    de::Window_WGL* m_window;

//    uint32_t m_updateTimerId;

    NVGcontext* m_vg;

    ERect m_erect;

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
