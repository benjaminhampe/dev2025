#include "8z_Worker.h"
#include <de/archive/FileInfo.h>

namespace EightZip {
namespace worker {

/*
Fl::add_timeout(0.5, callback); // 500 ms später
Fl::repeat_timeout(0.01, callback); // alle 10 ms

void poll(void*) {
    pollWorkerData();       // Worker abfragen
    updateGui();            // GUI aktualisieren
    Fl::repeat_timeout(0.01, poll); // wiederholen
}

Fl::add_timeout(0.01, poll); // starten

Fl::remove_timeout(callback);

template <typename T>
class DoubleBuffer {
public:
    // Worker schreibt neuen Zustand
    void workerWrite(const T& value)
    {
        int wi = writeIndex.load(std::memory_order_relaxed);
        buffers[wi] = value; // Copy, aber sicher
        writeIndex.store(1 - wi, std::memory_order_release);
    }

    // Mainthread liest den aktuellen Zustand
    T mainRead() const
    {
        int ri = writeIndex.load(std::memory_order_acquire);
        return buffers[ri]; // Copy out
    }

    // Optional: Referenz statt Kopie
    const T& mainReadRef() const
    {
        int ri = writeIndex.load(std::memory_order_acquire);
        return buffers[ri];
    }

private:
    std::atomic<int> writeIndex{0};
    T buffers[2];
};

*/

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

/*
Mainthread:

GuiState s;
if (guiState.mainReadIfDirty(s)) {
    updateGui(s);
}
*/

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

enum WidgetID
{
    WID_Progress,
    WID_File,
    WID_Dir,
    WID_FileIndex,
    WID_FileCount,
    WID_TotalSize,
    WID_Processed,
    WID_Compressed,
    WID_CompressRatio
};

//🟧
struct UI_Update
{
    WidgetID id; // = WID_Progress;
    std::string data;
};

struct UI_Update2
{
    std::optional<double> Progress;
    std::optional<std::string> File;
    std::optional<std::string> Dir;
    std::optional<uint64_t> FileIndex;
    std::optional<uint64_t> FileCount;
    std::optional<uint64_t> TotalBytes;
    std::optional<uint64_t> Processed;
    std::optional<uint64_t> Compressed;
    std::optional<double> CompressRatio;
};

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

    std::atomic<bool> bRunFlag{false};
    std::atomic<bool> bCancelFlag{false};
    std::atomic<bool> bPauseFlag{false};
    // std::atomic<bool> reloadFile{true};

    std::thread worker;

#if 0
    DoubleBufferDirty<double> pollProgress;
    DoubleBufferDirty<std::string> pollFile;
    DoubleBufferDirty<std::string> pollDir;
    DoubleBufferDirty<uint64_t> pollFileIndex;
    DoubleBufferDirty<uint64_t> pollFileCount;
    DoubleBufferDirty<uint64_t> pollTotalBytes;
    DoubleBufferDirty<uint64_t> pollProcessed;
    DoubleBufferDirty<uint64_t> pollCompressed;
    DoubleBufferDirty<double> pollCompressRatio;
#else
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
#endif

