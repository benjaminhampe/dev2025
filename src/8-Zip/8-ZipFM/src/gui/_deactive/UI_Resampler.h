#pragma once
#include "Base.h"
/*
// =============================================================
class UI_Resampler : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Button* start = nullptr;
    Fl_Choice* rate = nullptr;
    Fl_Choice* algo = nullptr;

    UI_Resampler(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (100 - m_spacing) * zoom;
        int w4 = W - w1 - 3*s;
        int w2 = w4/2;
        int w3 = w4/2;
        int x = X;
        int y = Y;
        start = new Fl_Check_Button(x, y, w1, H,"Resample"); x += w1 + s;
        rate = new Fl_Choice(x,y,w2,H); x += w2 + s;
        algo = new Fl_Choice(x,y,w3,H);

        rate->add("4000 Hz");
        rate->add("8000 Hz");
        rate->add("16000 Hz");
        rate->add("22050 Hz");
        rate->add("32000 Hz");
        rate->add("44100 Hz");
        rate->add("48000 Hz");
        rate->add("64000 Hz");
        rate->add("88200 Hz");
        rate->add("96000 Hz");
        rate->add("128000 Hz");
        rate->add("192000 Hz");
        rate->value(6);

        algo->add("r8brain");
        algo->value(0);

        end(); // wichtig

        DE_DEBUG("panel", this->x(), ",", this->y(), ",", this->w(), ",", this->h())
        //DE_DEBUG("label", label->x(), ",", label->y(), ",", label->w(), ",", label->h())
        DE_DEBUG("start", start->x(), ",", start->y(), ",", start->w(), ",", start->h())
        DE_DEBUG("rate", rate->x(), ",", rate->y(), ",", rate->w(), ",", rate->h())
        DE_DEBUG("algo", algo->x(), ",", algo->y(), ",", algo->w(), ",", algo->h())

    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (100 - m_spacing) * zoom;
        int w4 = W - w1 - 3*s;
        int w2 = w4/2;
        int w3 = w4/2;
        int x = X;
        int y = Y;
        start->resize(x,y,w1,H); x += w1 + s;
        rate->resize(x,y,w2,H); x += w2 + s;
        algo->resize(x,y,w3,H);
    }
};
*/