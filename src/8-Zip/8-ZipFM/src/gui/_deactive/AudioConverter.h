#include <gui/GuiCommon.h>

// =============================================================
class UI_FileInputField : public Fl_Input
{
public:
    Fl_Button* btnInput = nullptr;
    Fl_Button* btnZip = nullptr;
    Fl_Input* edtOutput = nullptr;

    UI_FileInputField(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Input(X, Y, W, H, L)
    {
        value("Drag & Drop a file or directory here...");
    }

    void setZipButton(Fl_Button* btn) { btnZip = btn; }

    void setInputButton(Fl_Button* btn) { btnInput = btn; }

    void setOutputLineEdit(Fl_Input* edt) { edtOutput = edt; }

    // void setOutputLineEdit(Fl_Input* edt) { edtOutput = edt; }

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
            std::string uri = de::FileSystem::makePosixPath(value());
            value(uri.c_str());

            if (dbExistFile(uri))
            {
                auto ext = dbFileSuffix(uri);
                if (ext == "zst" || ext == "zstd")
                {
                    DE_OK("Got .zstd file to decompress", uri)

                    if (btnInput) btnInput->label("Archive");
                    if (btnZip) btnZip->activate();
                    if (btnZip) btnZip->label("Decompress");

                    auto dst = dbParentDir(uri) + "/" + dbFileBase(uri);
                    if (edtOutput) edtOutput->value(dst.c_str());
                }
                else
                {
                    DE_OK("Got file to compress", uri)

                    if (btnInput) btnInput->label("File");
                    if (btnZip) btnZip->activate();
                    if (btnZip) btnZip->label("Compress");

                    auto dst = uri + ".zst";
                    if (edtOutput) edtOutput->value(dst.c_str());
                }
            }
            else if (dbExistDirectory(uri))
            {
                DE_OK("Got directory ", uri)

                if (btnInput) btnInput->label("Directory");
                if (btnZip) btnZip->activate();
                if (btnZip) btnZip->label("Compress");

                auto dst = uri + ".zst";
                if (edtOutput) edtOutput->value(dst.c_str());
            }
            else
            {
                DE_ERROR("Input not a file, archive or directory. ", uri)
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
    Fl_Button* btnLoad = nullptr;
    Fl_Box* lblLoad = nullptr;
    UI_FileInputField* edtUri = nullptr;
    // Fl_Button* btnChoose = nullptr;

    UI_FileInput(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;
        btnLoad  = new Fl_Button(x, y, w1, H, "Unselected"); x += w1 + s;
        lblLoad  = new Fl_Box(x, y, w2, H, "Input:"); x += w2 + s;
        edtUri = new UI_FileInputField(x, y, w3, H, ""); // x += w3 + s;
        // btnChoose  = new Fl_Button(x, y, w4, H, "...");
        btnLoad->tooltip("Loads file to preview and cut it");
        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;

        btnLoad->resize(x, y, w1, H);    x += w1 + s;
        lblLoad->resize(x, y, w2, H);    x += w2 + s;
        edtUri->resize(x, y, w3, H);  // x += w3 + s;
        //btnChoose->resize(x, y, w4, H);
    }
};
// =============================================================
class UI_FileOutput : public Fl_Group
{
public:
    int m_spacing = 5;

    Fl_Button* btnZip = nullptr;
    Fl_Box* lblOutput = nullptr;
    Fl_Input* edtUri = nullptr;
    // Fl_Button* btnChoose = nullptr;

    UI_FileOutput(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - s*2;
        int x = X;
        int y = Y;

        //ui.img1 = new ImageWidget(x,y,40,H);
        btnZip = new Fl_Button(x,y,w1,H, "Start Zip"); x += w1 + s;
        lblOutput = new Fl_Box(x,y,w2,H,"Output:"); x += w2 + s;
        edtUri = new Fl_Input(x,y,w3,H,""); x += w3 + s;
        // btnChoose = new Fl_Button(x,y,w4,H,"...");

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - s*2;
        int x = X;
        int y = Y;

        btnZip->resize(x,y,w1,H); x += w1 + s;
        lblOutput->resize(x,y,w2,H); x += w2 + s;
        edtUri->resize(x,y,w3,H); // x += w3 + s;
        // btnChoose->resize(x,y,w4,H);
    }
};

// =============================================================
class UI_DropList : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Button* btnLoad = nullptr;
    Fl_Box* lblLoad = nullptr;
    DropList* dropList = nullptr;
    // Fl_Button* btnChoose = nullptr;

    UI_DropList(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;
        btnLoad  = new Fl_Button(x, y, w1, H, "Unselected"); x += w1 + s;
        lblLoad  = new Fl_Box(x, y, w2, H, "Input:"); x += w2 + s;
        dropList = new DropList(x, y, w3, H, ""); // x += w3 + s;
        // btnChoose  = new Fl_Button(x, y, w4, H, "...");
        btnLoad->tooltip("Loads file to preview and cut it");
        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;

        btnLoad->resize(x, y, w1, H);    x += w1 + s;
        lblLoad->resize(x, y, w2, H);    x += w2 + s;
        dropList->resize(x, y, w3, H);  // x += w3 + s;
        //btnChoose->resize(x, y, w4, H);
    }
};


// ---------------- UI ----------------
struct UI
{
    Fl_Menu_Bar* menuBar = nullptr;
    UI_FileOutput* outFile = nullptr;
    UI_FileInput* inFile = nullptr;
    UI_DropList* inList = nullptr;
    UI_Progress* progress = nullptr;
    UI_LogBox* logbox = nullptr;