    /*
    Mainthread:

    GuiState s;
    if (guiState.mainReadIfDirty(s)) {
        updateGui(s);
    }
    */
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

#if 0
        if (pollProgress.mainReadIfDirty(d))
        {
            progressBar->value(d);
            progressBar->redraw();
            bRedraw = true;
        }
        if (pollFileIndex.mainReadIfDirty(u))
        {
            edtFileIndex->copy_label(std::to_string(u).c_str());
            edtFileIndex->redraw();
            bRedraw = true;
        }
        if (pollFileCount.mainReadIfDirty(u))
        {
            // DE_OK("Got FileCount ",u)
            edtFileCount->copy_label(std::to_string(u).c_str());
            edtFileCount->redraw();
            bRedraw = true;
        }
        if (pollTotalBytes.mainReadIfDirty(u))
        {
            edtTotalBytes->copy_label(dbStrBytes(u).c_str());
            pollTotalBytes->redraw();
            bRedraw = true;
        }
        if (pollProcessed.mainReadIfDirty(u))
        {
            edtProcessed->copy_label(dbStrBytes(u).c_str());
            edtProcessed->redraw();
            bRedraw = true;
        }
        if (pollCompressed.mainReadIfDirty(u))
        {
            edtCompressed->copy_label(dbStrBytes(u).c_str());
            edtCompressed->redraw();
            bRedraw = true;
        }
        if (pollCompressRatio.mainReadIfDirty(d))
        {
            auto t = std::to_string(int(d)) + " %";
            edtCompressRatio->copy_label(t.c_str());
            edtCompressRatio->redraw();
            bRedraw = true;
        }
#else
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
        edtCompressRatio->copy_label(dbStr(int(pollCompressRatio)," %").c_str());
        edtCompressRatio->redraw();
        bRedraw = true;
#endif
        // if (bRedraw) window->redraw();
    }

#if 0
    // Update mechanism 1:
    moodycamel::ConcurrentQueue<UI_Update> queue;

    void enqueueUpdate(const UI_Update& u)
    {
        //DE_BENNI("queue = ",queue.size_approx())
        queue.enqueue(u);
        //DE_BENNI("queue = ",queue.size_approx())
        //Fl::awake(this);   // Payload = Dispatcher
    }

    void requestUpdate()
    {
        // Fl::awake(this);   // Payload = Dispatcher
    }

    void dispatchUpdates()
    {
        //DE_BENNI("queue = ",queue.size_approx())
        UI_Update u;
        while (queue.try_dequeue(u))
        {
            applyUpdate(u);
        }
    }

    void applyUpdate(const UI_Update& u)
    {
        // DE_BENNI("u.id(", u.id, "), data(",u.data,")")
        switch (u.id)
        {
            case WID_Progress:
            {
                double v = std::atof(u.data.c_str());
                progressBar->value(v);
                progressBar->redraw();
                break;
            }
            case WID_File:
            {
                edtFile->copy_label(u.data.c_str());
                edtFile->redraw();
                break;
            }
            case WID_Dir:
            {
                edtDir->copy_label(u.data.c_str());
                edtDir->redraw();
                break;
            }
            case WID_FileIndex:
            {
                edtFileIndex->copy_label(u.data.c_str());
                edtFileIndex->redraw();
                break;
            }
            case WID_FileCount:
            {
                edtFileCount->copy_label(u.data.c_str());
                edtFileCount->redraw();
                break;
            }
            case WID_Processed:
            {
                edtProcessed->copy_label(u.data.c_str());
                edtProcessed->redraw();
                break;
            }
            case WID_Compressed:
            {
                edtCompressed->copy_label(u.data.c_str());
                edtCompressed->redraw();
                break;
            }
            case WID_CompressRatio:
            {
                edtCompressRatio->copy_label(u.data.c_str());
                edtCompressRatio->redraw();
                break;
            }
            default:
            {
                DE_ERROR("Missed WidgetId ",int(u.id))
                break;
            }
        }
    }


    // Update mechanism 2: Front & BackBuffer
    std::atomic<int> front;
    UI_Update2 buffers[2];

    UI()
    {
        front.store(0, std::memory_order_relaxed);
    }

    // Worker: Back-Buffer referenzieren
    UI_Update2& back() {
        int fi = front.load(std::memory_order_relaxed);
        return buffers[1 - fi];
    }

    // Worker: nach dem Schreiben aufwecken
    void notify() {
        Fl::awake((void*)MAGIC_GUI);
    }

