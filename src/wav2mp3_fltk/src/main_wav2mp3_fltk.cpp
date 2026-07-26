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


#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>


// #define DR_WAV_IMPLEMENTATION
// #include "dr_wav.h"

#ifdef _WIN32 // only for Window ICOn
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>            // only for Window ICOn
    #include "../res/resource.h"    // only for Window ICOn
#endif

#include <de/Core.h>

#include <DarkSound.h>

/*
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

Fl_Text_Display* logbox;
Fl_Text_Buffer*  logbuf;
Fl_Text_Buffer*  stylebuf;

// Style table: jeder char in stylebuf → Style-Index
Fl_Text_Display::Style_Table_Entry styles[] = {
    { FL_BLACK, FL_COURIER, 14 },        // 'A' = normal
    { FL_RED,   FL_COURIER_BOLD, 14 },   // 'B' = error
    { FL_BLUE,  FL_COURIER, 14 },        // 'C' = info
};

void add_log(const char* msg, char style)
{
    // Text anhängen
    logbuf->append(msg);
    logbuf->append("\n");

    // Style anhängen (gleiche Länge)
    int len = strlen(msg) + 1;
    std::string s(len, style);
    stylebuf->append(s.c_str());

    logbox->scroll(logbuf->length(), 0);
}

int main()
{
    Fl_Window* win = new Fl_Window(600, 400, "Logbox");
    logbuf   = new Fl_Text_Buffer();
    stylebuf = new Fl_Text_Buffer();

    logbox = new Fl_Text_Display(10, 10, 580, 380);
    logbox->buffer(logbuf);

    logbox->highlight_data(
        stylebuf,
        styles,
        sizeof(styles)/sizeof(styles[0]),
        'A',   // Default style
        nullptr, nullptr
    );

    win->end();
    win->show();

    add_log("System started", 'C');
    add_log("Loading modules...", 'A');
    add_log("ERROR: Failed to open file", 'B');

    return Fl::run();
}
*/

// ---------------- UI ----------------
struct UI {
    Fl_Input* inFile;
    Fl_Input* outFile;
    Fl_Button* btnIn;
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

    std::atomic<bool> cancelFlag{false};
    std::thread worker;

    std::string in;
    std::string out;
    int bitrate; // bitrate
    int q;  // quality
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

        // Hintergrund
        w->color(fl_rgb_color(45,45,45));

        // Text
        w->labelcolor(fl_rgb_color(230,230,230));
        //w->textcolor(fl_rgb_color(230,230,230));

