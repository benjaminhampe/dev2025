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

// #include <WaveformWidget_fltk.h>
#include <GL_WaveformWidget_fltk.h>

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <algorithm>

#ifdef _WIN32 // only for Window ICOn
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    // #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    // #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    #include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif

#include <DarkSound.h>
#include <DarkImage.h>

// ---------------- helpers ----------------
static std::string make_mp3_name(const std::string& wav)
{
    std::filesystem::path p = std::filesystem::u8path(wav);
    p.replace_extension(".mp3");
    return p.u8string();
}

// ---------------- helpers ----------------
static void trim(std::string& s)
{
    while (!s.empty() && (s.back()=='\r' ||
                          s.back()=='\n' ||
                          s.back()=='\t' ||
                          s.back()==' '))
    {
        s.pop_back();
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
class UI_FileInputField : public Fl_Input
{
public:
    Fl_Button* btnConvert = nullptr;
    Fl_Input* edtOutput = nullptr;

    UI_FileInputField(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Input(X, Y, W, H, L) {}

    void setConvertButton(Fl_Button* btn)
    {
        btnConvert = btn;
    }

    void setOutputLineEdit(Fl_Input* edt)
    {
        edtOutput = edt;
    }

    int handle(int event) override
    {
        // FLTK schreibt beim Drop den Text selbst ins Input,
        // wir reagieren nur auf FL_PASTE.
        if (event == FL_PASTE)
        {
            // Clear
            value("");

            // Handle paste
            int r = Fl_Input::handle(event); // Text wird gesetzt

            //
            std::string uri = value();

            if (dbExistFile(uri))
            {
                btnConvert->activate();
                auto dst = make_mp3_name(uri);
                edtOutput->value(dst.c_str());
            }
            else
            {
                DE_ERROR("InputFile does not exist, ", uri)
            }
            return r;
        }
        return Fl_Input::handle(event);
    }
};
// =============================================================
class UI_FileInput : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Button* btnLoad;
    Fl_Box* lblLoad;
    UI_FileInputField* edtUri;
    Fl_Button* btnChoose;

    UI_FileInput(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 50 * zoom;
        int w2 = 60 * zoom;
        int w3 = 400 * zoom;
        int w4 = 60 * zoom;

        int x = X;
        int y = Y;
        btnLoad  = new Fl_Button(x, y, w1, H, "Load");
        x += w1 + s;
        lblLoad  = new Fl_Box(x, y, w2, H, "Inputfile:");
        btnLoad->tooltip("Loads file to preview and cut it");
        x += w2 + s;
        edtUri = new UI_FileInputField(x, y, w3, H, "");
        x += w3 + s;
        btnChoose  = new Fl_Button(x, y, w4, H, "...");

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 50 * zoom;
        int w2 = 60 * zoom;
        int w3 = 400 * zoom;
        int w4 = 60 * zoom;

        int x = X;
        int y = Y;
        btnLoad->resize(x, y, w1, H);    x += w1 + s;
        lblLoad->resize(x, y, w2, H);    x += w2 + s;
        edtUri->resize(x, y, w3, H);     x += w3 + s;
        btnChoose->resize(x, y, w4, H);
    }
};
// =============================================================
class UI_FileOutput : public Fl_Group
{
public:
    int m_spacing = 5;

    Fl_Button* btnConvert = nullptr;
    //ImageWidget* img1;
    Fl_Box* lblOutput;
    Fl_Input* edtUri;
    Fl_Button* btnChoose;

    UI_FileOutput(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 60 * zoom;
        int w2 = 60 * zoom;
        int w4 = 60 * zoom;
        int w3 = W - w1-w2-w4 - s*3;
        int x = X;
        int y = Y;

        btnConvert = new Fl_Button(x,y,w1,H, "Convert"); x += w1 + s;
        //ui.img1 = new ImageWidget(x,y,40,H);
        lblOutput = new Fl_Box(x,y,w2,H,"Outputfile:"); x += w2 + s;
        edtUri = new Fl_Input(x,y,w3,H,""); x += w3 + s;
        btnChoose = new Fl_Button(x,y,w4,H,"...");

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 60 * zoom;
        int w2 = 60 * zoom;
        int w4 = 60 * zoom;
        int w3 = W - w1-w2-w4 - s*3;
        int x = X;
        int y = Y;
        btnConvert->resize(x,y,w1,H); x += w1 + s;
        //ui.img1->resize(x,y,40,H);
        lblOutput->resize(x,y,w2,H); x += w2 + s;
        edtUri->resize(x,y,w3,H); x += w3 + s;
        btnChoose->resize(x,y,w4,H);
    }
};
// =============================================================
class UI_Waveform : public Fl_Group
{
public:
    GL_WaveformWidget* waveform = nullptr;
    Fl_HScrollBarLite* scrollBar = nullptr;
    // Fl_Button* zoomIn = nullptr;
    // Fl_Button* zoomOut = nullptr;

    void setSound(de::Sound* snd)
    {
        waveform->setSound(snd);
        redraw();
    }

    void setDarkMode(bool bDarkMode)
    {
        waveform->setDarkMode(bDarkMode);
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
        waveform = new GL_WaveformWidget(x,y,W,h1); y += h1 + 5;
        waveform->tooltip("Waveform display: scroll, zoom, select region");
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

        waveform->resize(x,y,W,h1); y += h1 + 5;
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
// ---------------- UI ----------------
struct UI
{
    UI_FileInput* inFile;
    UI_Waveform* waveform;
    UI_Resampler* resampler;
    UI_Encoder* encoder;
    UI_FileOutput* outFile;
    UI_Progress* progress;
    UI_LogBox* logbox;

    std::atomic<bool> reloadFile{true};
    std::atomic<bool> cancelFlag{false};
    std::thread worker;

    int bitrate; // bitrate in kbit, e.g. 128, not 128000
    int quality; // quality 0..9

    de::Sound soundIn;
    de::Sound soundOut;

    std::string getSrcUri() const { return inFile->edtUri->value(); }
    std::string getDstUri() const { return outFile->edtUri->value(); }
};

UI ui;

void log_common(const char* msg, char style)
{
    // Text anhängen
    ui.logbox->logbuf->append(msg);
    ui.logbox->logbuf->append("\n");

    // Style anhängen (gleiche Länge)
    int len = strlen(msg) + 1;
    std::string s(len, style);
    ui.logbox->stylebuf->append(s.c_str());

    ui.logbox->logbox->scroll(ui.logbox->logbuf->length(), 0);
}

inline void log_info(const char* msg) { log_common(msg,'A'); }
inline void log_error(const char* msg) { log_common(msg,'B'); }
inline void log_debug(const char* msg) { log_common(msg,'C'); }
inline void log_warn(const char* msg) { log_common(msg,'D'); }
inline void log_success(const char* msg) { log_common(msg,'E'); }

struct LogAsync
{
    int logLevel = de::LogLevel::Info;
    char* msg = nullptr;
    ~LogAsync() { if (msg) { delete msg; } }
};

static void log_common_awake(void* data)
{
    auto logAsync = (LogAsync*)data;
    switch(logAsync->logLevel)
    {
        case de::LogLevel::Error: log_common(logAsync->msg,'B'); break;
        case de::LogLevel::Debug: log_common(logAsync->msg,'C'); break;
        case de::LogLevel::Warn: log_common(logAsync->msg,'D'); break;
        case de::LogLevel::Ok: log_common(logAsync->msg,'E'); break;
        default: log_common(logAsync->msg,'A'); break;
    }
    delete logAsync;
}

static void async_log_common(const std::string& text, int logLevel)
{
    if (text.empty()) return;
    auto logAsync = new LogAsync;
    logAsync->logLevel = logLevel;
    logAsync->msg = new char[text.size()+1];
    std::memcpy(logAsync->msg, text.c_str(), text.size());
    logAsync->msg[text.size()] = '\0';
    Fl::awake(log_common_awake, logAsync);
}

inline void async_log_debug(const std::string& msg) { async_log_common(msg,de::LogLevel::Debug); }
inline void async_log_info(const std::string& msg) { async_log_common(msg,de::LogLevel::Info); }
inline void async_log_warn(const std::string& msg) { async_log_common(msg,de::LogLevel::Warn); }
inline void async_log_error(const std::string& msg) { async_log_common(msg,de::LogLevel::Error); }
inline void async_log_ok(const std::string& msg) { async_log_common(msg,de::LogLevel::Ok); }

static void apply_dark_theme(Fl_Group* g)
{
    for (int i = 0; i < g->children(); ++i)
    {
        Fl_Widget* w = g->child(i);
        w->color(fl_rgb_color(45,45,45)); // Hintergrund
        w->labelcolor(fl_rgb_color(230,230,230)); // Text
        w->selection_color(fl_rgb_color(80,80,160)); // Auswahl

        // Buttons etwas heller
        if (dynamic_cast<Fl_Button*>(w))
            w->color(fl_rgb_color(60,60,60));

        // Rekursiv für Gruppen
        if (auto* grp = dynamic_cast<Fl_Group*>(w))
            apply_dark_theme(grp);
    }
}

static void darkmode_cb(Fl_Widget*, void*)
{
    static bool bDarkMode = false;
    bDarkMode = !bDarkMode;

    if (bDarkMode)
    {
        Fl::background(30,30,30);
        Fl::foreground(220,220,220);
        //apply_dark_theme(Fl::first_window());
    }
    else
    {
        Fl::background(240,240,240);
        Fl::foreground(0,0,0);
    }

    ui.waveform->setDarkMode(bDarkMode);
    Fl::redraw();
}



// ---------------- callbacks ----------------
static void cancel_cb(Fl_Widget*, void*)
{
    ui.cancelFlag = true;
}

static void progress_awake(void* data)
{
    int percent = *static_cast<int*>(data);
    ui.progress->progress->value(0.01f * percent);
    delete static_cast<int*>(data);
}

static void async_progress(int pc)
{
    auto progress = new int;
    *progress = pc;
    Fl::awake(progress_awake, progress);
}

// ---------------- Fl::awake ----------------
void convert_async()
{
    ui.cancelFlag = false;
    std::string srcUri = ui.getSrcUri();
    std::string dstUri = ui.getDstUri();

    if (ui.soundIn.empty())
    {
        log_error("You must load an audio file first");
        return;
    }

    auto s1 = dbStr("Convert Start: bitrate = ",ui.bitrate,", lameQualityPreset = ",ui.quality);
    async_log_ok(s1);
    async_progress(0);

    de::SoundSaveOptions optSave;
    optSave.bCancelFlag = &ui.cancelFlag;
    optSave.bitrate = ui.bitrate;
    optSave.quality = ui.quality;
    optSave.onProgress = [](int pc) { async_progress(pc); };

    if (!dbSaveSound(ui.soundIn, dstUri, optSave))
    {
        auto s2 = dbStr("Cannot save outputFile");
        async_log_error(s2);
        return;
    }

    auto s3 = dbStr("Convert Finished: ",dstUri);
    async_log_ok(s3);

    async_progress(100);
}

void convert_cb(Fl_Widget*, void*)
{
    std::string src_uri = ui.getSrcUri();
    std::string dst_uri = ui.getDstUri();
    if (src_uri == dst_uri)
    {
        DE_ERROR("Src and Dst fileNames must differ! Abort")
        return;
    }

    ui.bitrate = ui.encoder->getBitrate();
    ui.quality  = ui.encoder->getQuality();

    ui.worker = std::thread(convert_async);
    ui.worker.detach();
}

// ---------------- Fl::awake ----------------
void load_async_finish_awake(void*)
{
    ui.waveform->setSound(&ui.soundIn);

    ui.progress->progress->value(0);
}
void load_async()
{
    ui.cancelFlag = false;

    std::string uri = ui.getSrcUri();
    async_log_ok(dbStr("Load Start: ",uri));
    async_progress(0);

    de::SoundLoadOptions optLoad;
    optLoad.bCancelFlag = &ui.cancelFlag;
    optLoad.onProgress = [](int pc) { async_progress(pc); };

    if (!dbLoadSound(ui.soundIn,uri,optLoad))
    {
        async_log_error(dbStr("Cannot load ",uri));
        return;
    }

    async_log_ok(dbStr("Load finished = ",uri));
    async_log_debug(dbStr("Sound = ",ui.soundIn.str()));

    Fl::awake(load_async_finish_awake, nullptr);
}

// ---------------- file pickers ----------------
void load_async_cb(Fl_Widget*, void*)
{
    std::string uri = ui.getSrcUri();

    if (!dbExistFile(uri))
    {
        fl_alert("You must choose an existing file first");
        return;
    }

    ui.soundIn.clear();
    ui.worker = std::thread(load_async);
    ui.worker.detach();
}

static auto g_FileFilter =
    "All Files\t*.*\n"
    "Common Audio\t*.{wav,flac,mp3,mp4,m4a,opus,ogg,vorbis,aif,aiff}\n"
    "WAV\t*.{wav,rf64,pcm,riff}\n"
    "FLAC\t*.flac\n"
    "MP3\t*.mp3\n"
    "MP4\t*.{mp4,m4a}\n"
    "OGG-Opus\t*.opus\n"
    "OGG-Vorbis\t*.{ogg,ogx,oga,ogv,vorbis}\n"
    "OGG-Speex\t*.{spx}\n"
    "AIFF\t*.{aif,aiff}\n";

// ---------------- file pickers ----------------
void pick_input_cb(Fl_Widget*, void*)
{
    Fl_Native_File_Chooser dlg;
    dlg.type(Fl_Native_File_Chooser::BROWSE_FILE);
    dlg.filter(g_FileFilter);
    dlg.filter_value(0);   // 0 = All files

    auto uri = ui.getSrcUri();
    auto dir = dbFileDir(uri);
    if (dbExistDirectory(dir))
    {
        dlg.directory(dir.c_str());
    }

    if (dlg.show() == 0)
    {
        uri = dlg.filename();
        ui.inFile->edtUri->value(uri.c_str());
        uri = make_mp3_name(uri);
        ui.outFile->edtUri->value(uri.c_str());

        // TODO: Rework when button is activated.
        ui.outFile->btnConvert->activate();
    }
}

void pick_output_cb(Fl_Widget*, void*)
{
    Fl_Native_File_Chooser dlg;
    dlg.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    dlg.filter(g_FileFilter);
    dlg.filter_value(0);   // 0 = All files

    auto uri = ui.getSrcUri();
    if (!uri.empty())
    {
        auto dir = dbFileDir(uri);
        dlg.directory(dir.c_str());
    }

    if (dlg.show() == 0)
    {
        uri = dlg.filename();
        ui.outFile->edtUri->value(uri.c_str());
    }
}

// ---------------- callbacks ----------------
void compare_async()
{
    async_progress(1);
    async_log_ok("Compare Start: ");

    std::string src_uri = ui.getSrcUri();
    std::string dst_uri = ui.getDstUri();

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Load input
    async_log_debug(dbStr("Src: ", src_uri));
    if (ui.soundIn.empty() || ui.soundIn.m_uri != src_uri)
    {
        auto s = dbStr("Compare: Load src file: ", src_uri);
        async_log_debug(s);

        if (!dbExistFile(src_uri))
        {
            auto s = dbStr("Compare Failed. No such src file. ", src_uri);
            async_log_error(s);
            return;
        }

        de::SoundLoadOptions optLoad;
        optLoad.bCancelFlag = &ui.cancelFlag;
        optLoad.onProgress = [](int pc) { async_progress(pc); };

        if (!dbLoadSound(ui.soundIn,src_uri,optLoad))
        {
            auto s = dbStr("Compare Failed. Can't load src file. ", src_uri);
            async_log_error(s);
            return;
        }

        if (ui.soundIn.empty())
        {
            auto s = dbStr("Compare Failed. Loaded src file is empty. ", src_uri);
            async_log_error(s);
            return;
        }
    }
    async_log_debug(dbStr("Src: ", ui.soundIn.str()));

    // Load output
    async_log_debug(dbStr("Dst: ", dst_uri));
    if (ui.soundOut.empty() || ui.soundOut.m_uri != dst_uri)
    {
        auto s = dbStr("Compare: Load dst file: ", dst_uri);
        if (!dbExistFile(dst_uri))
        {
            auto s = dbStr("Compare Failed. No such dst file. ", dst_uri);
            async_log_error(s);
            return;
        }

        ui.cancelFlag = false;
        de::SoundLoadOptions optLoad;
        optLoad.bCancelFlag = &ui.cancelFlag;
        optLoad.onProgress = [](int pc) { async_progress(pc); };

        if (!dbLoadSound(ui.soundOut,dst_uri,optLoad))
        {
            auto s = dbStr("Compare Failed. Can't load dst file. ", dst_uri);
            async_log_error(s);
            return;
        }

        if (ui.soundOut.empty())
        {
            auto s = dbStr("Compare Failed. Loaded dst file is empty. ", dst_uri);
            async_log_error(s);
            return;
        }
    }
    async_log_debug(dbStr("Dst: ", ui.soundOut.str()));

    // Comparing....
    async_log_info("Comparing...");

    const auto & src = ui.soundIn;
    const auto & dst = ui.soundOut;
    if (src.m_frames != dst.m_frames)
    {
        auto a = src.m_frames;
        auto b = dst.m_frames;
        if (a > b)
        {
            auto d = a-b;
            auto r = 100.0 * double(d) / double(a);
            auto s = dbStr("Differing FrameCount (src > dst), delta(",d,"), percent(",r,")");
            async_log_error(s);
        }
        else
        {
            auto d = a-b;
            auto r = 100.0 * double(d) / double(a);
            auto s = dbStr("Differing FrameCount (dst > src), delta(",d,"), percent(",r,")");
            async_log_error(s);
        }
    }

    if (src.m_channels < 1)
    {
        async_log_error("No src channels.");
        return;
    }

    if (dst.m_channels < 1)
    {
        async_log_error("No dst channels.");
        return;
    }

    if (src.m_channels != dst.m_channels)
    {
        auto s = dbStr("Differing Channels: src(",src.m_channels,") != dst(",dst.m_channels,")");
        async_log_error(s);
    }

    if (src.m_sampleRate != dst.m_sampleRate)
    {
        auto s = dbStr("Differing SampleRate: src(",src.m_sampleRate,"Hz) != dst(",dst.m_sampleRate,"Hz)");
        async_log_error(s);
    }

    uint64_t minFrames = std::min(src.m_frames,dst.m_frames);
    uint32_t minChannels = std::min(src.m_channels,dst.m_channels);

    auto srcConv = de::SampleTypeConverter::getConverter(src.m_sampleType, de::SampleType::F32);
    if (!srcConv)
    {
        auto s = dbStr("No converter for src from ",src.m_sampleType.str()," to F32.");
        async_log_error(s);
        return;
    }
    auto dstConv = de::SampleTypeConverter::getConverter(dst.m_sampleType, de::SampleType::F32);
    if (!dstConv)
    {
        auto s = dbStr("No converter for dst from ",dst.m_sampleType.str()," to F32.");
        async_log_error(s);
        return;
    }

    de::TAlignedVector<float> srcBuf(src.m_channels);
    de::TAlignedVector<float> dstBuf(src.m_channels);

    for (int32_t c = 0; c < minChannels; ++c)
    {
        // src
        double src_min = 0.0; // Minimum
        double src_max = 0.0; // Maximum
        double src_acc = 0.0; // Absolute Summe
        double src_esu = 0.0; // Energy sum
        double src_eps = 0.0; // Energy per sample
        for (int64_t i = 0; i < minFrames; ++i)
        {
            src.read_frames(srcConv, srcBuf.data(), 1, i);
            float v = srcBuf[ c ];
            src_min = std::fminf(src_min,v);
            src_max = std::fmaxf(src_max,v);
            src_acc += std::fabsf(v);
            src_esu += v * v;
        }
        src_eps = src_acc / double(src.m_frames);

        // dst
        double dst_min = 0.0; // Minimum
        double dst_max = 0.0; // Maximum
        double dst_acc = 0.0; // Absolute Summe
        double dst_esu = 0.0; // Energy sum
        double dst_eps = 0.0; // Energy per sample
        for (int64_t i = 0; i < minFrames; ++i)
        {
            dst.read_frames(dstConv, dstBuf.data(), 1, i);
            float v = dstBuf[ c ];
            dst_min = std::fminf(dst_min,v);
            dst_max = std::fmaxf(dst_max,v);
            dst_acc += std::fabsf(v);
            dst_esu += v * v;
        }
        dst_eps = dst_acc / double(dst.m_frames);

        double d_min = 100.0 * dst_min / src_min;
        double d_max = 100.0 * dst_max / src_max;
        double d_acc = 100.0 * dst_acc / src_acc;
        double d_esu = 100.0 * dst_esu / src_esu;
        double d_eps = 100.0 * dst_eps / src_eps;

        auto s1 = dbStr("Channel[",c,"].Min       src(",src_min,", dst(",dst_min,"), diff(",d_min,"%)");
        auto s2 = dbStr("Channel[",c,"].Max       src(",src_max,", dst(",dst_max,"), diff(",d_max,"%)");
        auto s3 = dbStr("Channel[",c,"].AbsAccum  src(",src_acc,", dst(",dst_acc,"), diff(",d_acc,"%)");
        auto s4 = dbStr("Channel[",c,"].E_Sum     src(",src_esu,", dst(",dst_esu,"), diff(",d_esu,"%)");
        auto s5 = dbStr("Channel[",c,"].E_PerSamp src(",src_eps,", dst(",dst_eps,"), diff(",d_eps,"%)");

        async_log_info(s1);
        async_log_info(s2);
        async_log_info(s3);
        async_log_info(s4);
        async_log_info(s5);
/*
        // delta:
        double delta_max = 0.0; // Absolute Summe
        double delta_sum = 0.0; // Energy per sample
        for (int64_t i = 0; i < minFrames; ++i)
        {
            double a = src.m_samples[i * src.m_channels + c];
            double b = dst.m_samples[i * src.m_channels + c];
            double delta = std::fabs(b - a);
            delta_max = std::fmax(delta,delta_max);
            delta_sum += delta;
        }

        auto s4 = dbStr("Channel[",c,"] delta_max(",delta_max,"), delta_sum(",delta_sum,")");
        async_log_info(s4);
*/
    }

    log_success("Finished compare.");
}


// ---------------- callbacks ----------------
void compare_cb(Fl_Widget*, void*)
{
    ui.cancelFlag = false;
    ui.worker = std::thread(compare_async);
    ui.worker.detach();
}

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

void apply_global_font(Fl_Group* g) {
    for (int i = 0; i < g->children(); ++i) {
        Fl_Widget* w = g->child(i);
        w->labelfont(FL_HELVETICA);
        w->labelsize(14);
        if (auto* sub = dynamic_cast<Fl_Group*>(w))
            apply_global_font(sub);
    }
}


class MainWindow : public Fl_Window
{
public:
    MainWindow(int W, int H, const char* title)
        : Fl_Window(W, H, title)
    {
        begin();
        //resizable(win);
        // Fl_Box* dummy = new Fl_Box(0,0,1,1);
        // win->resizable(dummy);

        const float zoom = Fl::screen_scale(0);
        const int d = 5 * zoom;
        const int h1 = 30 * zoom;
        const int h2 = 128 * zoom;
        const int h3 = H - 5*(h1+d) - (h2+d) - 2*d;
        int x = d;
        int y = d;

        ui.inFile = new UI_FileInput(x,y,W-2*d,h1,d); y += h1 + d;
        ui.waveform = new UI_Waveform(x,y,W-2*d,h2); y += h2 + d;
        ui.outFile = new UI_FileOutput(x,y,W-2*d,h1,d); y += h1 + d;
        ui.progress = new UI_Progress(x,y,W-2*d,h1,d); y += h1 + d;
        ui.encoder = new UI_Encoder(x,y,W-2*d,h1,d); y += h1 + d;
        ui.resampler = new UI_Resampler(x,y,W-2*d,h1,d); y += h1 + d;
        ui.logbox = new UI_LogBox(x,y,W-2*d,h3,d);

        // Connect
        ui.inFile->btnLoad->callback(load_async_cb);
        ui.inFile->btnChoose->callback(pick_input_cb);
        ui.inFile->edtUri->setOutputLineEdit(ui.outFile->edtUri);
        ui.inFile->edtUri->setConvertButton(ui.outFile->btnConvert);

        // Connect
        ui.outFile->btnConvert->callback(convert_cb);
        ui.outFile->btnConvert->deactivate();
        ui.outFile->btnChoose->callback(pick_output_cb);

        // Connect
        ui.progress->btnCompare->callback(compare_cb);
        ui.progress->btnCancel->callback(cancel_cb);
        ui.progress->btnDarkMode->callback(darkmode_cb);

        ui.waveform->waveform->onZoom =
            [&](int64_t zoomBeg, int64_t zoomEnd, int64_t frameCount)
            {
                double t1 = double(zoomBeg) / double(frameCount);
                double t2 = double(zoomEnd - zoomBeg) / double(frameCount);
                ui.waveform->scrollBar->set_slider_pos(t1);
                ui.waveform->scrollBar->set_slider_size(std::max(0.001,t2));
                //ui.waveform->scrollBar->value(t1);
                //ui.waveform->scrollBar->slider_size(std::max(0.001,t2));
            };

        //ui.waveform->scrollBar->callback(hscroll_cb,nullptr);

        ui.waveform->scrollBar->onChange = [&] (double value)
        {
            // auto scrollBar = (Fl_Scrollbar*)w;

            // auto scrollBar = ui.waveform->scrollBar;

            // ui.waveform->setZoomStart( scrollBar->value() );

            ui.waveform->waveform->setZoomFromScrollBar( value );

        };

        //win->resizable(win);   // oder ein Child-Widget
        //win->resizable(nullptr);
        end();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);

        const float zoom = Fl::screen_scale(0);
        const int d = 5 * zoom;
        const int h1 = 30 * zoom;
        const int h2 = 128 * zoom;
        const int h3 = H - 5*(h1+d) - (h2+d) - 2*d;
        int x = d;
        int y = d;

        ui.inFile->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.waveform->resize(x,y,W-2*d,h2); y += h2 + d;
        ui.outFile->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.progress->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.encoder->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.resampler->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.logbox->resize(x,y,W-2*d,h3);
    }
};

static int global_handler(int event)
{
    if (event == FL_MOUSEWHEEL) {
        Fl_Window* win = Fl::first_window();
        if (!win)
        {
            DE_WARN("No first window")
            return 0;
        }

        int mx = Fl::event_x_root();
        int my = Fl::event_y_root();

        int x = win->x();
        int y = win->y();
        int w = win->w();
        int h = win->h();


        // Titlebar height (approx; OS-dependent)
        int title_h = 30; // you can refine this per OS

        bool over_title = dbMouseOver(mx,my,x,y,x+w-1,y+title_h-1);

        if (over_title)
        {
            float zoom = Fl::screen_scale(0);

            if (Fl::event_dy() > 0) // Zoom out
            {
                zoom = std::clamp<double>(zoom - 0.10, 0.5, 2.5);
            }
            else if (Fl::event_dy() < 0) // Zoom In
            {
                zoom = std::clamp<double>(zoom + 0.10, 0.5, 2.5);
            }

            Fl::screen_scale(0,zoom);

            DE_WARN("Zoom(",zoom,"), Mouse(",mx,",",my,"), Window(",x,",",y,",",w,",",h,")")
            return 1; // swallow event
        }
    }
    return 0;
}

// =============================================================
int main(int argc, char** argv)
// =============================================================
{
    DE_DEBUG("Fl::screen_scaling_supported() = ",Fl::screen_scaling_supported())
    DE_DEBUG("Fl::screen_scale(0) = ",Fl::screen_scale(0))
    DE_DEBUG("Fl::use_high_res_GL() = ",Fl::use_high_res_GL())

    Fl::use_high_res_GL(0);
    Fl::screen_scale(0, 1.5f);
    Fl::visual(FL_RGB);
    //Fl::set_font(FL_HELVETICA, "DejaVu Sans");
    Fl::set_font(FL_HELVETICA, "Noto Sans");
    // Fl::set_font(FL_HELVETICA, "Noto Emoji");
    // Fl::set_font(FL_FREE_FONT, "Noto Emoji");
    // my_widget->labelfont(FL_FREE_FONT);
    // my_widget->labelsize(20);
    // my_widget->label("🔥 Feuer!");

    //Fl::scheme("gtk+");
    //Fl::scheme("plastic");
    //Fl::scheme("gleam");
    //Fl::scheme("oxy");
    Fl::scheme("none");

    Fl::add_handler(global_handler); // Zoom +/- on TitleBar MouseWheel Scrolling

    const int w = 600;
    const int h = 600;
    auto win = new MainWindow(w, h, "AudioConverter | benjaminhampe@gmx.de | MP4 MP3 FLAC WAV OGG OPUS");
    win->resizable(win);

    #ifdef _WIN32
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(aaaa));
    win->icon((char*)hIcon);
    #else
    // win->icon((char*)LoadIcon(NULL, IDI_APPLICATION));
    #endif

    win->show(argc, argv);

    // log_debug("Test: log_debug()");
    // log_error("Test: log_error()");
    // log_info("Test: log_info()");
    // log_warn("Test: log_warn()");
    // log_success("Test: log_success()");

// #ifdef _WIN32
// SetProcessDPIAware(); // echte Pixel, keine Skalierung
// #endif

// #ifdef _WIN32
// SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
// #endif

    return Fl::run();
}
