#include "8z_Worker.h"
#include <de/archive/FileInfo.h>

namespace EightZip {
namespace worker {

template <typename T>
class DoubleBufferDirty {
public:
    void workerWrite(const T& value)
    {
        int wi = writeIndex.load(std::memory_order_relaxed);
        buffers[wi] = value;
        writeIndex.store(1 - wi, std::memory_order_release);
        dirty.store(true, std::memory_order_release);
    }

    DoubleBufferDirty& operator= (const T& value)
    {
        workerWrite(value);
        return *this;
    }

    bool mainReadIfDirty(T& out)
    {
        if (dirty.exchange(false))
        {
            int ri = writeIndex.load(std::memory_order_acquire);
            out = buffers[ri];
            return true;
        }
        return false;
    }

private:
    std::atomic<int> writeIndex{0};
    std::atomic<bool> dirty{false};
    T buffers[2];
};

namespace {

    struct Util
    {
        // Anticipate Long NT Paths (start with \\?\C: ...)
        static std::wstring getExeFile()
        {
            constexpr DWORD NT_MAX_PATH = 32767; // With trailing '\0'

            std::wstring blob;
            blob.resize(NT_MAX_PATH);

            DWORD n = GetModuleFileNameW(nullptr, blob.data(), NT_MAX_PATH);

            if (n == 0)
                return L""; // error

            blob.resize(n);
            return blob;
        }

        static std::wstring getExeDir()
        {
            std::wstring exeDir = getExeFile();
            size_t pos = exeDir.find_last_of(L"\\/");
            if (pos != std::wstring::npos)
            {
                exeDir = exeDir.substr(0, pos);
            }
            return exeDir;
        }
    };

} // end namespace.

/*
template< typename T >
struct StateMachine2
{
    T curr;
    T last;

    StateMachine2() : curr{}, last{} {}
    StateMachine2(const T& value) : curr{value}, last{value} {}
    StateMachine2& operator= (const T& value)
    {
        curr = last = value;
    }
};
*/

struct UI
{
    static constexpr uintptr_t MAGIC_GUI = 0xF1CED123;

    Fl_Window* window = nullptr;

    // Title: 59% <Entpacken|Komprimieren> C:\Github\__lib_neu\2_demos.zst
    Fl_Box* lblTimeCurr = nullptr; // Verstrichene Zeit:
    Fl_Box* edtTimeCurr = nullptr;
    Fl_Box* lblTimeLeft = nullptr; // Verbleibende Zeit:
    Fl_Box* edtTimeLeft = nullptr;

    Fl_Box* lblNumFiles = nullptr; // Dateien:
    Fl_Box* edtFileIndex = nullptr;
    Fl_Box* edtFileCount = nullptr;

    Fl_Box* lblTotalBytes = nullptr;  // Gesamtdatenmenge:
    Fl_Box* edtTotalBytes = nullptr;
    Fl_Box* lblSpeed = nullptr;     // Geschwindigket:
    Fl_Box* edtSpeed = nullptr;

    Fl_Box* lblProcessed = nullptr;   // Verarbeitet:
    Fl_Box* edtProcessed = nullptr;

    Fl_Box* lblCompressed = nullptr;  // Komprimiert:
    Fl_Box* edtCompressed = nullptr;

    Fl_Box* lblCompressRatio = nullptr;  // Kompressionsrate:
    Fl_Box* edtCompressRatio = nullptr;

    Fl_Box* edtModus = nullptr;  // Hinzufügen|Entpacken:
    Fl_Box* edtDir = nullptr;   // Current DirectoryName
    Fl_Box* edtFile = nullptr;  // Current FileName

    XP_ProgressBar* progressBar = nullptr;

    LogBox* logBox = nullptr;

    Fl_Button* btnBackground = nullptr;
    Fl_Button* btnPause = nullptr;
    Fl_Button* btnCancel = nullptr;

    // Other stuff:
    Job job;

    volatile std::atomic<bool> bRunFlag{false};
    volatile std::atomic<bool> bAbortFlag{false};
    volatile std::atomic<bool> bPauseFlag{false};
    // volatile std::atomic<bool> reloadFile{true};

    std::thread worker;