    void applyUpdate2(const UI_Update2& u)
    {
        bool bRedraw = false;

        // DE_BENNI("u.id(", u.id, "), data(",u.data,")")
        if (u.Progress)
        {
            progressBar->value(u.Progress.value());
            //progressBar->redraw();
            bRedraw = true;
        }
        if (u.File)
        {
            edtFile->copy_label(u.File.value().c_str());
            //edtFile->redraw();
            bRedraw = true;
        }
        if (u.Dir)
        {
            edtDir->copy_label(u.Dir.value().c_str());
            //edtDir->redraw();
            bRedraw = true;
        }
        if (u.FileIndex)
        {
            auto s = std::to_string(u.FileIndex.value());
            edtFileIndex->copy_label(s.c_str());
            //edtFileIndex->redraw();
            bRedraw = true;
        }
        if (u.FileCount)
        {
            auto s = std::to_string(u.FileCount.value());
            edtFileCount->copy_label(s.c_str());
            //edtFileCount->redraw();
            bRedraw = true;
        }
        if (u.TotalBytes)
        {
            auto s = dbStrBytes(u.TotalBytes.value());
            edtTotalBytes->copy_label(s.c_str());
            //edtProcessed->redraw();
            bRedraw = true;
        }
        if (u.Processed)
        {
            auto s = dbStrBytes(u.Processed.value());
            edtProcessed->copy_label(s.c_str());
            //edtProcessed->redraw();
            bRedraw = true;
        }
        if (u.Compressed)
        {
            auto s = dbStrBytes(u.Compressed.value());
            edtCompressed->copy_label(s.c_str());
            //edtCompressed->redraw();
            bRedraw = true;
        }
        if (u.CompressRatio)
        {
            auto s = std::to_string(int(u.CompressRatio.value())) + " %";
            edtCompressRatio->copy_label(s.c_str());
            //edtCompressRatio->redraw();
            bRedraw = true;
        }

        if (bRedraw) window->redraw();
    }

    void clear(UI_Update2& u)
    {
        u.Progress.reset();
        u.File.reset();
        u.Dir.reset();
        u.FileIndex.reset();
        u.FileCount.reset();
        u.Processed.reset();
        u.Compressed.reset();
        u.CompressRatio.reset();
    }

    // Mainthread: Flip + Anwenden
    void dispatch2()
    {
        int oldFront = front.load(std::memory_order_relaxed);
        int newFront = 1 - oldFront;

        front.store(newFront, std::memory_order_release);

        auto& buf = buffers[newFront];
        applyUpdate2(buf);
        clear(buf);
    }
    /*
    void awakeHandler2(void* payload) {
        uintptr_t magic = (uintptr_t)payload;
        if (magic == MAGIC_GUI && g_dispatcher)
            g_dispatcher->dispatch();
    }*/

    // 🟧Anwendung im Worker‑Thread
    /*
    void workerThread(FixedDispatcher* d)
    {
        while (true) {
            GuiUpdate& b = d->back();

            b.slider1 = GuiUpdateItem{0.25f, nullptr};
            b.slider2 = GuiUpdateItem{0.75f, nullptr};
            b.label1  = GuiUpdateItem{0.0f, "Hallo Benjamin"};
            b.label2  = GuiUpdateItem{0.0f, "Neuer Wert"};
            b.label3  = GuiUpdateItem{0.0f, "Noch ein Wert"};

            d->notify();

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    */
#endif
};

static UI ui;

// Awake handler
void awake_poll_update(void* payload)
{
    // DE_BENNI("")
    ui.pollGuiUpdate();
    Fl::repeat_timeout(0.01, awake_poll_update); // wiederholen
}

#if 0
// Awake handler
void awake_update_gui(void* payload)
{
    // DE_BENNI("")
    auto caller = static_cast<UI*>(payload);
    caller->dispatchUpdates();
}

// Awake handler
void awake_update_gui2(void* payload)
{
    // DE_BENNI("")
    auto caller = static_cast<UI*>(payload);
    caller->dispatch2();
}
#endif

