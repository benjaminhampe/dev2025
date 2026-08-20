#pragma once
#include <de/win32/win32_Load_Shell_Icon.h>
#include <de/archive/FileNames.h>

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
#include <FL/Fl_Hold_Browser.H>

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <algorithm>

#include <DarkImage.h>

// #include <windows.h>
// #include <shellapi.h>
#include <FL/Fl_RGB_Image.H>

class DropList : public Fl_Browser
{
public:
    struct Row {
        std::string uri;
        std::shared_ptr<de::Image> ico;   // RAII-safe shared ownership
        float progress = 0.0f;
        int typ = 0;
    };

    std::vector<Row> rows;

    FileNamesA getFileNamesA() const
    {
        FileNamesA fileNames;
        fileNames.reserve(rows.size());

        for (const auto& row : rows)
        {
            fileNames.push_back(row.uri);
        }
        return fileNames;
    }

    typedef void(*FN_onListChange)(void);

    std::vector<FN_onListChange> onListChangeListeners;

    // Drag state
    int drag_index = -1;
    int hover_index = -1;
    bool dragging = false;

    const int scroll_zone = 20;
    const int scroll_speed = 4;

    DropList(int X, int Y, int W, int H, const char* L = nullptr)
        : Fl_Browser(X, Y, W, H, L) {
        type(FL_MULTI_BROWSER);
    }

    // Add row
    void addRow(std::string uri)
    {
        if (uri.size() > 4096)
        {
            DE_DEBUG("Uri too big")
            return;
        }

        uri = make_posix_path(uri);

        if (uri.empty())
        {
            DE_DEBUG("Empty")
            return;
        }

        // Only add unique paths:
        auto found = std::find_if( rows.begin(), rows.end(),
                [&] (const Row& cached)
                {
                #ifdef _WIN32
                return de::StringUtil::makeLower(cached.uri)
                    == de::StringUtil::makeLower(uri);
                #else
                    return cached.uri == uri;
                #endif
                });
        if (found != rows.end())
        {
            DE_DEBUG("Already added: ", uri)
            return;
        }

        bool bFile = dbExistFile(uri);
        bool bDir = dbExistDirectory(uri);
        if (!bFile && !bDir)
        {
            DE_DEBUG("Not a dir or file: ", uri)
            return;
        }

        int ico_w = item_height(nullptr) - 4;

        Row row;
        row.uri = uri;
        row.ico = load_shell_icon(uri, ico_w, ico_w);
        row.progress = 0.0f;
        row.typ = (bFile) ? 0 : 5;
        rows.emplace_back( std::move(row) );

        sortRows();

        add(uri.c_str());

        DE_OK("Added: ",uri)

        emit_onListChanged();
    }

    void emit_onListChanged()
    {
        for (auto & listener : onListChangeListeners)
        {
            listener();
        }
    }

    // Update progress
    void set_progress(int row, float p) {
        rows[row - 1].progress = std::clamp(p, 0.0f, 1.0f);
        redraw();
    }

    // -----------------------------
    // Custom row height
    // -----------------------------
    int item_height(void*) const override
    {
        //return textsize() + 8;
        return 24 * Fl::screen_scale(0);
    }

    void sortRows()
    {
        std::sort(rows.begin(), rows.end(),
            [](const Row& a, const Row& b)
            {
                if (a.typ != b.typ)
                    return a.typ > b.typ;
#ifdef _WIN32
                return de::StringUtil::makeLower(a.uri)
                     < de::StringUtil::makeLower(b.uri);
#else
                return a.uri < b.uri;
#endif
            });
    }

    void draw() override
    {
        Fl_Browser::draw();

        if (rows.empty())
        {
            fl_color(FL_BLACK);

            int x = this->x();
            int y = this->y();
            int w = this->w();
            int h = this->h();
            auto s = "Drop files and folders here...";
            fl_draw(s, x + 20, y + h/2 + textsize()/2);
        }

    }

    // -----------------------------
    // Custom row drawing
    // -----------------------------
    void item_draw(void* v, int X, int Y, int W, int H) const override
    {
        int index = lineno(v);        // <-- FIXED
        // DE_DEBUG("index = ",index)
        const Row& r = rows.at(index - 1);

        fl_color(selected(index) ? fl_rgb_color(180,200,255) : FL_WHITE);
        fl_rectf(X, Y, W, H);

        int x = X;
        int y = Y;

        if (r.ico)
        {
            Fl_RGB_Image fl_img(r.ico->data(),
                                r.ico->w(),
                                r.ico->h(),
                                r.ico->bytesPerPixel());
            fl_img.draw(x + 2, y + 2);
            x += r.ico->w() + 4;
        }

        fl_color(FL_BLACK);
        fl_draw(r.uri.c_str(), x, y + H/2 + textsize()/2);

        if (r.progress > 0.0f)
        {
            int bar_x = X + 32;
            int bar_y = Y + H - 6;
            int bar_w = W - 40;
            int bar_h = 4;

            fl_color(fl_rgb_color(200,200,200));
            fl_rectf(bar_x, bar_y, bar_w, bar_h);

            fl_color(fl_rgb_color(0,160,0));
            fl_rectf(bar_x, bar_y, int(bar_w * r.progress), bar_h);

            fl_color(fl_rgb_color(80,80,80));
            fl_rect(bar_x, bar_y, bar_w, bar_h);
        }
    }

