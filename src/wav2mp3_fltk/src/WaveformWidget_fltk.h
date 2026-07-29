#pragma once
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Scrollbar.H>

#include <de/sound/Sound.h>
#include <de/sound/SoundAccessorF32.h>

class WaveformWidget : public Fl_Widget
{
private:
    de::Sound* m_sound = nullptr;
    de::SoundAccessorF32 m_accessor;
    Fl_Scrollbar* m_scroll = nullptr;

    double m_zoom = 1.0;

    int64_t m_startFrame = -1;
    int64_t m_endFrame   = -1;

    bool m_selecting = false;

public:
    WaveformWidget(int X, int Y, int W, int H);
    void setSound(de::Sound* snd);
    void setZoom(double z);
    double getZoom() const;
    int64_t startFrame() const;
    int64_t endFrame()   const;

protected:
    void draw() override;
    int handle(int e) override;
private:

    static void scroll_cb(Fl_Widget*, void* userdata);
    // --- 64-bit safe scroll range ---
    void update_scroll_range();
    // --- 64-bit safe pixel→frame ---
    int64_t pixelToFrame(int px) const;
    // --- Time ruler + grid ---
    void draw_time_ruler_and_grid(int64_t startFrame, double framesPerPixel);
};

