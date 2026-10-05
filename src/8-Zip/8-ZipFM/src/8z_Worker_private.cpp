#include "8z_Worker_private.h"
#include "8z_Worker_compressTar.h"
#include "8z_Worker_compressZst.h"

namespace EightZip {
namespace worker {

UI_Worker ui;

struct LogAsync
{
    int logLevel = de::LogLevel::Info;
    char* msg = nullptr;
    ~LogAsync() { if (msg) { delete msg; } }
};

void log_common_awake(void* data)
{
    auto logAsync = (LogAsync*)data;
    switch(logAsync->logLevel)
    {
        case de::LogLevel::Error: ui.logBox->log_error(logAsync->msg); break;
        case de::LogLevel::Debug: ui.logBox->log_debug(logAsync->msg); break;
        case de::LogLevel::Warn: ui.logBox->log_warn(logAsync->msg); break;
        case de::LogLevel::Ok: ui.logBox->log_success(logAsync->msg); break;
        default: ui.logBox->log_info(logAsync->msg); break;
    }
    delete logAsync;
}

void async_log_common(const std::string& text, int logLevel)
{
    if (text.empty()) return;
    auto logAsync = new LogAsync;
    logAsync->logLevel = logLevel;
    logAsync->msg = new char[text.size()+1];
    std::memcpy(logAsync->msg, text.c_str(), text.size());
    logAsync->msg[text.size()] = '\0';
    Fl::awake(log_common_awake, logAsync);
}

void async_log_trace(const std::string& msg) { async_log_common(msg,de::LogLevel::Info); }
void async_log_debug(const std::string& msg) { async_log_common(msg,de::LogLevel::Debug); }
void async_log_info(const std::string& msg) { async_log_common(msg,de::LogLevel::Info); }
void async_log_warn(const std::string& msg) { async_log_common(msg,de::LogLevel::Warn); }
void async_log_error(const std::string& msg) { async_log_common(msg,de::LogLevel::Error); }
void async_log_ok(const std::string& msg) { async_log_common(msg,de::LogLevel::Ok); }

// ---------------- callback ----------------
void toggle_logbox_cb(Fl_Widget*, void*)
{
    if ( ui.logBox->visible() != 0)
    {
        ui.logBox->hide();
    }
    else
    {
        ui.logBox->show();
    }
}

// ---------------- callback ----------------
void timer_update(void* payload)
{
    ui.pollGuiUpdate();
    Fl::repeat_timeout(0.01, timer_update); // wiederholen
}

// ---------------- callback ----------------
void noop_cb(Fl_Widget*, void*)
{
    // DE_WARN("Not implemented")
}

// ---------------- callback ----------------
void pause_cb(Fl_Widget*, void*)
{
    if (ui.bPauseFlag)
    {
        DE_WARN("Resumed")
        ui.bPauseFlag = false;
        ui.btnPause->label("Pause");
        ui.btnPause->redraw();
        ui.btnCancel->activate();
    }
    else
    {
        DE_WARN("Paused")
        ui.bPauseFlag = true;
        ui.btnPause->label("Resume");
        ui.btnPause->redraw();
        ui.btnCancel->deactivate();
    }
}

// ---------------- callbacks ----------------
void cancel_cb(Fl_Widget*, void*)
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
        "Abort operation",  // Button 0
        "Keep running", // Button 1
        nullptr
    );

    if (r == 0) // Cancel operation
    {
        DE_OK("Pressed Cancel")
        ui.bAbortFlag = true;
        ui.logBox->show();
        ui.btnPause->deactivate();
        ui.btnCancel->deactivate();
    }
    else if (r == 1) // Abort
    {
        DE_OK("Pressed Abort")
        ui.btnPause->activate();
        ui.btnCancel->activate();
    }
}

// ---------------- callback ----------------
void finish_cb(void*)
{
    ui.m_timeFinish = dbTimeInSeconds();

    Fl::remove_timeout(timer_update);

    const double duration = ui.m_timeFinish - ui.m_timeStart;
    const double speed = ui.pollProcessed / duration;

    // FinalSet
    ui.edtTimeCurr->copy_label(dbStrSeconds(duration).c_str()); // Set
    ui.edtSpeed->copy_label(dbStr(dbStrBytes(speed),"/s").c_str()); // Set
    ui.progressBar->value(1.0);

    // Reset
    ui.edtTimeLeft->copy_label("00:00:00");
    ui.btnPause->callback(noop_cb);
    ui.btnCancel->callback(noop_cb);
    ui.btnPause->deactivate();
    ui.btnCancel->deactivate();

    if (ui.bAbortFlag)
    {
        DE_ERROR("Aborted.")
        ui.logBox->log_error("Aborted by user.");
        ui.btnPause->label("Pause");
        ui.btnPause->redraw();
    }
    else
    {
        DE_OK("Finished.")
        if (ui.bAutoCloseWindow)
        {
            ui.window->hide();
        }
    }

    ui.bRunFlag = false;
}

// ---------------- callback ----------------
void start_cb(Fl_Widget*, void*)
{
    ui.m_timeStart = dbTimeInSeconds();

    DE_OK("MainThread ",std::this_thread::get_id())

    if (ui.bRunFlag)
    {
        DE_ERROR("Worker already running.")
        return;
    }

    DE_OK("Start worker from MainThread ",std::this_thread::get_id())

    // ui.timeStartInSec = dbTimeInSeconds();

    Fl::add_timeout(0.01, timer_update); // Start polling gui update 10 ms

    ui.btnPause->callback(pause_cb);
    ui.btnCancel->callback(cancel_cb);
    ui.btnPause->activate();
    ui.btnCancel->activate();

    auto ext = dbFileSuffix(ui.job.fileName);

    if (ui.job.bCompress)
    {
        if (ext == "tar")
        {
            auto e = dbStr("Start [tar] Writer (",ui.job.fileName,")");
            ui.logBox->log_success(e.c_str());

            ui.worker = std::thread(workerThread_CompressTar);
            ui.worker.detach();
        }
        else if (ext == "zst")
        {
            auto e = dbStr("Start [zst] Writer (",ui.job.fileName,")");
            ui.logBox->log_success(e.c_str());

            ui.worker = std::thread(workerThread_CompressZst);
            ui.worker.detach();
        }
        else
        {
            auto e = dbStr("Unsupported [",ext,"] Writer (",ui.job.fileName,")");
            ui.logBox->log_error(e.c_str());
        }
    }
    else if (ui.job.bExtract)
    {
        auto e = dbStr("Unsupported [",ext,"] Reader (",ui.job.fileName,")");
        ui.logBox->log_error(e.c_str());
    }
    else
    {
        auto e = dbStr("Unsupported Job (",ui.job.str(),")");
        ui.logBox->log_error(e.c_str());
    }
}

} // end namespace worker.
} // end namespace EightZip.