        // Auswahl
        w->selection_color(fl_rgb_color(80,80,160));
        //w->selection_text_color(fl_rgb_color(255,255,255));

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
        //Fl::scheme("gtk+");
        Fl::background(30,30,30);
        Fl::foreground(220,220,220);
        //apply_dark_theme(Fl::first_window());
    }
    else
    {
        //Fl::scheme("plastic");
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
    Fl::awake(convert_start_awake, nullptr);

    ui.cancelFlag = false;

    // New Async
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    de::Sound sound;
    if (!dbLoadSound(sound,ui.in))
    {
        auto s = dbStr("Cannot read inputFile");
        log_error(s.c_str());
        return;
    }

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

    if (!dbSaveSound(sound, ui.out, optSave))
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

// ---------------- file pickers ----------------
void pick_input_cb(Fl_Widget*, void*)
{
    Fl_Native_File_Chooser dlg;
    dlg.type(Fl_Native_File_Chooser::BROWSE_FILE);
    dlg.filter("All files\t*.*\n"
               "WAV\t*.wav\n"
               "MP3\t*.mp3\n"
               "M4A\t*.m4a\n"
               "MP4\t*.mp4\n"
               "FLAC\t*.flac\n");
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


// ---------------- main ----------------
int main(int argc, char** argv)
{
    // =============================================================
    // Test 1:
    // =============================================================
    de::Sound snd1;
    dbLoadSound(snd1,"C:/_media/Music/wav/piano-space.wav");
    dbSaveSound(snd1,"C:/_media/Music/wav/piano-space_test1.mp3");

    // =============================================================
    // Test 2:
    // =============================================================
    de::Sound snd2;
    dbCopySound(snd1,snd2,snd1.m_frames);
    dbSaveSound(snd2,"C:/_media/Music/wav/piano-space_test2_copy.mp3");

    // =============================================================
    // Test 3:
    // =============================================================
    de::Sound snd3;
    de::Sound snd4;
    dbDeinterleaveSound(snd2,snd3);
    dbSaveSound(snd3,"C:/_media/Music/wav/piano-space_test3_1.planar.mp3");
    dbInterleaveSound(snd3,snd4);
    dbSaveSound(snd4,"C:/_media/Music/wav/piano-space_test3_2.interleaved.mp3");

    // =============================================================
    // Test 4:
    // =============================================================
    de::Sound snd5;
    de::Sound snd6;
    dbConvertSound(snd1,snd5,de::SampleType::F64);
    dbSaveSound(snd5,"C:/_media/Music/wav/piano-space_test4_convertF64.mp3");
    dbConvertSound(snd5,snd6,de::SampleType::F32);
    dbSaveSound(snd6,"C:/_media/Music/wav/piano-space_test4_convertF32.mp3");

    // =============================================================
    // Test 5:
    // =============================================================
    de::Sound snd7;
    dbResampleSound(snd1,snd7,48000);
    dbSaveSound(snd7,"C:/_media/Music/wav/piano-space_test5_resample_48Hz_r8brain.mp3");

    // =============================================================

    Fl::scheme("none");
    //Fl::scheme("gtk+");
    //Fl::scheme("plastic");
    //Fl::scheme("gleam");
    //Fl::scheme("oxy");

    Fl_Window* win = new Fl_Window(600, 600, "WAV to MP3 | benjaminhampe@gmx.de | fltk-1.4.5 + lame-3.100 + dr.wav + dr.mp3");
    //win->resizable();
    win->begin();

    ui.inFile = new InputField(110, 10, 400, 30, "Inputfile:");
    ui.btnIn  = new Fl_Button(520, 10, 60, 30, "...");
    ui.btnIn->callback(pick_input_cb);

    ui.outFile = new Fl_Input(110, 50, 400, 30, "Outputfile:");
    ui.btnOut  = new Fl_Button(520, 50, 60, 30, "...");
    ui.btnOut->callback(pick_output_cb);

    ui.cbxBitrate = new Fl_Choice(110, 90, 150, 30, "Bitrate:");
    ui.cbxBitrate->add("96 - Low");
    ui.cbxBitrate->add("128 - OK");
    ui.cbxBitrate->add("160 - Medium");
    ui.cbxBitrate->add("192 - Better");
    ui.cbxBitrate->add("224 - Good");
    ui.cbxBitrate->add("256 - Very Good");
    ui.cbxBitrate->add("320 - Highest");
    ui.cbxBitrate->value(1);

    ui.quality = new Fl_Choice(110, 130, 150, 30, "Quality:");
    ui.quality->add("0 - Best");
    ui.quality->add("1 - High");
    ui.quality->add("5 - Default");
    ui.quality->add("7 - Fast");
    ui.quality->add("9 - Fastest");
    ui.quality->value(0);

    ui.convert = new Fl_Button(110, 170, 150, 30, "Convert now");
    ui.convert->callback(convert_cb);
    ui.convert->deactivate();

    ui.cancel = new Fl_Button(270, 170, 150, 30, "Cancel");
    ui.cancel->callback(cancel_cb);

    ui.darkToggle = new Fl_Button(430, 170, 150, 30, "Dark Mode");
    ui.darkToggle->callback(darkmode_cb);

    ui.progress = new XP_Progress(110, 210, 300, 30);
    ui.progress->minimum(0);
    ui.progress->maximum(1);
    ui.progress->value(0);

    ui.btnCompare = new Fl_Button(110, 250, 200, 30, "Compare files");
    ui.btnCompare->callback(compare_cb);

    ui.logbuf   = new Fl_Text_Buffer();
    ui.stylebuf = new Fl_Text_Buffer();

    ui.logbox = new Fl_Text_Display(10, 290, 580, 300);
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
#endif

    // win->icon((char*)LoadIcon(NULL, IDI_APPLICATION));
    win->show(argc, argv);

    // log_debug("Test: log_debug()");
    // log_error("Test: log_error()");
    // log_info("Test: log_info()");
    // log_warn("Test: log_warn()");
    // log_success("Test: log_success()");

    return Fl::run();
}



/*

void convert_async()
{
    Fl::awake(convert_start_awake, nullptr);

    // New Async
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    const char* inWav = ui.in.c_str();
    const char* outMp3 = ui.out.c_str();
    const int bitrate = ui.br;
    const int lameQuality = ui.q;

    ui.cancelFlag = false;

    de::Sound sound;
    if (!dbLoadSound(sound,ui.in))
    {
        auto s = dbStr("Cannot read inputFile");
        log_error(s.c_str());
        return;
    }

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, sound.m_sampleRate);
    lame_set_num_channels(lame, sound.m_channels);
    lame_set_brate(lame, bitrate);
    lame_set_quality(lame, lameQuality);
    lame_init_params(lame);

    FILE* out = fopen(outMp3, "wb");
    if (!out)
    {
        log_error("Cannot write MP3 output-file");
        lame_close(lame);
        return;
    }

    const int64_t FRAMES = 1152 * 16;
    const int64_t SAMPLES = FRAMES * sound.m_channels;

    // Single raw byte buffer
    de::TAlignedVector<uint8_t> chunk(SAMPLES * sound.getBytesPerSample());

    // MP3 output buffer
    de::TAlignedVector<uint8_t> mp3Buf(1.25 * SAMPLES + 7200);

    std::size_t frameCount = sound.m_frames;
    std::size_t frameIndex = 0;

    while (!ui.cancelFlag)
    {
        float* __restrict__ chunkPtr = reinterpret_cast<float*>(chunk.data());
        int64_t framesRead = sound.read_frames_f32(chunkPtr, FRAMES, frameIndex);
        if (framesRead < 1)
            break;

        // new
        frameIndex += framesRead;
        if (frameIndex > frameCount)
            frameIndex = frameCount;

        float percent = static_cast<float>(frameIndex) / static_cast<float>(frameCount);
        Fl::awake(convert_progress_awake, new float(percent));

        int bytes = 0;

        // Interpret raw bytes as float32
        const float* __restrict__ src = reinterpret_cast<const float*>(chunk.data());
        bytes = lame_encode_buffer_interleaved_ieee_float(
            lame,
            src,
            (int)framesRead,
            mp3Buf.data(),
            (int)mp3Buf.size()
        );

        if (bytes > 0)
            fwrite(mp3Buf.data(), 1, bytes, out);

        // New Async
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Fl::awake(convert_progress_awake, new float(1.0f));

    int flushBytes = lame_encode_flush(lame, mp3Buf.data(), (int)mp3Buf.size());
    if (flushBytes > 0)
        fwrite(mp3Buf.data(), 1, flushBytes, out);

    fclose(out);
    lame_close(lame);

    // New Async
    Fl::awake(convert_finished_awake, nullptr);

    return;
}

void convert_async()
{
    Fl::awake(convert_start_awake, nullptr);

    // New Async
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    const char* inWav = ui.in.c_str();
    const char* outMp3 = ui.out.c_str();
    const int bitrate = ui.br;
    const int lameQuality = ui.q;

    ui.cancelFlag = false;

    drwav wav;
    if (!drwav_init_file(&wav, inWav, nullptr))
    {
        log_error("Cannot read WAV input-input");
        return;
    }

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, wav.sampleRate);
    lame_set_num_channels(lame, wav.channels);
    lame_set_brate(lame, bitrate);
    lame_set_quality(lame, lameQuality);
    lame_init_params(lame);

    FILE* out = fopen(outMp3, "wb");
    if (!out)
    {
        log_error("Cannot write MP3 output-file");
        drwav_uninit(&wav);
        lame_close(lame);
        return;
    }

    const size_t FRAMES = 1152 * 16;
    const size_t SAMPLES = FRAMES * wav.channels;

    // Single raw byte buffer
    std::vector<uint8_t> raw(SAMPLES * wav.bitsPerSample / 8);

    // MP3 output buffer
    std::vector<unsigned char> mp3Buf(1.25 * SAMPLES + 7200);

    bool isFloat = (wav.translatedFormatTag == DR_WAVE_FORMAT_IEEE_FLOAT);
    bool isPCM16 = (wav.bitsPerSample == 16 && wav.translatedFormatTag == DR_WAVE_FORMAT_PCM);

    std::size_t total = wav.totalPCMFrameCount;
    std::size_t processed = 0;
    //std::size_t CHUNK = 1152 * 16;

    while (!ui.cancelFlag) {

        // if (processed >= total)
        //     break;

        // old
        size_t framesRead = drwav_read_pcm_frames(&wav, FRAMES, raw.data());
        if (framesRead == 0)
            break;

        // new
        processed += framesRead;
        if (processed > total)
            processed = total;

        float percent = static_cast<float>(processed) / static_cast<float>(total);
        Fl::awake(convert_progress_awake, new float(percent));

        int bytes = 0;

        if (isFloat) {
            // Interpret raw bytes as float32
            float* f = reinterpret_cast<float*>(raw.data());
            bytes = lame_encode_buffer_interleaved_ieee_float(
                lame,
                f,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }
        else if (isPCM16) {
            // Interpret raw bytes as int16_t
            int16_t* s = reinterpret_cast<int16_t*>(raw.data());
            bytes = lame_encode_buffer_interleaved(
                lame,
                s,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }
        else {
            // Other formats: convert in-place into float32
            float* f = reinterpret_cast<float*>(raw.data());
            size_t samples = framesRead * wav.channels;

            if (wav.bitsPerSample == 24)
            {
                // 24-bit → float
                for (size_t i = 0; i < samples; i++)
                {
                    uint8_t* p = raw.data() + i * 3;
                    int32_t v = (p[0] | (p[1] << 8) | (p[2] << 16));
                    if (v & 0x800000) v |= ~0xFFFFFF;
                    f[i] = (float)v / 8388607.0f;
                }
            }
            else if (wav.bitsPerSample == 32 && wav.translatedFormatTag == DR_WAVE_FORMAT_PCM)
            {
                int32_t* p = reinterpret_cast<int32_t*>(raw.data());
                for (size_t i = 0; i < samples; i++)
                    f[i] = (float)p[i] / 2147483647.0f;
            }
            else
            {
                log_error("Unsupported WAV format");
                fclose(out);
                drwav_uninit(&wav);
                lame_close(lame);
                return;
            }

            bytes = lame_encode_buffer_interleaved_ieee_float(
                lame,
                f,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }

        if (bytes > 0)
            fwrite(mp3Buf.data(), 1, bytes, out);

        // New Async
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Fl::awake(convert_progress_awake, new float(1.0f));

    int flushBytes = lame_encode_flush(lame, mp3Buf.data(), (int)mp3Buf.size());
    if (flushBytes > 0)
        fwrite(mp3Buf.data(), 1, flushBytes, out);

    fclose(out);
    drwav_uninit(&wav);
    lame_close(lame);

    // New Async
    Fl::awake(convert_finished_awake, nullptr);

    return;
}

void convert_async()
{
    const char* inWav = ui.in.c_str();
    const char* outMp3 = ui.out.c_str();
    const int bitrate = ui.br;
    const int lameQuality = ui.q;

    ui.cancelFlag = false;

    drwav wav;
    if (!drwav_init_file(&wav, inWav, nullptr)) {
        DE_ERROR("Cannot open WAV input")
        return;
    }

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, wav.sampleRate);
    lame_set_num_channels(lame, wav.channels);
    lame_set_brate(lame, bitrate);
    lame_set_quality(lame, lameQuality);
    lame_init_params(lame);

    FILE* out = fopen(outMp3, "wb");
    if (!out) {
        DE_ERROR("Cannot open MP3 output")
        drwav_uninit(&wav);
        lame_close(lame);
        return;
    }

    const size_t FRAMES = 1152 * 16;
    const size_t SAMPLES = FRAMES * wav.channels;

    // Single raw byte buffer
    std::vector<uint8_t> raw(SAMPLES * wav.bitsPerSample / 8);

    // MP3 output buffer
    std::vector<unsigned char> mp3Buf(1.25 * SAMPLES + 7200);

    bool isFloat = (wav.translatedFormatTag == DR_WAVE_FORMAT_IEEE_FLOAT);
    bool isPCM16 = (wav.bitsPerSample == 16 && wav.translatedFormatTag == DR_WAVE_FORMAT_PCM);

    std::size_t total = wav.totalPCMFrameCount;
    std::size_t processed = 0;
    //std::size_t CHUNK = 1152 * 16;

    while (!ui.cancelFlag) {

        // if (processed >= total)
        //     break;

        // old
        size_t framesRead = drwav_read_pcm_frames(&wav, FRAMES, raw.data());
        if (framesRead == 0)
            break;

        // new
        processed += framesRead;
        if (processed > total)
            processed = total;

        float percent = static_cast<float>(processed) / static_cast<float>(total);
        Fl::awake(progress_awake, new float(percent));

        int bytes = 0;

        if (isFloat) {
            // Interpret raw bytes as float32
            float* f = reinterpret_cast<float*>(raw.data());
            bytes = lame_encode_buffer_interleaved_ieee_float(
                lame,
                f,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }
        else if (isPCM16) {
            // Interpret raw bytes as int16_t
            int16_t* s = reinterpret_cast<int16_t*>(raw.data());
            bytes = lame_encode_buffer_interleaved(
                lame,
                s,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }
        else {
            // Other formats: convert in-place into float32
            float* f = reinterpret_cast<float*>(raw.data());
            size_t samples = framesRead * wav.channels;

            if (wav.bitsPerSample == 24)
            {
                // 24-bit → float
                for (size_t i = 0; i < samples; i++)
                {
                    uint8_t* p = raw.data() + i * 3;
                    int32_t v = (p[0] | (p[1] << 8) | (p[2] << 16));
                    if (v & 0x800000) v |= ~0xFFFFFF;
                    f[i] = (float)v / 8388607.0f;
                }
            }
            else if (wav.bitsPerSample == 32 && wav.translatedFormatTag == DR_WAVE_FORMAT_PCM)
            {
                int32_t* p = reinterpret_cast<int32_t*>(raw.data());
                for (size_t i = 0; i < samples; i++)
                    f[i] = (float)p[i] / 2147483647.0f;
            }
            else
            {
                DE_ERROR("Unsupported WAV format")
                fclose(out);
                drwav_uninit(&wav);
                lame_close(lame);
                return;
            }

            bytes = lame_encode_buffer_interleaved_ieee_float(
                lame,
                f,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }

        if (bytes > 0)
            fwrite(mp3Buf.data(), 1, bytes, out);

        // New Async
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Fl::awake(progress_awake, new float(1.0f));

    int flushBytes = lame_encode_flush(lame, mp3Buf.data(), (int)mp3Buf.size());
    if (flushBytes > 0)
        fwrite(mp3Buf.data(), 1, flushBytes, out);

    fclose(out);
    drwav_uninit(&wav);
    lame_close(lame);

    // New Async
    Fl::awake(finished_awake, nullptr);

    return;
}
*/