// Awake handler
void awake_exit_gui(void* payload)
{
    DE_BENNI("Exit Program from Thread ",std::this_thread::get_id())
    auto caller = static_cast<UI*>(payload);
    ui.window->hide();
}

/*
// 🟧
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
*/


// ---------------- callbacks ----------------
static void pause_cb(Fl_Widget*, void*)
{
    ui.bPauseFlag = !ui.bPauseFlag;
}

// ---------------- callbacks ----------------
static void cancel_cb(Fl_Widget*, void*)
{
    if (ui.bCancelFlag)
    {
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

    if (r == 0) /* Cancel operation */
    {
        DE_OK("Pressed Cancel")
        ui.bCancelFlag = true;
    }
    else if (r == 1) /* Abort */
    {
        DE_OK("Pressed Abort")
    }
}

static void finish_cb(void*)
{
    DE_OK("Finish callback from MainThread ",std::this_thread::get_id())

    Fl::remove_timeout(awake_poll_update);
}

// ---------------- worker ----------------
static void async_compress_worker_demo()
{
    const double timeStart = dbTimeInSeconds();
    // StateMachine2<double> Progress;
    // StateMachine2<std::string> File;
    // StateMachine2<std::string> Dir;
    // StateMachine2<uint64_t> FileIndex;
    // StateMachine2<uint64_t> FileCount;
    // StateMachine2<uint64_t> TotalBytes;
    // StateMachine2<uint64_t> Processed;
    // StateMachine2<uint64_t> Compressed;
    // StateMachine2<double> CompressRatio;

    if (ui.bRunFlag)
    {
        DE_ERROR("Worker already running, abort")
        return; // Already running!
    }
    ui.bRunFlag = true;
    ui.bCancelFlag = false;

    DE_BENNI("Begin Worker Thread ",std::this_thread::get_id())

#if 0
    UI_Update2& b = ui.back();
    b.Progress = 0.0;
    Fl::awake(awake_update_gui2,&ui);
    //ui.enqueueUpdate({ WID_Progress, "0.0" });
    //Fl::awake(awake_update_gui,&ui);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
#else
    #if 0
    ui.pollProgress.workerWrite(0.0);
    #else
    ui.pollProgress = 0.0;
    #endif
#endif


    // FileNamesA fileNames;
    // for (int i = 0; i < 1000; i++)
    // {
    //     fileNames.emplace_back(dbStr("File",i,".txt"));
    // }

    std::wstring exeDir = Util::getExeDir();
    FileInfos fileInfos;

    DE_BENNI("exeDir = ", de_mbstr(exeDir))
    scanDirectory(fileInfos,exeDir,true);
    DUMP(fileInfos);

#if 0
    // ui.enqueueUpdate({ WID_FileIndex, "0" });
    // ui.enqueueUpdate({ WID_FileCount, std::to_string(fileNames.size()) });
    // ui.requestUpdate();
    // Fl::awake(awake_update_gui,&ui);
    {
        UI_Update2& b = ui.back();
        b.Progress = 0.0;
        b.FileCount = fileInfos.size();
        b.TotalBytes = SUM(fileInfos);
        Fl::awake(awake_update_gui2,&ui);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
#else
    uint64_t totalBytes = SUM(fileInfos);
    #if 0
    ui.pollProgress.workerWrite(0.01);
    ui.pollFileCount.workerWrite(fileInfos.size());
    ui.pollTotalBytes.workerWrite(totalBytes);
    #else
    ui.pollProgress = 0.01;
    ui.pollFileCount = fileInfos.size();
    ui.pollTotalBytes = totalBytes;
    #endif
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif

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

    dbRandomize();

    int numItersBeforeSleep = 10;
    int curIters = 0;

    for (uint64_t i = 0; i < fileInfos.size(); ++i)
    {
        const auto& fileInfo = fileInfos[i];
        progress = 0.01 + (0.98*double(i+1) / double(fileInfos.size()));
        curFile = de_mbstr(fileInfos[i].fileName());
        curDir = de_mbstr(fileInfos[i].dir());
        fileIndex = i+1;
        processedBytes += fileInfo.fileSize();
        compressedBytes += fileInfo.fileSize() / (1+(dbRND() % 100));
        compressRatio = double(compressedBytes) / double(processedBytes);

        timeElapsed = dbTimeInSeconds() - timeStart;
        speed = double(processedBytes) / timeElapsed;
        timeRemain = double(totalBytes - processedBytes) / speed;  // v = s/t -> t = s / v
#if 0
        {
            UI_Update2& b = ui.back();
            b.Progress = progress;
            b.File = curFile;
            b.Dir = curDir;
            b.FileIndex = fileIndex;
            b.Processed = processedBytes;
            b.Compressed = compressedBytes;
            b.CompressRatio = compressRatio;
            Fl::awake(awake_update_gui2,&ui);
            //std::this_thread::yield();
        }
#else
        #if 0
        ui.pollFileCount.workerWrite(fileInfos.size());
        ui.pollTotalBytes.workerWrite(totalBytes);
        ui.pollProgress.workerWrite(progress);
        ui.pollFile.workerWrite(curFile);
        ui.pollDir.workerWrite(curDir);
        ui.pollFileIndex.workerWrite(fileIndex);
        ui.pollProcessed.workerWrite(processedBytes);
        ui.pollCompressed.workerWrite(compressedBytes);
        ui.pollCompressRatio.workerWrite(compressRatio);
        #else
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
        #endif
#endif

        curIters++;
        if (curIters >= numItersBeforeSleep)
        {
            curIters = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
#if 0
    // async_progressBar(100);
    ui.enqueueUpdate({ WID_Progress, "1.0" });
    ui.requestUpdate();
    Fl::awake(awake_update_gui,&ui);
#else
    ui.pollProgress = 1.0;
#endif

    ui.bRunFlag = false;

    Fl::awake(finish_cb,&ui);

    DE_BENNI("End Worker Thread ",std::this_thread::get_id())
    Fl::awake(awake_exit_gui,&ui);
}

static void start_worker_cb(Fl_Widget*, void*)
{
    DE_OK("Start worker from MainThread ",std::this_thread::get_id())
/*
    std::string src_uri = ui.getSrcUri();
    std::string dst_uri = ui.getDstUri();
    if (src_uri == dst_uri)
    {
        DE_ERROR("Src and Dst fileNames must differ! Abort")
        return;
    }

    ui.bitrate = 0; // ui.encoder->getBitrate();
    ui.quality = 5; // ui.encoder->getQuality();
*/

    Fl::add_timeout(0.01, awake_poll_update); // Start polling gui update 10 ms

    ui.worker = std::thread(async_compress_worker_demo);
    ui.worker.detach();
}



/*
Fl_Choice* combo = new Fl_Choice(20, 40, 120, 28);
    combo->add(".tar");
    combo->add(".zst");

    combo->callback([](Fl_Widget* w){
        Fl_Choice* c = (Fl_Choice*)w;
        printf("selected: %s\n", c->mvalue()->label());
    });
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

    ui.btnBackground->callback([](Fl_Widget* w)
    {
        Fl_Choice* c = (Fl_Choice*)w;
        DE_DEBUG("selected: ", c->label())

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
            start_worker_cb(nullptr,nullptr);
        }
    });

    ui.btnPause->callback([](Fl_Widget* w)
    {
        Fl_Choice* c = (Fl_Choice*)w;
        DE_DEBUG("selected: ", c->label())
    });

    ui.btnCancel->callback([](Fl_Widget* w)
    {
        Fl_Choice* c = (Fl_Choice*)w;
        DE_DEBUG("selected: ", c->label())
    });

    end();

    // Fl::add_awake_handler_(awakeHandler, &ui);

    resizable(ui.edtFile);
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


/*

//🟧
class FixedDispatcher
{
public:
    FixedDispatcher(Fl_Slider* s1, Fl_Slider* s2, Fl_Slider* s3,
                    Fl_Box* l1, Fl_Box* l2, Fl_Box* l3)
        : slider1(s1), slider2(s2), slider3(s3),
          label1(l1), label2(l2), label3(l3)
    {
        front.store(0, std::memory_order_relaxed);
    }

    // Worker: Back-Buffer referenzieren
    GuiUpdate& back() {
        int fi = front.load(std::memory_order_relaxed);
        return buffers[1 - fi];
    }

    // Worker: nach dem Schreiben aufwecken
    void notify() {
        Fl::awake((void*)MAGIC_GUI);
    }

    // Mainthread: Flip + Anwenden
    void dispatch() {
        int oldFront = front.load(std::memory_order_relaxed);
        int newFront = 1 - oldFront;

        front.store(newFront, std::memory_order_release);

        auto& buf = buffers[newFront];
        apply(buf);
        clear(buf);
    }

private:
    std::atomic<int> front;
    GuiUpdate buffers[2];

    Fl_Slider* slider1;
    Fl_Slider* slider2;
    Fl_Slider* slider3;
    Fl_Box*    label1;
    Fl_Box*    label2;
    Fl_Box*    label3;

    void apply(const GuiUpdate& u) {
        if (u.slider1) { slider1->value(u.slider1->value); slider1->redraw(); }
        if (u.slider2) { slider2->value(u.slider2->value); slider2->redraw(); }
        if (u.slider3) { slider3->value(u.slider3->value); slider3->redraw(); }

        if (u.label1) { label1->copy_label(u.label1->text); label1->redraw(); }
        if (u.label2) { label2->copy_label(u.label2->text); label2->redraw(); }
        if (u.label3) { label3->copy_label(u.label3->text); label3->redraw(); }
    }

    void clear(GuiUpdate& u) {
        u.slider1.reset();
        u.slider2.reset();
        u.slider3.reset();
        u.label1.reset();
        u.label2.reset();
        u.label3.reset();
    }
};

// 🟧Awake‑Handler mit MAGIC‑Routing

FixedDispatcher* g_dispatcher = nullptr;

void awakeHandler(void* payload) {
    uintptr_t magic = (uintptr_t)payload;
    if (magic == MAGIC_GUI && g_dispatcher)
        g_dispatcher->dispatch();
}

// 🟧Anwendung im Worker‑Thread

void workerThread(FixedDispatcher* d)
{
    while (true) {
        GuiUpdate& b = d->back();

        b.slider1 = GuiUpdateItem{0.25f, nullptr};
        b.slider2 = GuiUpdateItem{0.75f, nullptr};
        b.label1  = GuiUpdateItem{0.0f, "Hallo Benjamin"};
        b.label2  = GuiUpdateItem{0.0f, "Neuer Wert"};
        b.label3  = GuiUpdateItem{0.0f, "Noch ein Wert"};

        d->notify();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

🟧Main‑Setup

int main() {
    Fl_Window* win = new Fl_Window(400, 300);

    Fl_Slider* s1 = new Fl_Slider(50, 50, 300, 30);
    Fl_Slider* s2 = new Fl_Slider(50, 100, 300, 30);
    Fl_Slider* s3 = new Fl_Slider(50, 150, 300, 30);

    Fl_Box* l1 = new Fl_Box(50, 200, 300, 30, "Label 1");
    Fl_Box* l2 = new Fl_Box(50, 240, 300, 30, "Label 2");
    Fl_Box* l3 = new Fl_Box(50, 280, 300, 30, "Label 3");

    FixedDispatcher dispatcher(s1, s2, s3, l1, l2, l3);
    g_dispatcher = &dispatcher;

    Fl::add_awake_handler(awakeHandler);

    std::thread worker(workerThread, &dispatcher);
    worker.detach();

    win->end();
    win->show();

    return Fl::run();
}
*/
