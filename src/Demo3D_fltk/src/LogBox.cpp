#include "LogBox.h"
#include <string>
#include "App.h"

static void toggle_double(Fl_Widget* wid, void *data)
{
    static bool doublebuff = true;
    doublebuff = !doublebuff;

    Canvas* canvas = (Canvas*)data;
    int flags = canvas->mode();

    if (doublebuff)
        flags |= FL_DOUBLE;
    else
        flags &= ~FL_DOUBLE;

    canvas->reset();
    canvas->hide();
    canvas->mode(flags);
    canvas->show();
}

// =============================================================
LogBox::LogBox(int X, int Y, int W, int H)
    : Fl_Group(X, Y, W, H)
{
    begin();

    m_btnDoubBuf = new Fl_Light_Button(0,0,W,20, "Double-Buffered");
    m_textDisplay = new Fl_Text_Display(0,20,W,H-20);

    end(); // wichtig

    m_textBuffer   = new Fl_Text_Buffer();
    m_styleBuffer = new Fl_Text_Buffer();

    m_textDisplay->buffer(m_textBuffer);

    // Style table: jeder char in stylebuf → Style-Index
    static const Fl_Text_Display::Style_Table_Entry m_logStyles[] = {
        { FL_BLACK, FL_COURIER, 14 },       // 'A' = info
        { FL_RED,   FL_COURIER_BOLD, 14 },  // 'B' = error
        { FL_BLUE,  FL_COURIER, 14 },       // 'C' = debug
        { FL_MAGENTA,FL_COURIER, 14 },       // 'D' = warn
        { FL_DARK_GREEN, FL_COURIER_BOLD, 14 },  // 'E' = success
    };
    m_textDisplay->highlight_data(
        m_styleBuffer,
        m_logStyles,
        sizeof(m_logStyles)/sizeof(m_logStyles[0]),
        'A',   // Default style
        nullptr, nullptr
    );

    auto & ui = App::getInstance()->ui;


    m_btnDoubBuf->callback(toggle_double);
    m_btnDoubBuf->user_data(ui.canvas);
    m_btnDoubBuf->value(1);


}

void LogBox::resize(int X, int Y, int W, int H)
{
    Fl_Group::resize(X,Y,W,H);

    const float zoom = Fl::screen_scale(0);

    const int hB = zoom * 30; // hButtons
    const int hL = H - hB; // hLogbox

    auto& ui = App::getInstance()->ui;

    m_btnDoubBuf->resize(X,Y,W,hB);
    m_textDisplay->resize(X,Y+hB,W,hL);
}

// 🟧
void LogBox::log_common(const char* msg, char style)
{
    // Text anhängen
    m_textBuffer->append(msg);
    m_textBuffer->append("\n");

    // Style anhängen (gleiche Länge)
    int len = strlen(msg) + 1;
    std::string s(len, style);
    m_styleBuffer->append(s.c_str());

    m_textDisplay->scroll(m_textBuffer->length(), 0);

    // ui.logbox->buffer()->append(line_buffer);
    // ui.logbox->scroll(10000, 0);
    // ui.logbox->redraw();
}

void LogBox::log_info(const char* msg) { log_common(msg,'A'); }
void LogBox::log_error(const char* msg) { log_common(msg,'B'); }
void LogBox::log_debug(const char* msg) { log_common(msg,'C'); }
void LogBox::log_warn(const char* msg) { log_common(msg,'D'); }
void LogBox::log_ok(const char* msg) { log_common(msg,'E'); }
