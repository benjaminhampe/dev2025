#pragma once
#include "Base.h"

/*
// =============================================================
class UI_Encoder : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Choice* encoder = nullptr;
    Fl_Box* lblBitrate = nullptr;
    Fl_Choice* bitrate = nullptr;
    Fl_Box* lblQuality = nullptr;
    Fl_Choice* quality = nullptr;
    Fl_Check_Button* vbr = nullptr;

    UI_Encoder(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (100 - m_spacing) * zoom;
        int w2 = (100 - m_spacing) * zoom;
        int w3 = (100 - m_spacing) * zoom;
        int w4 = (100 - m_spacing) * zoom;
        int w5 = (100 - m_spacing) * zoom;
        int w6 = (100 - m_spacing) * zoom;

        int x = X;
        int y = Y;
        encoder = new Fl_Choice(x,y,w1,H,""); x += w1 + s;
        lblBitrate = new Fl_Box(x,y,w2,H,"Bitrate:"); x += w2 + s;
        bitrate = new Fl_Choice(x,y,w3,H,""); x += w3 + s;
        lblQuality = new Fl_Box(x,y,w4,H,"Quality:"); x += w4 + s;
        quality = new Fl_Choice(x,y,w5,H,""); x += w5 + s;
        vbr = new Fl_Check_Button(x,y,w6,H,"VBR (Variable Bitrate):");

        bitrate->add("8 - Speech Very Low");
        bitrate->add("16 - Speech Low");
        bitrate->add("24 - Speech Medium");
        bitrate->add("32 - Speech Good");
        bitrate->add("48 - Very Low");
        bitrate->add("64 - Lower");
        bitrate->add("96 - Low");
        bitrate->add("128 - OK");
        bitrate->add("160 - Medium");
        bitrate->add("192 - Better");
        bitrate->add("224 - Good");
        bitrate->add("256 - Very Good");
        bitrate->add("320 - Highest");
        bitrate->value(7);

        quality->add("0 - Best");
        quality->add("1 - High");
        quality->add("5 - Default");
        quality->add("7 - Fast");
        quality->add("9 - Fastest");
        quality->value(0);

        end();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (100 - m_spacing) * zoom;
        int w2 = (100 - m_spacing) * zoom;
        int w3 = (100 - m_spacing) * zoom;
        int w4 = (100 - m_spacing) * zoom;
        int w5 = (100 - m_spacing) * zoom;
        int w6 = (100 - m_spacing) * zoom;

        int x = X;
        int y = Y;
        encoder->resize(x,y,w1,H); x += w1 + s;
        lblBitrate->resize(x,y,w2,H); x += w2 + s;
        bitrate->resize(x,y,w3,H); x += w3 + s;
        lblQuality->resize(x,y,w4,H); x += w4 + s;
        quality->resize(x,y,w5,H); x += w5 + s;
        vbr->resize(x,y,w6,H);
    }

    int getBitrate()
    {
        static int bitrate_map[] = {8,16,24,32,48,64,96,128,160,192,224,256,320};
        return bitrate_map[bitrate->value()];
    }

    int getQuality()
    {
        int quality_map[] = {0,1,5,7,9};
        return quality_map[quality->value()];
    }
};
*/
