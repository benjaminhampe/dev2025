#pragma once
//#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Scrollbar.H>
#include <FL/Fl_Gl_Window.H>

#include <de/sound/Sound.h>
#include <de/sound/SoundAccessorF32.h>

// =============================================================
class GL_WaveformWidget : public Fl_Gl_Window
// =============================================================
{
private:
    de::Sound* m_sound = nullptr;
    de::SoundAccessorF32 m_accessor;
    Fl_Scrollbar* m_scroll = nullptr;

    double m_zoom = 1.0;
    int64_t m_zoomFrameStart = 0;
    // int64_t m_zoomFrameCount = 0;

    int64_t m_loopFrameStart = -1;
    int64_t m_loopFrameEnd   = -1;
    // int64_t m_loopFrameCount = 0;

    bool m_selecting = false;

public:
    GL_WaveformWidget(int X, int Y, int W, int H);
    void setSound(de::Sound* snd);
    void setZoom(double z);
    double getZoom() const;
    int64_t startFrame() const;
    int64_t endFrame()   const;

    double getFramesPerPixel()
    {
        int32_t b = pixel_w();
        if (b < 1) return 1.0;
        return (double(m_sound->m_frames) * m_zoom) / double(b);
    }

protected:
    void draw() override;
    int handle(int e) override;
private:

    static void scroll_cb(Fl_Widget*, void* userdata);
    // --- 64-bit safe scroll range ---
    void update_scroll_range();
    // --- 64-bit safe pixel→frame ---
    int64_t pixelToFrame(int px);
};

