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
    Fl_Scrollbar* scrollBar;

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

    int bitrate; // bitrate
    int q;  // quality

    de::Sound soundIn;
    de::Sound soundOut;

    std::string getSrcUri() const { return inFile->value(); }
    std::string getDstUri() const { return outFile->value(); }
};

UI ui;

// Style table: jeder char in stylebuf → Style-Index
static const Fl_Text_Display::Style_Table_Entry g_logStyles[] = {
    { FL_BLACK, FL_COURIER, 14 },       // 'A' = info
    { FL_RED,   FL_COURIER_BOLD, 14 },  // 'B' = error
    { FL_BLUE,  FL_COURIER, 14 },       // 'C' = debug
    { FL_MAGENTA,FL_COURIER, 14 },       // 'D' = warn
    { FL_DARK_GREEN, FL_COURIER_BOLD, 14 },  // 'E' = success
};

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

    Fl::redraw();
}

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

// ---------------- callbacks ----------------
static void cancel_cb(Fl_Widget*, void*)
{
    ui.cancelFlag = true;
}

static void progress_awake(void* data)
{
    int percent = *static_cast<int*>(data);
    ui.progress->value(0.01f * percent);
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

    auto s1 = dbStr("Convert Start: bitrate = ",ui.bitrate,", lameQualityPreset = ",ui.q);
    async_log_ok(s1);
    async_progress(0);

    de::SoundSaveOptions optSave;
    optSave.bCancelFlag = &ui.cancelFlag;
    optSave.bitrate = ui.bitrate;
    optSave.quality = ui.q;
    optSave.onProgress = [](int pc) { async_progress(pc); };

    if (!dbSaveSound(ui.soundIn, dstUri, optSave))
    {
        auto s2 = dbStr("Cannot save outputFile");
        async_log_error(s2);
        return;
    }

    auto s3 = dbStr("Convert Finished: ",dstUri);
    async_log_ok(s3);
}

void convert_cb(Fl_Widget*, void*)
{
    int bitrate_map[] = {96,128,160,192,224,256,320};
    int quality_map[] = {0,1,5,7,9};

    ui.bitrate = bitrate_map[ui.cbxBitrate->value()];
    ui.q  = quality_map[ui.quality->value()];

    ui.worker = std::thread(convert_async);
    ui.worker.detach();
}

// ---------------- Fl::awake ----------------
void load_async_finish_awake(void*)
{
    ui.inWavf->setSound(&ui.soundIn);
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
        ui.inFile->value(uri.c_str());
        uri = make_mp3_name(uri);
        ui.outFile->value(uri.c_str());

        // TODO: Rework when button is activated.
        ui.convert->activate();
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
        ui.outFile->value(uri.c_str());
    }
}

// ---------------- Fl::awake ----------------
void compare_async_start_awake(void* data)
{
    ui.progress->value(0);
}

// ---------------- callbacks ----------------
void compare_async()
{
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

// =============================================================
class InputField : public Fl_Input
// =============================================================
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
            std::string uri = value();

            if (dbExistFile(uri))
            {
                ui.convert->activate();
                auto dst = make_mp3_name(uri);
                ui.outFile->value(dst.c_str());
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
class XP_Progress : public Fl_Progress
// =============================================================
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

void hscroll_cb(Fl_Widget* w, void* data)
{
    auto scrollBar = (Fl_Scrollbar*)w;

    ui.inWavf->setZoomStart( scrollBar->value() );
}


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

    int d = 5;
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

    ui.scrollBar = new Fl_Scrollbar(10, y, 580, b);
    ui.scrollBar->type(FL_HORIZONTAL);
    ui.scrollBar->bounds(0, 1);
    ui.scrollBar->value(0);
    ui.scrollBar->callback(hscroll_cb, nullptr);
    y += b + d;

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
    ui.logbox->highlight_data(
        ui.stylebuf,
        g_logStyles,
        sizeof(g_logStyles)/sizeof(g_logStyles[0]),
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
