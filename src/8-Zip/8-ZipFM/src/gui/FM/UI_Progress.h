#pragma once
#include <gui/XP_ProgressBar.h>

// =============================================================
class UI_Progress : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Button* btnCompare;
    Fl_Progress* progress;
    Fl_Button* btnCancel;
    Fl_Button* btnDarkMode;

    UI_Progress(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (150 - m_spacing) * zoom;
        int w3 = (100 - m_spacing) * zoom;
        int w4 = (120 - m_spacing) * zoom;
        int w2 = W - w1 - w3 - w4 - 3*s;
        int x = X;
        int y = Y;
        btnCompare = new Fl_Button(x,y,w1,H,"Compare in/out"); x += w1 + s;
        progress = new XP_ProgressBar(x,y,w2,H); x += w2 + s;
        btnCancel = new Fl_Button(x,y,w3,H,"Cancel"); x += w3 + s;
        btnDarkMode = new Fl_Button(x,y,w4,H,"DarkMode");

        progress->minimum(0);
        progress->maximum(1);
        progress->value(0);

        end();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (150 - m_spacing) * zoom;
        int w3 = (100 - m_spacing) * zoom;
        int w4 = (120 - m_spacing) * zoom;
        int w2 = W - w1 - w3 - w4 - 3*s;

        int x = X;
        int y = Y;
        btnCompare->resize(x,y,w1,H); x += w1 + s;
        progress->resize(x,y,w2,H); x += w2 + s;
        btnCancel->resize(x,y,w3,H); x += w3 + s;
        btnDarkMode->resize(x,y,w4,H);
    }
};


/*
void hscroll_cb(Fl_Widget* w, void* data)
{
    auto scrollBar = (Fl_Scrollbar*)w;

    // ui.waveform->setZoomStart( scrollBar->value() );

    ui.waveform->waveform->setZoomFromScrollBar( scrollBar->value() );
}
*/