    double pollTimeElapsed; // In [s]
    double pollTimeRemain;  // In [s]
    double pollSpeed;       // In [bytes/s]
    double pollProgress;
    DoubleBufferDirty<std::string> pollFile;
    DoubleBufferDirty<std::string> pollDir;
    uint64_t pollFileIndex;
    uint64_t pollFileCount;
    uint64_t pollTotalBytes;
    uint64_t pollProcessed;
    uint64_t pollCompressed;
    double pollCompressRatio;

    void pollGuiUpdate()
    {
        bool bRedraw = false;

        // DE_BENNI("u.id(", u.id, "), data(",u.data,")")
        double d;
        std::string s;
        uint64_t u;

        if (pollFile.mainReadIfDirty(s))
        {
            edtFile->copy_label(s.c_str());
            edtFile->redraw();
            bRedraw = true;
        }
        if (pollDir.mainReadIfDirty(s))
        {
            edtDir->copy_label(s.c_str());
            edtDir->redraw();
            bRedraw = true;
        }

        edtTimeCurr->copy_label(dbStrSeconds(pollTimeElapsed).c_str());
        edtTimeCurr->redraw();
        edtTimeLeft->copy_label(dbStrSeconds(pollTimeRemain).c_str());
        edtTimeLeft->redraw();
        edtSpeed->copy_label(dbStr(pollSpeed / double(1024*1024)," MB/s").c_str());
        edtSpeed->redraw();
        progressBar->value(pollProgress);
        progressBar->redraw();
        edtFileIndex->copy_label(std::to_string(pollFileIndex).c_str());
        edtFileIndex->redraw();
        edtFileCount->copy_label(std::to_string(pollFileCount).c_str());
        edtFileCount->redraw();
        edtTotalBytes->copy_label(dbStrBytes(pollTotalBytes).c_str());
        edtTotalBytes->redraw();
        edtProcessed->copy_label(dbStrBytes(pollProcessed).c_str());
        edtProcessed->redraw();
        edtCompressed->copy_label(dbStrBytes(pollCompressed).c_str());
        edtCompressed->redraw();
        edtCompressRatio->copy_label(dbStr(int(100.0 * pollCompressRatio)," %").c_str());
        edtCompressRatio->redraw();
        bRedraw = true;
        // if (bRedraw) window->redraw();
    }
};

static UI ui;

// Awake handler
void awake_poll_update(void* payload)
{
    // DE_BENNI("")
    ui.pollGuiUpdate();
    Fl::repeat_timeout(0.01, awake_poll_update); // wiederholen
}

// 🟧
void log_common(const char* msg, char style)
{
    // Text anhängen
    ui.logBox->logbuf->append(msg);
    ui.logBox->logbuf->append("\n");

    // Style anhängen (gleiche Länge)
    int len = strlen(msg) + 1;
    std::string s(len, style);
    ui.logBox->stylebuf->append(s.c_str());

    ui.logBox->logbox->scroll(ui.logBox->logbuf->length(), 0);
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
static void pause_cb(Fl_Widget*, void*)
{
    if (ui.bPauseFlag)
    {
        DE_WARN("Resumed")
        ui.bPauseFlag = false;
        ui.btnPause->label("Pause");
        ui.btnPause->redraw();
    }
    else
    {
        DE_WARN("Paused")
        ui.bPauseFlag = true;
        ui.btnPause->label("Resume");
        ui.btnPause->redraw();
    }
}

// ---------------- callbacks ----------------
static void cancel_cb(Fl_Widget*, void*)
{
    if (ui.bAbortFlag)
    {
        DE_WARN("Abort already in progress")
        return;
    }

    int r = fl_choice(
        "\n"
        "Do you like cancel the operation?\n"
        "\n",
        "Cancel operation",  // Button 0
        "Abort this dialog", // Button 1
        nullptr
    );

    if (r == 0) // Cancel operation
    {
        DE_OK("Pressed Cancel")
        ui.bAbortFlag = true;
        ui.logBox->show();
    }
    else if (r == 1) // Abort
    {
        DE_OK("Pressed Abort")
    }
}

static void finish_cb(void*)
{
    DE_OK("Finish callback from MainThread ",std::this_thread::get_id())

    Fl::remove_timeout(awake_poll_update);

    if (ui.bAbortFlag)
    {
        DE_ERROR("Aborted.")
        log_error("Aborted by user.");
        ui.btnPause->label("Pause");
        ui.btnPause->redraw();
    }
    else
    {
        DE_BENNI("Exit Program from Thread ",std::this_thread::get_id())
        ui.window->hide();
    }
}

// ---------------- worker ----------------
static void workerThread_Demo()
{
    const double timeStart = dbTimeInSeconds();

    if (ui.bRunFlag)
    {
        DE_ERROR("Worker already running, abort")
        return; // Already running!
    }

    ui.bRunFlag = true;
    ui.bAbortFlag = false;
    ui.bPauseFlag = false;
    ui.pollProgress = 0.0;

    DE_BENNI("Begin Worker Thread ",std::this_thread::get_id())

    std::wstring exeDir = Util::getExeDir();
    FileInfos fileInfos;

    DE_BENNI("exeDir = ", de_mbstr(exeDir))
    scanDirectory(fileInfos,exeDir,true);
    DUMP(fileInfos);

    double timeElapsed = 0;
    double timeRemain = 0;
    double speed = 0.0;
    double progress = 0.01;
    std::string curFile;
    std::string curDir;
    uint64_t fileIndex = 0;
    uint64_t processedBytes = 0;
    uint64_t compressedBytes = 0;
    double compressRatio = 1.0;
    uint64_t totalBytes = TOTAL_FILE_SIZE(fileInfos);

    ui.pollProgress = 0.01;
    ui.pollFileCount = fileInfos.size();
    ui.pollTotalBytes = totalBytes;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    int numItersBeforeSleep = 10;
    int curIters = 0;
    dbRandomize();

    uint64_t i = 0;
    uint64_t n = fileInfos.size();
    while (ui.bRunFlag && i < n)
    {
        if (ui.bAbortFlag)
        {
            DE_ERROR("Abort ThreadLoop")
            break;
        }

        if (ui.bPauseFlag)
        {
            std::this_thread::yield();
            continue;
        }

        const auto& fileInfo = fileInfos[i];
        progress = 0.01 + (0.98*double(i+1) / double(fileInfos.size()));
        curFile = de_mbstr(fileInfos[i].fileName());
        curDir = de_mbstr(fileInfos[i].dir());
        fileIndex = i+1;
        processedBytes += fileInfo.fileSize();
        compressedBytes += fileInfo.fileSize() / uint32_t(1+(dbRND() % 47));
        compressRatio = double(compressedBytes) / double(processedBytes);
        timeElapsed = dbTimeInSeconds() - timeStart;
        speed = double(processedBytes) / timeElapsed;
        timeRemain = double(totalBytes - processedBytes) / speed;  // v = s/t -> t = s / v

        ui.pollProgress = progress;
        ui.pollFile = curFile;
        ui.pollDir = curDir;
        ui.pollFileIndex = fileIndex;
        ui.pollProcessed = processedBytes;
        ui.pollCompressed = compressedBytes;
        ui.pollCompressRatio = compressRatio;
        ui.pollTimeElapsed = timeElapsed;
        ui.pollSpeed = speed;
        ui.pollTimeRemain = timeRemain;

        curIters++;
        if (curIters >= numItersBeforeSleep)
        {
            curIters = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        i++;
    }

    ui.bRunFlag = false;
    ui.pollProgress = 1.0;
    DE_BENNI("End Worker Thread ",std::this_thread::get_id())
    Fl::awake(finish_cb,&ui);
}

static void start_worker_cb(Fl_Widget*, void*)
{
    DE_OK("MainThread ",std::this_thread::get_id())

    if ( ui.logBox->visible() != 0)
    {
        ui.logBox->hide();
    }
    else
    {
        ui.logBox->show();
    }

    if (!ui.bRunFlag)
    {
        DE_OK("Start worker from MainThread ",std::this_thread::get_id())

        Fl::add_timeout(0.01, awake_poll_update); // Start polling gui update 10 ms

        ui.worker = std::thread(workerThread_Demo);
        ui.worker.detach();
    }
    else
    {
        DE_ERROR("Worker already running.")
    }
}

/*
    for (size_t i = 0; i < fileNames.size(); ++i)
    {
        // ui.enqueueUpdate({ WID_FileIndex, std::to_string(i+1) });
        // ui.enqueueUpdate({ WID_File, dbFileName(fileNames[i]) });
        // ui.enqueueUpdate({ WID_Dir, dbFileDir(fileNames[i]) });
        // ui.enqueueUpdate({ WID_Progress, std::to_string(double(i+1) / double(fileNames.size())) });
        // ui.requestUpdate();
        // Fl::awake(awake_update_gui,&ui);
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));

        {
            UI_Update2& b = ui.back();
            b.Progress = double(i+1) / double(fileNames.size());
            b.File = dbFileName(fileNames[i]);
            b.Dir = dbFileDir(fileNames[i]);
            b.FileIndex = i + 1;
            Fl::awake(awake_update_gui2,&ui);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
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
*/

Dialog::Dialog(int W, int H, const char* title)
    : Fl_Double_Window(W, H, title)
{
    ui.window = this;

    begin();

    const float zoom = Fl::screen_scale(0);
    const int ml = 10 * zoom;
    const int mt = 10 * zoom;
    const int mr = 10 * zoom;
    const int mb = 10 * zoom;

    const int mw = W - ml - mr;
    const int mh = H - mt - mb;

    const int sx = 5 * zoom;
    const int sy = 3 * zoom;
    const int h1 = 12 * zoom;
    const int h2 = 30 * zoom;
    const int hLogBox = mh - 8*(h1+sy) - 2*(h2+sy); // hLogBox

    const int w0 = 40 * zoom;
    const int w1 = ((mw - w0) / 4) - sx;
    const int wBackG = (mw / 2) - sx;
    const int wPause = (mw / 4) - sx;
    const int wCancel = (mw / 4) - sx;

    // Row[1]
    int x = ml;
    int y = mt;
    ui.lblTimeCurr = new Fl_Box(x,y,w1,h1,"Elapsed Time:"); x += w1 + sx;
    ui.edtTimeCurr = new Fl_Box(x,y,w1,h1,"00:00:00"); x += w1 + sx + w0;
    ui.lblTotalBytes = new Fl_Box(x,y,w1,h1,"Total Size:"); x += w1 + sx;
    ui.edtTotalBytes = new Fl_Box(x,y,w1,h1,"420 MB"); //x += w1 + sx;
    y += h1 + sy;

    ui.lblTimeCurr->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtTimeCurr->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblTotalBytes->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtTotalBytes->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[2]
    x = ml;
    ui.lblTimeLeft = new Fl_Box(x,y,w1,h1,"Remaining Time:"); x += w1 + sx;
    ui.edtTimeLeft = new Fl_Box(x,y,w1,h1,"00:00:00"); x += w1 + sx + w0;
    ui.lblSpeed = new Fl_Box(x,y,w1,h1,"Speed:"); x += w1 + sx;
    ui.edtSpeed = new Fl_Box(x,y,w1,h1,"59 MB/s"); //x += w1 + sx;
    y += h1 + sy;

    ui.lblTimeLeft->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtTimeLeft->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblSpeed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtSpeed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[3]
    x = ml;
    ui.lblNumFiles = new Fl_Box(x,y,w1,h1,"Files:"); x += w1 + sx;
    ui.edtFileIndex = new Fl_Box(x,y,w1,h1,"3140"); x += w1 + sx + w0;
    ui.lblProcessed = new Fl_Box(x,y,w1,h1,"Processed:"); x += w1 + sx;
    ui.edtProcessed = new Fl_Box(x,y,w1,h1,"123 MB"); //x += w1 + sx;
    y += h1 + sy;

    ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtFileIndex->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblProcessed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtProcessed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[4]
    x = ml + w1 + sx;
    ui.edtFileCount = new Fl_Box(x,y,w1,h1,"4280"); x += w1 + sx + w0;
    ui.lblCompressed = new Fl_Box(x,y,w1,h1,"Compressed:"); x += w1 + sx;
    ui.edtCompressed = new Fl_Box(x,y,w1,h1,"46 MB"); //x += w1 + sx;
    y += h1 + sy;

    //ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtFileCount->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblCompressed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtCompressed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[5]
    x = ml + 2*(w1 + sx) + w0;
    ui.lblCompressRatio = new Fl_Box(x,y,w1,h1,"Compress Ratio:"); x += w1 + sx;
    ui.edtCompressRatio = new Fl_Box(x,y,w1,h1,"34%"); //x += w1 + sx;
    y += h1 + sy;

    //ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    //ui.edtFileCount->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblCompressRatio->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtCompressRatio->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[6]
    x = ml;
    ui.edtModus = new Fl_Box(x,y,mw,h1,"Compress:");
    y += h1 + sy;

    ui.edtModus->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[7]
    x = ml;
    ui.edtDir = new Fl_Box(x,y,mw,h1,"DirectoryName");
    y += h1 + sy;

    ui.edtDir->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[8]
    x = ml;
    ui.edtFile = new Fl_Box(x,y,mw,h1,"FileName");
    y += h1 + sy;

    ui.edtFile->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[9]
    x = ml;
    ui.progressBar = new XP_ProgressBar(x,y,mw,h2);
    y += h2 + sy;

    // Row[10]
    x = ml;
    ui.logBox = new LogBox(x,y,mw,hLogBox,0);
    y += hLogBox + sy;

    // Row[11]
    x = ml;
    ui.btnBackground = new Fl_Button(x,y,wBackG,h2,"Background"); x += wBackG + sx;
    ui.btnPause = new Fl_Button(x,y,wPause,h2,"Pause"); x += wPause + sx;
    ui.btnCancel = new Fl_Button(x,y,wCancel,h2,"Cancel");

    // m_edtArchiveName->onChange = [](int index, std::string text)
    // {
    //     DE_OK("Selected[",index,"] ", text)
    // };

    ui.btnBackground->callback(start_worker_cb);

    ui.btnPause->callback(pause_cb);

    ui.btnCancel->callback(cancel_cb);

    end();

    // Fl::add_awake_handler_(awakeHandler, &ui);

    // resizable(ui.edtFile);
}

void Dialog::resize(int X, int Y, int W, int H)
{
    // 1. Basis-Resize
    Fl_Double_Window::resize(X, Y, W, H);

    const float zoom = Fl::screen_scale(0);
    const int ml = 10 * zoom;
    const int mt = 10 * zoom;
    const int mr = 10 * zoom;
    const int mb = 10 * zoom;

    const int mw = W - ml - mr;
    const int mh = H - mt - mb;

    const int sx = 5 * zoom;
    const int sy = 3 * zoom;
    const int h1 = 12 * zoom;
    const int h2 = 30 * zoom;
    const int hLogBox = mh - 8*(h1+sy) - 2*(h2+sy); // hLogBox

    const int w0 = 40 * zoom;
    const int w1 = ((mw - w0) / 4) - sx;
    const int wBackG = (mw / 2) - sx;
    const int wPause = (mw / 4) - sx;
    const int wCancel = (mw / 4) - sx;

    // Row[1]
    int x = ml;
    int y = mt;
    ui.lblTimeCurr->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtTimeCurr->resize(x,y,w1,h1); x += w1 + sx + w0;
    ui.lblTotalBytes->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtTotalBytes->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[2]
    x = ml;
    ui.lblTimeLeft->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtTimeLeft->resize(x,y,w1,h1); x += w1 + sx + w0;
    ui.lblSpeed->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtSpeed->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[3]
    x = ml;
    ui.lblNumFiles->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtFileIndex->resize(x,y,w1,h1); x += w1 + sx + w0;
    ui.lblProcessed->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtProcessed->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[4]
    x = ml + w1 + sx;
    ui.edtFileCount->resize(x,y,w1,h1); x += w1 + sx + w0;
    ui.lblCompressed->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtCompressed->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[5]
    x = ml + 2*(w1 + sx) + w0;
    ui.lblCompressRatio->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtCompressRatio->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[6]
    x = ml;
    ui.edtModus->resize(x,y,mw,h1);
    y += h1 + sy;

    // Row[7]
    x = ml;
    ui.edtDir->resize(x,y,mw,h1);
    y += h1 + sy;

    // Row[8]
    x = ml;
    ui.edtFile->resize(x,y,mw,h1);
    y += h1 + sy;

    // Row[9]
    x = ml;
    ui.progressBar->resize(x,y,mw,h2);
    y += h2 + sy;

    // Row[10]
    x = ml;
    ui.logBox->resize(x,y,mw,hLogBox);
    y += hLogBox + sy;

    // Row[11]
    x = ml;
    ui.btnBackground->resize(x,y,wBackG,h2); x += wBackG + sx;
    ui.btnPause->resize(x,y,wPause,h2); x += wPause + sx;
    ui.btnCancel->resize(x,y,wCancel,h2);
}


} // end namespace worker.
} // end namespace EightZip.
