#pragma once
//#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Gl_Window.H>

#include <de/sound/Sound.h>
#include <de/sound/Sound2Image.h>
//#include <de/sound/SoundAccessorF32.h>

// =============================================================
class GL_WaveformWidget :
#ifdef USE_BENNI_GL
    public Fl_Gl_Window
#else
    public Fl_Widget
#endif
// =============================================================
{
private:
    de::Sound* m_sound = nullptr;
    de::SampleTypeConverter::Converter_t m_converter = nullptr;
    de::TAlignedVector<float> m_sampleBuf;
    //de::SoundAccessorF32 m_accessor;
    //Fl_Scrollbar* m_scroll = nullptr;

    double m_zoom = 1.0;
    int64_t m_zoomBeg = 0;
    int64_t m_zoomEnd = 0;
    // int64_t m_zoomFrameCount = 0;

    int64_t m_loopBeg = -1;
    int64_t m_loopEnd = -1;
    // int64_t m_loopFrameCount = 0;

    int64_t m_mouseFrame = -1;

    bool m_selecting = false;

    bool m_bHovered = false; // Enter/Leave

    bool m_bImageDirty = true; // Enter/Leave
#ifdef USE_BENNI_GL
#else
    de::Image m_img;
#endif

    void updateImage();

public:
    GL_WaveformWidget(int X, int Y, int W, int H);
    void setSound(de::Sound* snd);

    void setZoomStart(double pc);
    void setZoom(double z);
    double zoom() const { return m_zoom; }

    int64_t zoomBeg() const { return m_zoomBeg; }
    int64_t zoomEnd() const { return m_zoomEnd; }
    int64_t zoomDelta() const { return m_zoomEnd - m_zoomBeg; }

    int64_t loopBeg() const { return m_loopBeg; }
    int64_t loopEnd() const { return m_loopEnd; }
    int64_t loopDelta() const { return m_loopEnd - m_loopBeg; }

    double getFramesPerPixel()
    {
        auto a = zoomDelta();
#ifdef USE_BENNI_GL
        auto b = pixel_w();
#else
        auto b = w();
#endif
        if (a < 1) return 1.0;
        if (b < 1) return 1.0;
        return double(a) / double(b);
    }

public:
    void resize(int X, int Y, int W, int H) override;

    void draw() override;

    int handle(int e) override;
private:
#ifdef USE_EXTERNAL_SCROLLBAR
    //static void scroll_cb(Fl_Widget*, void* userdata);
    // --- 64-bit safe scroll range ---
    //void update_scroll_range();
#endif
    // --- 64-bit safe pixel→frame ---
    int64_t pixelToFrame(int px);
};

