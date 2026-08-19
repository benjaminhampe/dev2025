#pragma once
#include "Config.h"

class Synth;
class Plugin;

// 📊
struct Preview
{
    struct Curve
    {
        int partial;

        float amplitude;

        float amplitudeSum;

        NVGcolor color;

        de::TAlignedVector<float> original;     // y[n]

        de::TAlignedVector<float> scaled;       // A_n * y[n]

        de::TAlignedVector<float> accum;        // sum(A_n * y[n],n,0,N)

        de::TAlignedVector<float> normalized;   // A_sum_inv * sum(A_n * y[n],n,0,N)

        de::TAlignedVector<glm::vec2> points;   // Screen coords in pixel
    };

    std::vector<Curve> m_curves;

    Curve m_accum;

    void init( const Cfg & cfg, int n = 1024 );

    void update( const Cfg & cfg );

    void updatePoints( de::Recti pos, int n = 1024 );

    void draw(NVGcontext* vg, de::Recti pos, int n = 1024 );

protected:
    void drawCurve(NVGcontext* vg, const Curve & curve, float strokeWidth);
};

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
