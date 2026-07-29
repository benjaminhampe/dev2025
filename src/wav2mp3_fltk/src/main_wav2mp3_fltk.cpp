#include "FL/fl_draw.H"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

#include <WaveformWidget_fltk.h>
#include <GL_WaveformWidget_fltk.h>

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>

#ifdef _WIN32 // only for Window ICOn
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>            // only for Window ICOn
    #include "../res/resource.h"    // only for Window ICOn
#endif

#include <DarkSound.h>
#include <DarkImage.h>

// =============================================================
class ImageWidget : public Fl_Widget
// =============================================================
{
    de::Image m_img;
public:
    ImageWidget(int X,int Y,int W,int H)
        : Fl_Widget(X,Y,W,H)
    {
        renderImage(W,H);
    }

    void renderImage(int w, int h)
    {
        if (w < 1 || h < 1)
            return;
        m_img = de::Image(w,h);
        m_img.fill(dbRGBA(55,55,0));

        int d = std::max(1, std::min(w,h) - 2);
        int x = (w - d)/2;
        int y = (h - d)/2;
        de::Recti pos(x,y,d,d);
        de::ImagePainter::drawCircle(m_img,pos,dbRGBA(255,0,0));
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

// ---------------- UI ----------------
struct UI {
    Fl_Button* btnLoad;
    Fl_Input* inFile;
    Fl_Button* btnIn;
    GL_WaveformWidget* inWavf;
    Fl_Button* zoomIn;
    Fl_Button* zoomOut;

    ImageWidget* img1;
    Fl_Input* outFile;
    Fl_Button* btnOut;
    Fl_Choice* cbxBitrate;
    Fl_Choice* quality;
    Fl_Button* convert;
    Fl_Button* cancel;
    Fl_Button* darkToggle;
    Fl_Progress* progress;

    // Fl_Input* inFile;
    Fl_Button* btnCompare;

    Fl_Text_Display* logbox;
    Fl_Text_Buffer*  logbuf;
    Fl_Text_Buffer*  stylebuf;

    std::atomic<bool> reloadFile{true};
    std::atomic<bool> cancelFlag{false};
    std::thread worker;

    std::string in;
    std::string out;
    int bitrate; // bitrate
    int q;  // quality

    de::Sound soundIn;
    de::Sound soundOut;
};

UI ui;

void log_common(const char* msg, char style)
{
    // Text anhängen
    ui.logbuf->append(msg);
    ui.logbuf->append("\n");

    // Style anhängen (gleiche Länge)
    int len = strlen(msg) + 1;
    std::string s(len, style);
    ui.stylebuf->append(s.c_str());

    ui.logbox->scroll(ui.logbuf->length(), 0);
}

void log_info(const char* msg) { log_common(msg,'A'); }
void log_error(const char* msg) { log_common(msg,'B'); }
void log_debug(const char* msg) { log_common(msg,'C'); }
void log_warn(const char* msg) { log_common(msg,'D'); }
void log_success(const char* msg) { log_common(msg,'E'); }

// ---------------- Fl::awake ----------------
/*
void log_success_awake(void* data)
{
    auto msg = static_cast<const char*>(data);
    log_success(msg);
    free(data);
}
*/

void apply_dark_theme(Fl_Group* g)
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

void darkmode_cb(Fl_Widget*, void*)
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

    Fl::redraw();
}

// ---------------- helpers ----------------
std::string make_mp3_name(const std::string& wav)
{
    std::filesystem::path p = std::filesystem::u8path(wav);
    p.replace_extension(".mp3");
    return p.u8string();
}

void trim(std::string& s)
{
    while (!s.empty() && (s.back()=='\r' || s.back()=='\n' || s.back()==' ' || s.back()=='\t'))
        s.pop_back();
}


// ---------------- Fl::awake ----------------
void convert_start_awake(void* data)
{
    log_success("Start conversion:");

    auto s = dbStr("bitrate = ",ui.bitrate, ", lameQualityPreset = ", ui.q);

    log_debug(s.c_str());

    log_debug("Import inputFile...");

    ui.progress->value(0);
}
// ---------------- Fl::awake ----------------
void convert_resample_awake(void* data)
{
    log_debug("Resample inputFile...");
}
// ---------------- Fl::awake ----------------
void convert_save_awake(void* data)
{
    log_debug("Export outputFile...");
}
// ---------------- Fl::awake ----------------
void convert_progress_awake(void* data)
{
    int percent = *static_cast<int*>(data);
    ui.progress->value(0.01f * percent);
    delete static_cast<int*>(data);
}
// ---------------- Fl::awake ----------------
void convert_finished_awake(void*)
{
    log_success("Finished conversion.");
}

void convert_async()
{
    std::string srcUri = ui.in;
    std::string dstUri = ui.out;

    if (ui.soundIn.empty())
    {
        DE_ERROR("No file loaded to export")
        // de::SoundLoadOptions optLoad;
        // optLoad.bCancelFlag = &ui.cancelFlag;
        // optLoad.onProgress = [](int percent)
        //     {
        //         auto progress = new int;
        //         *progress = percent;
        //         Fl::awake(convert_progress_awake, progress);
        //     };

        // if (!dbLoadSound(ui.soundIn,srcUri,optLoad))
        // {
        //     auto s = dbStr("Cannot Load ",srcUri);
        //     log_error(s.c_str());
        //     return;
        // }
        return;
    }

    Fl::awake(convert_start_awake, nullptr);

    ui.cancelFlag = false;

    // New Async
    std::this_thread::sleep_for(std::chrono::milliseconds(10));


    Fl::awake(convert_start_awake, nullptr);

    de::SoundSaveOptions optSave;
    optSave.bCancelFlag = &ui.cancelFlag;
    optSave.bitrate = ui.bitrate;
    optSave.quality = ui.q;
    optSave.onProgress = [](int percent)
            {
                auto progress = new int;
                *progress = percent;
                Fl::awake(convert_progress_awake, progress);
            };

    if (!dbSaveSound(ui.soundIn, ui.out, optSave))
    {
        auto s = dbStr("Cannot save outputFile");
        log_error(s.c_str());
        return;
    }

    // New Async
    Fl::awake(convert_finished_awake, nullptr);

    return;
}


// ---------------- callbacks ----------------
void cancel_cb(Fl_Widget*, void*)
{
    ui.cancelFlag = true;
}

// ---------------- callbacks ----------------
void convert_cb(Fl_Widget*, void*)
{
    ui.in = ui.inFile->value();
    ui.out = ui.outFile->value();

    int bitrate_map[] = {96,128,160,192,224,256,320};
    int quality_map[] = {0,1,5,7,9};

    ui.bitrate = bitrate_map[ui.cbxBitrate->value()];
    ui.q  = quality_map[ui.quality->value()];

    ui.worker = std::thread(convert_async);
    ui.worker.detach();
}

// ---------------- Fl::awake ----------------
void load_async_start_awake(void* data)
{
    auto s = dbStr("LoadAsync: Start... uri = ",ui.in);
    log_success(s.c_str());

    ui.progress->value(0);
}
void load_async_progress_awake(void* data)
{
    int percent = *static_cast<int*>(data);
    ui.progress->value(0.01f * percent);
    delete static_cast<int*>(data);
}
void load_async_finish_awake(void*)
{
    auto s = dbStr("LoadAsync: Finished. uri = ",ui.in, ", sound ",ui.soundIn.str());
    log_success(s.c_str());

    ui.inWavf->setSound(&ui.soundIn);
}

void load_async()
{
    std::string uri = ui.in;

    Fl::awake(load_async_start_awake, nullptr);

    ui.cancelFlag = false;

    // New Async
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    de::SoundSaveOptions optLoad;
    optLoad.bCancelFlag = &ui.cancelFlag;
    optLoad.onProgress = [](int percent)
            {
                auto progress = new int;
                *progress = percent;
                Fl::awake(load_async_progress_awake, progress);
            };

    if (!dbLoadSound(ui.soundIn,uri))
    {
        auto s = dbStr("Cannot load ",uri);
        log_error(s.c_str());
        return;
    }

    Fl::awake(load_async_finish_awake, nullptr);

    return;
}

// ---------------- file pickers ----------------
void load_async_cb(Fl_Widget*, void*)
{
    ui.in = ui.inFile->value();

    if (!dbExistFile(ui.in))
    {
        fl_alert("You must choose an existing file first");
        return;
    }

    ui.worker = std::thread(load_async);
    ui.worker.detach();
}

// ---------------- file pickers ----------------
void pick_input_cb(Fl_Widget*, void*)
{
    Fl_Native_File_Chooser dlg;
    dlg.type(Fl_Native_File_Chooser::BROWSE_FILE);
    dlg.filter("All files\t*.*\n"
               "Audio Files\t*.{wav,flac,mp3,mp4,m4a,opus,ogg,vorbis,aif,aiff}\n"
               "WAV\t*.wav\n"
               "FLAC\t*.flac\n"
               "MP3\t*.mp3\n"
               // "M4A\t*.m4a\n"
               "MP4\t*.{mp4,m4a}\n"
               // "MP4\t*.mp4\n"
               // "MP4\t*.m4a\n"
               "OPUS\t*.opus\n"
               "OGG-Vorbis\t*.{ogg,vorbis}\n"
               // "OGG-Vorbis\t*.ogg\n"
               // "OGG-Vorbis\t*.vorbis\n"
                "AIFF\t*.{aif,aiff}\n"
               );
    dlg.filter_value(0);   // 0 = All files
    if (dlg.show() == 0) {
        ui.in = dlg.filename();
        ui.inFile->value(ui.in.c_str());
        ui.convert->activate();

        ui.out = make_mp3_name(ui.in);
        ui.outFile->value(ui.out.c_str());
    }
}

void pick_output_cb(Fl_Widget*, void*)
{
    Fl_Native_File_Chooser fc;
    fc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    fc.filter("MP3\t*.mp3");

    if (!ui.in.empty()) {
        std::filesystem::path p(ui.in);
        fc.directory(p.parent_path().string().c_str());   // ⭐ funktioniert
    }

    if (fc.show() == 0) {
        ui.out = fc.filename();
        ui.outFile->value(ui.out.c_str());
    }
}

// ---------------- callbacks ----------------
void compare_cb(Fl_Widget*, void*)
{
    log_success("Start compare...");

    de::Sound src;
    if (!dbLoadSound(src,ui.in))
    {
        log_error("Cannot load src file");
        log_error("Abort compare...");
        return;
    }

    de::Sound dst;
    if (!dbLoadSound(dst,ui.out))
    {
        log_error("Cannot load dst file");
        log_error("Abort compare...");
        return;
    }

    log_info("Files loaded.");

    if (src.m_frames != dst.m_frames)
    {
        auto a = src.m_frames;
        auto b = dst.m_frames;
        // auto s = dbStr("Diff found: src.m_frames(",a,") != dst.m_frames(",b,")");
        // log_error(s.c_str());

        if (a > b)
        {
            auto s = dbStr("Diff src > dst frames(",a-b,")");
            log_error(s.c_str());
        }
        else
        {
            auto s = dbStr("Diff dst > src frames(",b-a,")");
            log_error(s.c_str());
        }
    }

    if (src.m_channels != dst.m_channels)
    {
        auto s = dbStr("Diff found: src.m_channels(",src.m_channels,") != dst.m_channels(",dst.m_channels,")");
        log_error(s.c_str());
    }

    if (src.m_sampleRate != dst.m_sampleRate)
    {
        auto s = dbStr("Diff found: src.m_sampleRate(",src.m_sampleRate,") != dst.m_sampleRate(",dst.m_sampleRate,")");
        log_error(s.c_str());
    }

    uint64_t minFrames = std::min(src.m_frames,dst.m_frames);
    uint32_t minChannels = std::min(src.m_channels,dst.m_channels);
    for (uint32_t c = 0; c < minChannels; ++c)
    {
        // src
        double src_s = 0.0; // Vorzeichenbehaftete Summe
        double src_a = 0.0; // Absolute Summe
        double src_e = 0.0; // Energy per sample
        for (uint64_t i = 0; i < minFrames; ++i)
        {
            float sample = src.m_samples[i * src.m_channels + c];
            src_s += sample;
            src_a += std::fabsf(sample);
            src_e += sample * sample;
        }
        src_a /= double(src.m_frames);
        src_e /= double(src.m_frames);

        // dst
        double dst_s = 0.0; // Vorzeichenbehaftete Summe
        double dst_a = 0.0; // Absolute Summe
        double dst_e = 0.0; // Energy
        for (uint64_t i = 0; i < minFrames; ++i)
        {
            float sample = dst.m_samples[i * dst.m_channels + c];
            dst_s += sample;
            dst_a += std::fabsf(sample);
            dst_e += sample * sample;
        }
        dst_a /= double(dst.m_frames);
        dst_e /= double(dst.m_frames);

        double ds = 100.0 * dst_s / src_s;
        double da = 100.0 * dst_a / src_a;
        double de = 100.0 * dst_e / src_e;

        auto s1 = dbStr("Channel[",c,"] src_s(",src_s,", dst_s(",dst_s,"), diff(",ds,"%)");
        auto s2 = dbStr("Channel[",c,"] src_a(",src_a,", dst_a(",dst_a,"), diff(",da,"%)");
        auto s3 = dbStr("Channel[",c,"] src_e(",src_e,", dst_e(",dst_e,"), diff(",de,"%)");

        log_info(s1.c_str());
        log_info(s2.c_str());
        log_info(s3.c_str());

        // delta:
        double delta_max = 0.0; // Absolute Summe
        double delta_sum = 0.0; // Energy per sample
        for (uint64_t i = 0; i < minFrames; ++i)
        {
            double a = src.m_samples[i * src.m_channels + c];
            double b = dst.m_samples[i * src.m_channels + c];
            double delta = std::fabs(b - a);
            delta_max = std::fmax(delta,delta_max);
            delta_sum += delta;
        }

        auto s4 = dbStr("Channel[",c,"] delta_max(",delta_max,"), delta_sum(",delta_sum,")");
        log_info(s4.c_str());
    }

    log_success("Finished compare.");
}


// ---------------- Input field mit DnD ----------------
class InputField : public Fl_Input
{
public:
    InputField(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Input(X, Y, W, H, L) {}

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
            std::string path = value();
            //trim(path);

            ui.in = path;

            if (dbExistFile(path))
            {
                ui.convert->activate();
                ui.out = make_mp3_name(path);
                ui.outFile->value(ui.out.c_str());
            }
            else
            {
                DE_ERROR("InputFile does not exist, ", path)
            }


            return r;
        }
        return Fl_Input::handle(event);
    }
};
/*
class XP_Progress : public Fl_Progress {
public:
    XP_Progress(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(200, 200, 200));   // trough background
        selection_color(fl_rgb_color(0, 120, 215)); // XP blue
        labelcolor(FL_WHITE);
    }

    void draw() override
    {
        // Draw trough background
        fl_push_clip(x(), y(), w(), h());
        fl_color(color());
        fl_rectf(x(), y(), w(), h());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_rect(x(), y(), w(), h());

        // Progress fraction
        float frac = 0.0f;
        if (maximum() > minimum())
            frac = (value() - minimum()) / (maximum() - minimum());

        int pw = int(frac * w());

        if (pw > 0)
        {
            // Base XP blue
            fl_color(selection_color());
            fl_rectf(x(), y(), pw, h());

            // Glossy highlight (top half)
            fl_color(fl_rgb_color(0, 180, 255));
            fl_rectf(x(), y(), pw, h() / 2);

            // XP stripes
            fl_color(fl_rgb_color(0, 100, 200));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Draw label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);

        fl_pop_clip();
    }
};

class XP_Progress : public Fl_Progress {
public:
    XP_Progress(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(200, 200, 200));   // trough background
        selection_color(fl_rgb_color(0, 120, 215)); // XP blue-ish
        labelcolor(FL_WHITE);
    }

    void draw() override
    {
        // Draw trough
        fl_color(color());
        fl_rectf(x(), y(), w(), h());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_rect(x(), y(), w(), h());

        // Progress width
        float frac = (maximum() > minimum())
            ? (value() - minimum()) / (maximum() - minimum())
            : 0.0f;

        int pw = int(frac * w());

        if (pw > 0)
        {
            // XP blue base
            fl_color(selection_color());
            fl_rectf(x(), y(), pw, h());

            // Glossy highlight (top half)
            fl_color(fl_rgb_color(0, 180, 255));
            fl_rectf(x(), y(), pw, h() / 2);

            // XP stripes
            fl_color(fl_rgb_color(0, 100, 200));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Draw label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);
    }
};

class XP_Green_Progress : public Fl_Progress {
public:
    XP_Green_Progress(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(200, 200, 200));        // trough background
        selection_color(fl_rgb_color(0, 180, 0));  // XP green base
        labelcolor(FL_WHITE);
    }

    void draw() override
    {
        fl_push_clip(x(), y(), w(), h());

        // Draw trough
        fl_color(color());
        fl_rectf(x(), y(), w(), h());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_rect(x(), y(), w(), h());

        // Progress fraction
        float frac = 0.0f;
        if (maximum() > minimum())
            frac = (value() - minimum()) / (maximum() - minimum());

        int pw = int(frac * w());

        if (pw > 0)
        {
            // Base XP green
            fl_color(selection_color());
            fl_rectf(x(), y(), pw, h());

            // Glossy highlight (top half)
            fl_color(fl_rgb_color(0, 220, 0));
            fl_rectf(x(), y(), pw, h() / 2);

            // XP stripes (dark green)
            fl_color(fl_rgb_color(0, 140, 0));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Draw label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);

        fl_pop_clip();
    }
};
*/
class XP_Progress : public Fl_Progress {
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
int main(int argc, char** argv)
// =============================================================
{
    Fl::use_high_res_GL(1);
    //Fl::scheme("gtk+");
    //Fl::scheme("plastic");
    //Fl::scheme("gleam");
    //Fl::scheme("oxy");
    Fl::scheme("none");

    auto win = new Fl_Window(600, 600, "WAV to MP3 | benjaminhampe@gmx.de | fltk-1.4.5 + lame-3.100 + dr.wav + dr.mp3");
    win->begin();

    int d = 10;
    int y = d;
    int b = 30;
    int k = 0;
    ui.btnLoad  = new Fl_Button(10, y, 50, b, "Load");
    ui.btnLoad->tooltip("Loads file to preview and cut it");
    ui.btnLoad->callback(load_async_cb);
    ui.inFile = new InputField(110, y, 400, b, "Inputfile:");
    ui.btnIn  = new Fl_Button(520, y, 60, b, "...");
    ui.btnIn->callback(pick_input_cb);
    y += b + d;

    int c = 128;
    ui.inWavf = new GL_WaveformWidget(10, y, 530, c);
    ui.inWavf->tooltip("Waveform display: scroll, zoom, select region");
    ui.zoomIn  = new Fl_Button(550, y,      40, (c-d)/2, "+");
    ui.zoomIn->tooltip("Zoom in");
    ui.zoomIn->callback([](Fl_Widget*, void* ud)
    {
        ((WaveformWidget*)ud)->setZoom(((WaveformWidget*)ud)->getZoom() * 1.2f);
    }, ui.inWavf);
    ui.zoomOut = new Fl_Button(550, y + c/2, 40, (c-d)/2, "-");
    ui.zoomOut->tooltip("Zoom out");
    ui.zoomOut->callback([](Fl_Widget*, void* ud)
    {
        ((WaveformWidget*)ud)->setZoom(((WaveformWidget*)ud)->getZoom() * 0.8f);
    }, ui.inWavf);
    y += c + d;

    ui.img1 = new ImageWidget(10, y, 40, b);
    ui.outFile = new Fl_Input(110, y, 400, b, "Outputfile:");
    ui.btnOut  = new Fl_Button(520, y, 60, b, "...");
    ui.btnOut->callback(pick_output_cb);
    y += b + d;

    ui.cbxBitrate = new Fl_Choice(110, y, 150, b, "Bitrate:");
    ui.cbxBitrate->add("96 - Low");
    ui.cbxBitrate->add("128 - OK");
    ui.cbxBitrate->add("160 - Medium");
    ui.cbxBitrate->add("192 - Better");
    ui.cbxBitrate->add("224 - Good");
    ui.cbxBitrate->add("256 - Very Good");
    ui.cbxBitrate->add("320 - Highest");
    ui.cbxBitrate->value(1);
    y += b + d;

    ui.quality = new Fl_Choice(110, y, 150, b, "Quality:");
    ui.quality->add("0 - Best");
    ui.quality->add("1 - High");
    ui.quality->add("5 - Default");
    ui.quality->add("7 - Fast");
    ui.quality->add("9 - Fastest");
    ui.quality->value(0);
    y += b + d;

    ui.convert = new Fl_Button(110, y, 150, b, "Convert now");
    ui.convert->callback(convert_cb);
    ui.convert->deactivate();
    ui.cancel = new Fl_Button(270, y, 150, b, "Cancel");
    ui.cancel->callback(cancel_cb);

    ui.darkToggle = new Fl_Button(430, y, 150, b, "Dark Mode");
    ui.darkToggle->callback(darkmode_cb);
    y += b + d;

    ui.progress = new XP_Progress(110, y, 400, b);
    ui.progress->minimum(0);
    ui.progress->maximum(1);
    ui.progress->value(0);
    y += b + d;

    ui.btnCompare = new Fl_Button(110, y, 200, b, "Compare files");
    ui.btnCompare->callback(compare_cb);
    y += b + d;

    ui.logbuf   = new Fl_Text_Buffer();
    ui.stylebuf = new Fl_Text_Buffer();

    int dy = 600 - d - y;
    ui.logbox = new Fl_Text_Display(10, y, 580, dy);
    ui.logbox->buffer(ui.logbuf);

    // Style table: jeder char in stylebuf → Style-Index
    Fl_Text_Display::Style_Table_Entry styles[] = {
        { FL_BLACK, FL_COURIER, 14 },       // 'A' = info
        { FL_RED,   FL_COURIER_BOLD, 14 },  // 'B' = error
        { FL_BLUE,  FL_COURIER, 14 },       // 'C' = debug
        { FL_MAGENTA,FL_COURIER, 14 },       // 'D' = warn
        { FL_DARK_GREEN, FL_COURIER_BOLD, 14 },  // 'E' = success
    };

    ui.logbox->highlight_data(
        ui.stylebuf,
        styles,
        sizeof(styles)/sizeof(styles[0]),
        'A',   // Default style
        nullptr, nullptr
    );

    win->resizable(win);   // oder ein Child-Widget
    win->end();

    #ifdef _WIN32
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(aaaa));
    win->icon((char*)hIcon);
    #else
    // win->icon((char*)LoadIcon(NULL, IDI_APPLICATION));
    #endif

    win->show(argc, argv);

    // Fl::set_font(FL_HELVETICA, "Noto Emoji");
    // Fl::set_font(FL_FREE_FONT, "Noto Emoji");
    // my_widget->labelfont(FL_FREE_FONT);
    // my_widget->labelsize(20);
    // my_widget->label("🔥 Feuer!");

    // log_debug("Test: log_debug()");
    // log_error("Test: log_error()");
    // log_info("Test: log_info()");
    // log_warn("Test: log_warn()");
    // log_success("Test: log_success()");

    return Fl::run();
}
