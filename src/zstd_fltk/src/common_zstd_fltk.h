#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <algorithm>

#include <DarkImage.h>


void apply_global_font(Fl_Group* g)
{
    for (int i = 0; i < g->children(); ++i)
    {
        Fl_Widget* w = g->child(i);
        w->labelfont(FL_HELVETICA);
        w->labelsize(14);
        if (auto* sub = dynamic_cast<Fl_Group*>(w))
            apply_global_font(sub);
    }
}

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
// =============================================================
class ImageWidget : public Fl_Widget
{
    de::Image m_img;
public:
    // =============================================================
    ImageWidget(int X,int Y,int W,int H)
    // =============================================================
        : Fl_Widget(X,Y,W,H)
    {
        renderImage(W,H);
    }

    void renderImage(int w, int h)
    {
        if (w < 1 || h < 1)
            return;
        m_img = de::Image(w,h);
        m_img.fill(dbRGB(55,55,0));

        int d = std::max(1, std::min(w,h) - 2);
        int x = (w - d)/2;
        int y = (h - d)/2;
        de::Recti pos(x,y,d,d);
        de::ImagePainter::drawCircle(m_img,pos,dbRGB(255,0,0));
    }

    void draw() override
    {
        if (w() < 1 || h() < 1)
        {
            DE_WARN("Null")
            return;
        }

        fl_draw_image(m_img.data(), x(), y(), m_img.w(), m_img.h(), 4);
    }

    void resize(int X, int Y, int W, int H) override
    {
        //DE_DEBUG("Resize(",X,",",Y,",",W,",",H,")")

        Fl_Widget::resize(X, Y, W, H);

        // Reagiere hier auf neue Größe
        // z.B. internen Buffer neu anlegen

        renderImage(W,H);

        // Wenn du sofort neu zeichnen willst:
        redraw();
    }
};
// =============================================================
class UI_LogBox : public Fl_Group
{
public:
    //int m_spacing = 5;

    Fl_Text_Display* logbox;
    Fl_Text_Buffer*  logbuf;
    Fl_Text_Buffer*  stylebuf;

    UI_LogBox(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        //, m_spacing(spacing)
    {
        begin();


        logbuf   = new Fl_Text_Buffer();
        stylebuf = new Fl_Text_Buffer();

        logbox = new Fl_Text_Display(0,0,W,H);

        logbox->buffer(logbuf);

        // Style table: jeder char in stylebuf → Style-Index
        static const Fl_Text_Display::Style_Table_Entry g_logStyles[] = {
            { FL_BLACK, FL_COURIER, 14 },       // 'A' = info
            { FL_RED,   FL_COURIER_BOLD, 14 },  // 'B' = error
            { FL_BLUE,  FL_COURIER, 14 },       // 'C' = debug
            { FL_MAGENTA,FL_COURIER, 14 },       // 'D' = warn
            { FL_DARK_GREEN, FL_COURIER_BOLD, 14 },  // 'E' = success
        };
        logbox->highlight_data(
            stylebuf,
            g_logStyles,
            sizeof(g_logStyles)/sizeof(g_logStyles[0]),
            'A',   // Default style
            nullptr, nullptr
        );

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X,Y,W,H);

        logbox->resize(X,Y,W,H);
    }
};








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
// =============================================================
class XP_Progress : public Fl_Progress
{
public:
    XP_Progress(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_NO_BOX);  // wir zeichnen alles selbst
        color(fl_rgb_color(200, 200, 200));        // XP trough
        selection_color(fl_rgb_color(0, 180, 0));  // XP green
        labelcolor(FL_WHITE);
    }

    void draw_round_rect(int X, int Y, int W, int H, int r, Fl_Color c)
    {
        fl_color(c);
        fl_begin_polygon();

        // top-left arc
        fl_arc(X, Y, 2*r, 2*r, 90, 180);

        // left side
        fl_vertex(X, Y + r);
        fl_vertex(X, Y + H - r);

        // bottom-left arc
        fl_arc(X, Y + H - 2*r, 2*r, 2*r, 180, 270);

        // bottom side
        fl_vertex(X + r, Y + H);
        fl_vertex(X + W - r, Y + H);

        // bottom-right arc
        fl_arc(X + W - 2*r, Y + H - 2*r, 2*r, 2*r, 270, 360);

        // right side
        fl_vertex(X + W, Y + H - r);
        fl_vertex(X + W, Y + r);

        // top-right arc
        fl_arc(X + W - 2*r, Y, 2*r, 2*r, 0, 90);

        // top side
        fl_vertex(X + W - r, Y);
        fl_vertex(X + r, Y);

        fl_end_polygon();
    }

    void draw() override
    {
        fl_push_clip(x(), y(), w(), h());

        int r = 3; // XP typical corner radius

        // Trough (rounded)
        draw_round_rect(x(), y(), w(), h(), r, color());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_begin_loop();
        fl_arc(x(), y(), 2*r, 2*r, 90, 180);
        fl_arc(x(), y() + h() - 2*r, 2*r, 2*r, 180, 270);
        fl_arc(x() + w() - 2*r, y() + h() - 2*r, 2*r, 2*r, 270, 360);
        fl_arc(x() + w() - 2*r, y(), 2*r, 2*r, 0, 90);
        fl_end_loop();

        // Progress fraction
        float frac = (maximum() > minimum())
            ? (value() - minimum()) / (maximum() - minimum())
            : 0.0f;

        int pw = int(frac * w());

        if (pw > 0)
        {
            // Base XP green
            draw_round_rect(x(), y(), pw, h(), r, selection_color());

            // Glossy highlight (top half)
            draw_round_rect(x(), y(), pw, h()/2, r, fl_rgb_color(0, 220, 0));

            // XP stripes
            fl_color(fl_rgb_color(0, 140, 0));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);

        fl_pop_clip();
    }
};
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
        progress = new XP_Progress(x,y,w2,H); x += w2 + s;
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
class DynamicLayout : public Fl_Group {
public:
    DynamicLayout(int X, int Y, int W, int H)
        : Fl_Group(X, Y, W, H)
    {
        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Group::resize(X, Y, W, H);

        // Layout-Regeln
        child(0)->resize(10, 10, W - 20, 30);
        child(1)->resize(10, 50, W - 20, 30);
        child(2)->resize(10, 90, W - 20, H - 100);
    }
};
