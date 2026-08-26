#pragma once
#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>

// =============================================================
class LogBox : public Fl_Group
{
public:
    //int m_spacing = 5;

    Fl_Text_Display* logbox;
    Fl_Text_Buffer*  logbuf;
    Fl_Text_Buffer*  stylebuf;

    LogBox(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        //, m_spacing(spacing)
    {
        begin();


        logbuf   = new Fl_Text_Buffer();
        stylebuf = new Fl_Text_Buffer();

        logbox = new Fl_Text_Display(0,0,W,H);

        logbox->buffer(logbuf);

        // Style table: jeder char in stylebuf → Style-Index
        static const Fl_Text_Display::Style_Table_Entry g_logStyles[] = {
            { FL_BLACK, FL_COURIER, 14 },       // 'A' = info
            { FL_RED,   FL_COURIER_BOLD, 14 },  // 'B' = error
            { FL_BLUE,  FL_COURIER, 14 },       // 'C' = debug
            { FL_MAGENTA,FL_COURIER, 14 },       // 'D' = warn
            { FL_DARK_GREEN, FL_COURIER_BOLD, 14 },  // 'E' = success
        };
        logbox->highlight_data(
            stylebuf,
            g_logStyles,
            sizeof(g_logStyles)/sizeof(g_logStyles[0]),
            'A',   // Default style
            nullptr, nullptr
        );

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X,Y,W,H);

        logbox->resize(X,Y,W,H);
    }
};
