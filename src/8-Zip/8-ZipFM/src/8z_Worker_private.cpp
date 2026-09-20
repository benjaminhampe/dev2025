#include "8z_Worker_private.h"

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
void awake_poll_update(void* payload)
{
    ui.pollGuiUpdate();
    Fl::repeat_timeout(0.01, awake_poll_update); // wiederholen
}

// ---------------- callback ----------------
void noop_cb(Fl_Widget*, void*)
{
    // DE_WARN("Not implemented")
}

} // end namespace worker.
} // end namespace EightZip.