    // -----------------------------
    // FULL EVENT HANDLING
    // -----------------------------
    int handle(int e) override {
        switch (e) {

        // -----------------------------
        // Drag & Drop (file drop)
        // -----------------------------
        case FL_DND_ENTER:
        case FL_DND_DRAG:
        case FL_DND_RELEASE:
            return 1;

        case FL_PASTE:
        {
            const char* t = Fl::event_text();
            if (t)
            {
                std::string s(t);
                std::stringstream ss(s);
                std::string line;

                while (std::getline(ss, line))
                {
                    addRow(line);
                }
            }
            return 1;
        }

        // -----------------------------
        // Keyboard → delete selected rows
        // -----------------------------
        case FL_KEYDOWN: {
            if (Fl::event_key() == FL_Delete)
            {
                bool bRemovedSomething = false;
                for (int i = size(); i >= 1; --i)
                {
                    if (selected(i))
                    {
                        rows.erase(rows.begin() + (i - 1));
                        remove(i);

                        bRemovedSomething = true;
                    }
                }

                if (bRemovedSomething)
                {
                    emit_onListChanged();
                }
                return 1;
            }
            break;
        }
/*
        // -----------------------------
        // Mouse push → begin drag
        // -----------------------------
        case FL_PUSH: {
            //int idx = find_line(Fl::event_y());
            int idx = lineno(find_line(Fl::event_y()));
            if (idx >= 1) {
                drag_index = idx;
                dragging = true;
            }
            return 1;
        }

        // -----------------------------
        // Mouse drag → reorder + auto-scroll
        // -----------------------------
        case FL_DRAG: {
            if (dragging && drag_index >= 1) {
                //hover_index = find_line(Fl::event_y());
                hover_index = lineno(find_line(Fl::event_y()));

                if (hover_index >= 1)
                    select(hover_index, 1);

                int my = Fl::event_y();

                // Auto-scroll up
                if (my < y() + scroll_zone)
                    position(position() - scroll_speed);

                // Auto-scroll down
                if (my > y() + h() - scroll_zone)
                    position(position() + scroll_speed);

                return 1;
            }
            break;
        }

        // -----------------------------
        // Mouse release → finalize reorder
        // -----------------------------
        case FL_RELEASE: {
            if (dragging && drag_index >= 1) {
                //int index = lineno(v);   // 1-based index
                //int drop_index = find_line(Fl::event_y());
                int drop_index = lineno(find_line(Fl::event_y()));


                if (drop_index >= 1 && drop_index != drag_index) {
                    Row r = rows[drag_index - 1];

                    rows.erase(rows.begin() + (drag_index - 1));
                    rows.insert(rows.begin() + (drop_index - 1), r);

                    // Rebuild browser text list
                    clear();
                    for (auto& row : rows)
                        add(row.uri.c_str());
                }
            }

            dragging = false;
            drag_index = -1;
            hover_index = -1;
            return 1;
        }
*/

        default: break;
        }

        return Fl_Browser::handle(e);
    }


};

/*
class DropList : public Fl_Hold_Browser
{
public:
    DropList(int X, int Y, int W, int H, const char *L = 0)
        : Fl_Hold_Browser(X, Y, W, H, L)
    {
        type(FL_MULTI_BROWSER); // allow multi-selection
    }

    void add_dropped_path(const std::string& path)
    {
        Fl_Image* icon = nullptr;

    #ifdef _WIN32
        icon = load_shell_icon(path);
    #elif __APPLE__
        icon = load_nsimage_icon(path);
    #else
        icon = is_directory(path)
            ? load_xdg_icon("folder")
            : load_xdg_icon("text-x-generic");
    #endif

        add(icon, path.c_str());
    }

    int handle(int e) override
    {
        switch (e)
        {
            case FL_DND_ENTER:
            case FL_DND_DRAG:
            case FL_DND_RELEASE:
                return 1;

            case FL_KEYDOWN:
            {
                int key = Fl::event_key();
                if (key == FL_Delete)
                {
                    for (int i = size(); i >= 1; --i)
                        if (selected(i))
                            remove(i);
                    return 1;
                }
                break;
            }

            case FL_PASTE:
            {
                const char *t = Fl::event_text();
                if (!t) return 1;

                // FLTK gives dropped paths separated by newlines
                std::string s(t);
                size_t pos = 0;
                while ((pos = s.find('\n')) != std::string::npos)
                {
                    std::string path = s.substr(0, pos);
                    if (!path.empty())
                        // add(path.c_str());
                        add(load_shell_icon(path), path.c_str());
                    s.erase(0, pos + 1);
                }
                if (!s.empty())
                    add(s.c_str());

                return 1;
            }
        }
        return Fl_Hold_Browser::handle(e);
    }
};

int main() {
    Fl_Window win(400, 300, "Drop List");
    DropList list(10, 10, 380, 280);
    win.end();
    win.show();
    return Fl::run();
}
*/

// =============================================================
inline void trim(std::string& s)
{
    while (!s.empty() && (s.back()=='\r' ||
                          s.back()=='\n' ||
                          s.back()=='\t' ||
                          s.back()==' '))
    {
        s.pop_back();
    }
}
// =============================================================
inline void apply_global_font(Fl_Group* g)
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
// =============================================================
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
// =============================================================
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

    //ui.waveform->setDarkMode(bDarkMode);
    Fl::redraw();
}


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
*/
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
