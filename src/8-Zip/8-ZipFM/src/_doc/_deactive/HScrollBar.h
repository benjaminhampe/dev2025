#pragma once
#include "Base.h"

// =============================================================
class Fl_HScrollBarLite : public Fl_Widget
{
public:
    bool m_bDarkMode = false;
/*

    Fl_HScrollBarLite* sb = new Fl_HScrollBarLite(10, 10, 300, 20);

    sb->set_slider_pos(0.0);
    sb->set_slider_size(0.1);

    sb->onChange = [&](double pos) { printf("Slider moved: %f\n", pos); };
*/
    // Normalized [0..1] slider position and width
    double sliderPos  = 0.0;   // left edge
    double sliderSize = 0.2;   // width

    // Callback when sliderPos changes
    std::function<void(double)> onChange;

    Fl_HScrollBarLite(int X, int Y, int W, int H)
        : Fl_Widget(X, Y, W, H) {}

    void setDarkMode(bool bDarkMode)
    {
        m_bDarkMode = bDarkMode;
        redraw();
    }

    // --- Getters / Setters ---
    void set_slider_pos(double p) {
        sliderPos = clamp01(p);
        redraw();
        if (onChange) onChange(sliderPos);
    }

    void set_slider_size(double s) {
        sliderSize = clamp01(s);
        redraw();
    }

    double get_slider_pos()  const { return sliderPos; }
    double get_slider_size() const { return sliderSize; }

    // --- Drawing ---
    void draw() override
    {
        fl_push_clip(x(), y(), w(), h());

        // Background
        fl_color(FL_GRAY); // m_bDarkMode ? FL_BLACK : FL_GRAY // fl_color(FL_DARK3);
        fl_rectf(x(), y(), w(), h());

        // Border
        // fl_color(m_bDarkMode ? FL_WHITE : FL_BLACK);
        // fl_rect(x(), y(), w(), h());

        // Slider geometry in pixels
        int sx = x() + int(sliderPos * w());
        int sw = std::lroundf(sliderSize * w());

        // Slider
        if (sw < w())
        {
            fl_color(m_bDarkMode ? FL_LIGHT2 : FL_WHITE);
            fl_rectf(sx, y() + 1, sw, h() - 2);
        }
        // fl_color(FL_BLACK);
        // fl_rect(x(), y(), w(), h());



        fl_pop_clip();
    }

    // --- Event handling ---
    int handle(int event) override
    {
        switch (event)
        {
        case FL_PUSH:
        case FL_DRAG:
        {
            double mx = Fl::event_x();
            double local = (mx - x()) / double(w());

            // Center slider under mouse
            double newPos = local - sliderSize * 0.5;
            #if 1
            newPos = std::max(0.0, std::min(newPos, 1.0 - sliderSize));
            #else
            newPos = clamp01(newPos);
            #endif
            if (newPos != sliderPos)
            {
                sliderPos = newPos;
                redraw();
                if (onChange) onChange(sliderPos);
            }
            return 1;
        }
        case FL_RELEASE:
            return 1;
        }
        return 0;
    }

    // --- Resize handling ---
    void resize(int X, int Y, int W, int H) override {
        Fl_Widget::resize(X, Y, W, H);
        redraw();
    }

private:
    static double clamp01(double v) {
        return std::max(0.0, std::min(1.0, v));
    }
};