    std::atomic<bool> bCancelFlag{false};
    // std::atomic<bool> reloadFile{true};

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

// ---------------- callbacks ----------------
static void cancel_cb(Fl_Widget*, void*)
{
    ui.bCancelFlag = true;
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
void compress_async()
{
    ui.bCancelFlag = false;
    std::string srcUri = ui.getSrcUri();
    std::string dstUri = ui.getDstUri();

    async_log_ok(dbStr("Compress Start: qualityPreset = ",ui.quality));
    async_progress(0);

    FileNamesA fileNames;
    collectFileNames(srcUri, fileNames);

    if (fileNames.empty())
    {
        async_log_error("No files: ");
        async_log_error(dbStr("Compress srcUri = ",srcUri));
        async_log_error(dbStr("Compress dstUri = ",dstUri));
        async_progress(0);
        return;
    }

    auto tarUri = dstUri+".tar";
    if (!tar_writer(fileNames,tarUri))
    {
        async_log_error("TAR failed: ");
        async_log_error(dbStr("srcUri = ",srcUri));
        async_log_error(dbStr("tarUri = ",tarUri));
        async_progress(0);
        return;
    }

    async_log_ok("TAR file created. ");
    async_log_ok(dbStr("TAR srcUri = ",srcUri));
    async_log_ok(dbStr("TAR tarUri = ",tarUri));
    async_progress(5);

    // CompressOptions opts;
    // opts.bCancelFlag = &ui.bCancelFlag;
    // opts.onLogger = [](int level, std::string msg) { async_log_common(msg,level); };
    // opts.onProgress = [](int pc) { async_progress(pc); };

    // if (!compress_zstd(tarUri,dstUri, opts))
    // {
    //     async_log_error("Compress failed: ");
    //     async_log_error(dbStr("Compress srcUri = ",srcUri));
    //     async_log_error(dbStr("Compress tarUri = ",tarUri));
    //     async_log_error(dbStr("Compress dstUri = ",dstUri));
    //     async_progress(0);
    //     return;
    // }

    // async_log_ok("TAR + ZStd compress Finished OK.");
    // async_log_ok(dbStr("Compress srcUri = ",srcUri));
    // async_log_ok(dbStr("Compress tarUri = ",tarUri));
    // async_log_ok(dbStr("Compress dstUri = ",dstUri));
    async_progress(100);
}

void compress_cb(Fl_Widget*, void*)
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

    ui.worker = std::thread(compress_async);
    ui.worker.detach();
}

static auto g_FileFilter =
    "All Files\t*.*\n"
    "ZStd Archive\t*.{zst,zstd}\n";
/*
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
        uri = make_output_name(uri);
        ui.outFile->edtUri->value(uri.c_str());

        // TODO: Rework when button is activated.
        ui.outFile->btnZip->activate();
    }
}
*/

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
    ui.bCancelFlag = false;
    ui.worker = std::thread(compare_async);
    ui.worker.detach();
}


void onMenuFile_Open(Fl_Widget*, void*) {
    fl_message("Open clicked");
}

void onMenuFile_Quit(Fl_Widget*, void*) {
    Fl::first_window()->hide();   // clean FLTK exit
}

void onMenuHelp_Install(Fl_Widget*, void*) { win32_8zip_install(); }

void onMenuHelp_Unistall(Fl_Widget*, void*) { win32_8zip_uninstall(); }

void onMenuHelp_About(Fl_Widget*, void*) {

    auto s = dbStr(
        "About dialog\n",
        "\n"
        "IsAdmin = ", win32_is_admin(), "\n"
        "IsInstalled = ", win32_8zip_is_installed(), "\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "\n");
    fl_message("%s", s.c_str());
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
        const int hR = H - 3*h1 - 5*d; // hRemain
        const int h2 = 2 * hR / 3;
        const int h3 = hR - h2;
        int x = d;
        int y = d;

        ui.menuBar = new Fl_Menu_Bar(0, 0, 400, 25);

        //ui.menuBar->add("&File/&Open", 0, onMenuFile_Open);
        ui.menuBar->add("&File/&Exit program", 0, onMenuFile_Quit);

        ui.menuBar->add("&Help/&Install", 0, onMenuHelp_Install);
        ui.menuBar->add("&Help/&Deinstall", 0, onMenuHelp_Unistall);

        ui.menuBar->add("&Help/&About", 0, onMenuHelp_About);

        ui.outFile = new UI_FileOutput(x,y,W-2*d,h1,d); y += h1 + d;
        ui.inFile = new UI_FileInput(x,y,W-2*d,h1,d); y += h1 + d;
        ui.inList = new UI_DropList(x,y,W-2*d,h2,d); y += h2 + d;
        ui.progress = new UI_Progress(x,y,W-2*d,h1,d); y += h1 + d;
        ui.logbox = new UI_LogBox(x,y,W-2*d,h3,d);

        // Connect
        //ui.inFile->btnLoad->callback(load_async_cb);
        //ui.inFile->btnChoose->callback(pick_input_cb);
        ui.inFile->edtUri->setOutputLineEdit(ui.outFile->edtUri);
        ui.inFile->edtUri->setZipButton(ui.outFile->btnZip);
        ui.inFile->edtUri->setInputButton(ui.inFile->btnLoad);

        // Connect
        ui.outFile->btnZip->callback(compress_cb);
        ui.outFile->btnZip->deactivate();
        //ui.outFile->btnChoose->callback(pick_output_cb);

        // Connect
        ui.progress->btnCompare->callback(compare_cb);
        ui.progress->btnCancel->callback(cancel_cb);
        ui.progress->btnDarkMode->callback(darkmode_cb);

        ui.inList->dropList->onListChangeListeners.push_back(
            []() -> void
            {
                const auto fileNames = ui.inList->dropList->getFileNamesA();
                const auto outName = computeBestOutputFileName(fileNames,"tar.zst");
                ui.outFile->edtUri->value(outName.c_str());
            });
        end();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);

        const float zoom = Fl::screen_scale(0);
        const int d = 5 * zoom;
        const int h0 = 25 * zoom;
        const int h1 = 30 * zoom;
        const int hR = H - h0 - 3*h1 - 5*d; // hRemain
        const int h2 = 2 * hR / 3;
        const int h3 = hR - h2;
        int x = d;
        int y = d;

        ui.menuBar->resize(x,y,W-2*d,h0); y += h0 + d;
        ui.outFile->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.inFile->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.inList->resize(x,y,W-2*d,h2); y += h2 + d;
        ui.progress->resize(x,y,W-2*d,h1); y += h1 + d;
        ui.logbox->resize(x,y,W-2*d,h3);
    }
};

static int global_handler(int event)
{
    if (event == FL_MOUSEWHEEL)
    {
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
