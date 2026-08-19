#include <common_zstd_fltk.h>
#include <de/archive/zstd_writer.h>
#include <de/archive/zstd_reader.h>

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


// =============================================================
class UI_FileInputField : public Fl_Input
{
public:
    Fl_Button* btnZip = nullptr;
    Fl_Input* edtOutput = nullptr;

    UI_FileInputField(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Input(X, Y, W, H, L) {}

    void setZipButton(Fl_Button* btn)
    {
        btnZip = btn;
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
                if (btnZip)
                    btnZip->activate();
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
        btnLoad  = new Fl_Button(x, y, w1, H, "File/Dir");
        x += w1 + s;
        lblLoad  = new Fl_Box(x, y, w2, H, "Input:");
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

    Fl_Button* btnZip = nullptr;
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

        //ui.img1 = new ImageWidget(x,y,40,H);
        btnZip = new Fl_Button(x,y,w1,H, "Start Zip"); x += w1 + s;
        lblOutput = new Fl_Box(x,y,w2,H,"Output:"); x += w2 + s;
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
        btnZip->resize(x,y,w1,H); x += w1 + s;
        //ui.img1->resize(x,y,40,H);
        lblOutput->resize(x,y,w2,H); x += w2 + s;
        edtUri->resize(x,y,w3,H); x += w3 + s;
        btnChoose->resize(x,y,w4,H);
    }
};

// ---------------- UI ----------------
struct UI
{
    UI_FileInput* inFile;
    UI_FileOutput* outFile;
    UI_Progress* progress;
    UI_LogBox* logbox;

    std::atomic<bool> reloadFile{true};
    std::atomic<bool> cancelFlag{false};
    std::thread worker;

    int bitrate; // bitrate in kbit, e.g. 128, not 128000
    int quality; // quality 0..9

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

    //ui.waveform->setDarkMode(bDarkMode);
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


    auto s1 = dbStr("Convert Start: bitrate = ",ui.bitrate,", lameQualityPreset = ",ui.quality);
    async_log_ok(s1);
    async_progress(0);

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

    ui.bitrate = 0; // ui.encoder->getBitrate();
    ui.quality = 5; // ui.encoder->getQuality();

    ui.worker = std::thread(convert_async);
    ui.worker.detach();
}

// ---------------- Fl::awake ----------------
void load_async_finish_awake(void*)
{

    ui.progress->progress->value(0);
}
void load_async()
{
    ui.cancelFlag = false;

    std::string uri = ui.getSrcUri();
    async_log_ok(dbStr("Load Start: ",uri));
    async_progress(0);

    async_log_ok(dbStr("Load finished = ",uri));

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
        ui.outFile->btnZip->activate();
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

    // Load output
    async_log_debug(dbStr("Dst: ", dst_uri));
    // Comparing....
    async_log_info("Comparing...");

    log_success("Finished compare.");
}


// ---------------- callbacks ----------------
void compare_cb(Fl_Widget*, void*)
{
    ui.cancelFlag = false;
    ui.worker = std::thread(compare_async);
    ui.worker.detach();
}




class MainWindow : public Fl_Window
{
public:
    MainWindow(int W, int H, const char* title)
        : Fl_Window(W, H, title)
    {
        begin();

        const float zoom = Fl::screen_scale(0);
        const int d = 5 * zoom;
        const int h1 = 30 * zoom;
        const int h2 = 128 * zoom;
        const int h3 = H - 5*(h1+d) - (h2+d) - 2*d;
        int x = d;
        int y = d;

        ui.inFile = new UI_FileInput(x,y,W-2*d,h1,d); y += h1 + d;
        ui.outFile = new UI_FileOutput(x,y,W-2*d,h1,d); y += h1 + d;
        ui.progress = new UI_Progress(x,y,W-2*d,h1,d); y += h1 + d;
        ui.logbox = new UI_LogBox(x,y,W-2*d,h3,d);

        // Connect
        ui.inFile->btnLoad->callback(load_async_cb);
        ui.inFile->btnChoose->callback(pick_input_cb);
        ui.inFile->edtUri->setOutputLineEdit(ui.outFile->edtUri);
        ui.inFile->edtUri->setZipButton(ui.outFile->btnZip);

        // Connect
        ui.outFile->btnZip->callback(convert_cb);
        ui.outFile->btnZip->deactivate();
        ui.outFile->btnChoose->callback(pick_output_cb);

        // Connect
        ui.progress->btnCompare->callback(compare_cb);
        ui.progress->btnCancel->callback(cancel_cb);
        ui.progress->btnDarkMode->callback(darkmode_cb);


        // ui.waveform->scrollBar->onChange = [&] (double value)
        // {
        //     // auto scrollBar = (Fl_Scrollbar*)w;

        //     // auto scrollBar = ui.waveform->scrollBar;

        //     // ui.waveform->setZoomStart( scrollBar->value() );

        //     // ui.waveform->waveform->setZoomFromScrollBar( value );

        // };

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
        const int h3 = H - 3*h1 - 4*d;
        int x = d;
        int y = d;

        ui.inFile->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.outFile->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.progress->resize(x,y,W-2*d,h1); y += h1 + d;
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
