#pragma once
#include "8z_Worker.h"
#include "8z_App.h"
#include "8z_ArgParser.h"

#include <gui/Widgets.h>
#include <gui/XP_ProgressBar.h>
#include <gui/LogBox.h>

#include <de/ScanDirectory.h>

namespace EightZip {
namespace worker {

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

template <typename T>
class DoubleBufferDirty {
public:
    void workerWrite(const T& value)
    {
        int wi = m_writeIndex.load(std::memory_order_relaxed);
        m_buffers[wi] = value;
        m_writeIndex.store(1 - wi, std::memory_order_release);
        m_dirty.store(true, std::memory_order_release);
    }

    DoubleBufferDirty& operator= (const T& value)
    {
        workerWrite(value);
        return *this;
    }

    bool mainReadIfDirty(T& out)
    {
        if (m_dirty.exchange(false))
        {
            int ri = m_writeIndex.load(std::memory_order_acquire);
            out = m_buffers[ri];
            return true;
        }
        return false;
    }

private:
    std::atomic<int> m_writeIndex{0};
    std::atomic<bool> m_dirty{false};
    T m_buffers[2];
};

struct UI_Worker
{
    static constexpr uintptr_t MAGIC_GUI = 0xF1CED123;

    bool bAutoCloseWindow = false; // TODO: Set to true for Release.

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

    de::FileInfos fileInfos;

    // double timeStartInSec = 0.0;   // Dialog tracks elapsed time, not the worker.
    // double timeElapsedInSec = 0.0; // Dialog tracks elapsed time, not the worker.

    volatile std::atomic<bool> bDebug{false};
    volatile std::atomic<bool> bRunFlag{false};
    volatile std::atomic<bool> bAbortFlag{false};
    volatile std::atomic<bool> bPauseFlag{false};
    // volatile std::atomic<bool> reloadFile{true};

    std::thread worker;

    double pollTimeElapsed = 0.0; // In [s]
    double pollTimeRemain = 0.0;  // In [s]
    double pollSpeed = 0.0;       // In [bytes/s]
    double pollProgress = 0.0;
    DoubleBufferDirty<std::string> pollFile;
    DoubleBufferDirty<std::string> pollDir;
    uint64_t pollFileIndex = 0;
    uint64_t pollFileCount = 0;
    uint64_t pollDirIndex = 0;
    uint64_t pollDirCount = 0;
    uint64_t pollTotalBytes = 0;
    uint64_t pollProcessed = 0;
    uint64_t pollCompressed = 0;
    double pollCompressRatio = 0;

    void pollGuiUpdate()
    {

        // bool bRedraw = false;

        // DE_BENNI("u.id(", u.id, "), data(",u.data,")")
        double d;
        std::string s;
        uint64_t u;

        if (pollFile.mainReadIfDirty(s))
        {
            edtFile->copy_label(s.c_str());
            edtFile->redraw();
            //bRedraw = true;
        }
        if (pollDir.mainReadIfDirty(s))
        {
            edtDir->copy_label(s.c_str());
            edtDir->redraw();
            //bRedraw = true;
        }

        // Update GUI worker time
        //timeElapsedInSec = dbTimeInSeconds() - timeStartInSec;

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
        // edtDirIndex->copy_label(std::to_string(pollDirIndex).c_str());
        // edtDirIndex->redraw();
        // edtDirCount->copy_label(std::to_string(pollDirCount).c_str());
        // edtDirCount->redraw();
        edtTotalBytes->copy_label(dbStrBytes(pollTotalBytes).c_str());
        edtTotalBytes->redraw();
        edtProcessed->copy_label(dbStrBytes(pollProcessed).c_str());
        edtProcessed->redraw();
        edtCompressed->copy_label(dbStrBytes(pollCompressed).c_str());
        edtCompressed->redraw();
        edtCompressRatio->copy_label(dbStr(int(100.0 * pollCompressRatio)," %").c_str());
        edtCompressRatio->redraw();
        // bRedraw = true;
        // if (bRedraw) window->redraw();
    }
};

extern UI_Worker ui;

void log_common_awake(void* data);
void async_log_common(const std::string& text, int logLevel);
void async_log_trace(const std::string& msg);
void async_log_debug(const std::string& msg);
void async_log_info(const std::string& msg);
void async_log_warn(const std::string& msg);
void async_log_error(const std::string& msg);
void async_log_ok(const std::string& msg);

// ---------------- callbacks ----------------
void timer_update(void* payload);
void noop_cb(Fl_Widget*, void*);
void pause_cb(Fl_Widget*, void*);
void cancel_cb(Fl_Widget*, void*);
void finish_cb(void*);
void start_cb(Fl_Widget*, void*);

// ---------------- callbacks ----------------
void logbox_cb(Fl_Widget*, void*);
} // end namespace worker.
} // end namespace EightZip.

