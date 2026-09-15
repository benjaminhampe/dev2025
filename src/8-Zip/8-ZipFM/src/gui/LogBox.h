#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <string>

// =============================================================
class LogBox : public Fl_Group
{
public:
    //int m_spacing = 5;

    Fl_Text_Display* m_logbox;
    Fl_Text_Buffer*  m_logbuf;
    Fl_Text_Buffer*  m_stylebuf;

    LogBox(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        //, m_spacing(spacing)
    {
        begin();


        m_logbuf   = new Fl_Text_Buffer();
        m_stylebuf = new Fl_Text_Buffer();

        m_logbox = new Fl_Text_Display(0,0,W,H);

        m_logbox->buffer(m_logbuf);

        // Style table: jeder char in stylebuf → Style-Index
        static const Fl_Text_Display::Style_Table_Entry m_logStyles[] = {
            { FL_BLACK, FL_COURIER, 14 },       // 'A' = info
            { FL_RED,   FL_COURIER_BOLD, 14 },  // 'B' = error
            { FL_BLUE,  FL_COURIER, 14 },       // 'C' = debug
            { FL_MAGENTA,FL_COURIER, 14 },       // 'D' = warn
            { FL_DARK_GREEN, FL_COURIER_BOLD, 14 },  // 'E' = success
        };
        m_logbox->highlight_data(
            m_stylebuf,
            m_logStyles,
            sizeof(m_logStyles)/sizeof(m_logStyles[0]),
            'A',   // Default style
            nullptr, nullptr
        );

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X,Y,W,H);

        m_logbox->resize(X,Y,W,H);
    }

    // 🟧
    void log_common(const char* msg, char style)
    {
        // Text anhängen
        m_logbuf->append(msg);
        m_logbuf->append("\n");

        // Style anhängen (gleiche Länge)
        int len = strlen(msg) + 1;
        std::string s(len, style);
        m_stylebuf->append(s.c_str());

        m_logbox->scroll(m_logbuf->length(), 0);
    }

    void log_info(const char* msg) { log_common(msg,'A'); }
    void log_error(const char* msg) { log_common(msg,'B'); }
    void log_debug(const char* msg) { log_common(msg,'C'); }
    void log_warn(const char* msg) { log_common(msg,'D'); }
    void log_success(const char* msg) { log_common(msg,'E'); }
};


/*

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

*/
