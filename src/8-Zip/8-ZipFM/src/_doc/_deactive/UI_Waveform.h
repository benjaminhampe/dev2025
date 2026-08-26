#pragma once
#include "Base.h"

/*
// =============================================================
class UI_Waveform : public Fl_Group
{
public:

    Fl_HScrollBarLite* scrollBar = nullptr;
    // Fl_Button* zoomIn = nullptr;
    // Fl_Button* zoomOut = nullptr;

    void setDarkMode(bool bDarkMode)
    {
        scrollBar->setDarkMode(bDarkMode);
    }

    UI_Waveform(int X, int Y, int W, int H)
        : Fl_Group(X, Y, W, H)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int h2 = zoom * 24;
        int h1 = H - h2 - 5;

        int x = X;
        int y = Y;
        // ui.zoomIn  = new Fl_Button(550, y,      40, (c-d)/2, "+");
        // ui.zoomIn->tooltip("Zoom in");
        // ui.zoomIn->callback([](Fl_Widget*, void* ud)
        // {
        //     ((WaveformWidget*)ud)->setZoom(((WaveformWidget*)ud)->getZoom() * 1.2f);
        // }, ui.inWavf);
        // ui.zoomOut = new Fl_Button(550, y + c/2, 40, (c-d)/2, "-");
        // ui.zoomOut->tooltip("Zoom out");
        // ui.zoomOut->callback([](Fl_Widget*, void* ud)
        // {
        //     ((WaveformWidget*)ud)->setZoom(((WaveformWidget*)ud)->getZoom() * 0.8f);
        // }, ui.inWavf);
        scrollBar = new Fl_HScrollBarLite(x,y,W,h2);
        scrollBar->type(FL_HORIZONTAL);
        scrollBar->set_slider_pos(0);
        scrollBar->set_slider_size(1);
        // scrollBar->bounds(0.0, 1.0);
        // scrollBar->step(1.0e-9);
        // scrollBar->precision(9);
        // scrollBar->value(0.0);
        // scrollBar->slider_size(1.0);
        //scrollBar->callback(hscroll_cb, nullptr);

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        int x = X;
        int y = Y;

        float zoom = Fl::screen_scale(0);
        //int s = m_spacing * zoom;
        int h2 = zoom * 24;
        int h1 = H - h2 - 5;

        scrollBar->resize(x,y,W,h2);
    }
};
*/