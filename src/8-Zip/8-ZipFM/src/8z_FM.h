#pragma once
#include <gui/FM/FM.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>

namespace EightZip {
namespace FM {

// =============================================================
class MainWindow : public Fl_Window
// =============================================================
{
public:
    MainWindow(int W, int H, const char* title);

    void resize(int X, int Y, int W, int H) override;

    void addUri(const std::string& uri);
};


} // end namespace FM.
} // end namespace EightZip.


/*
// =============================================================
int main(int argc, char** argv)
// =============================================================
{
    DE_DEBUG("argc = ",argc)
    for (int i = 0; i < argc; ++i)
    {
        DE_DEBUG("argv[",i,"] = ",argv[i])
    }
    DE_DEBUG("Fl::screen_scaling_supported() = ",Fl::screen_scaling_supported())
    DE_DEBUG("Fl::screen_scale(0) = ",Fl::screen_scale(0))
    DE_DEBUG("Fl::use_high_res_GL() = ",Fl::use_high_res_GL())

    Fl::use_high_res_GL(0);
    Fl::screen_scale(0, 1.5f);
    Fl::visual(FL_RGB);
    //Fl::set_font(FL_HELVETICA, "DejaVu Sans");
    Fl::set_font(FL_HELVETICA, "Noto Sans");
    // Fl::set_font(FL_HELVETICA, "Noto Emoji");
    // Fl::set_font(FL_FREE_FONT, "Noto Emoji");
    // my_widget->labelfont(FL_FREE_FONT);
    // my_widget->labelsize(20);
    // my_widget->label("🔥 Feuer!");

    //Fl::scheme("gtk+");
    //Fl::scheme("plastic");
    //Fl::scheme("gleam");
    //Fl::scheme("oxy");
    Fl::scheme("none");

    Fl::add_handler(global_handler); // Zoom +/- on TitleBar MouseWheel Scrolling

    const int w = 600;
    const int h = 600;

    auto sTitle = dbStr("8-ZipFM | "
                        "2026 (c) by benjaminhampe@gmx.de | ",
                        win32_is_admin() ? "Admin" : "User");
    auto win = new MainWindow(w, h, sTitle.c_str());
    win->resizable(win);

    set_window_icon_from_resource(win);

    // win32_8zip_install();

    int fl_argc = 1;
    char* fl_argv[1];
    fl_argv[0] = argv[0];
    win->show(fl_argc, fl_argv);

    for (int i = 1; i < argc; ++i)
    {
        ui.inList->dropList->addRow(argv[i]);
    }
    // log_debug("Test: log_debug()");
    // log_error("Test: log_error()");
    // log_info("Test: log_info()");
    // log_warn("Test: log_warn()");
    // log_success("Test: log_success()");

    // #ifdef _WIN32
    // SetProcessDPIAware(); // echte Pixel, keine Skalierung
    // #endif

    // #ifdef _WIN32
    // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
    // #endif

    return Fl::run();
}
*/
