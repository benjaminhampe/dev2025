#pragma once
#include <de/win32/win32_Load_Shell_Icon.h>
#include <de/archive/FileNames.h>
#include <de/archive/tar_writer.h>
#include <de/archive/tar_reader.h>
#include <de/archive/zstd_writer.h>
#include <de/archive/zstd_reader.h>
#include <de/win32/install_8-Zip.h>
#include <de/win32/win32_Set_Window_Icon.h>

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <FL/Fl_RGB_Image.H>

#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Native_File_Chooser.H>

#include <FL/fl_draw.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Hold_Browser.H>

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <algorithm>

#include <DarkImage.h>

// #include <windows.h>
// #include <shellapi.h>


// =============================================================
inline void trim(std::string& s)
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
inline void apply_global_font(Fl_Group* g)
{
    for (int i = 0; i < g->children(); ++i)
    {
        Fl_Widget* w = g->child(i);
        w->labelfont(FL_HELVETICA);
        w->labelsize(14);
        if (auto* sub = dynamic_cast<Fl_Group*>(w))
            apply_global_font(sub);
    }
}
// =============================================================
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
// =============================================================
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
