#include "8z_ArchiveBuilder.h"
#include "8z_FM.h"
#include "8z_Progress.h"

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

    // Fl::add_handler(global_handler); // Zoom +/- on TitleBar MouseWheel Scrolling

    const int w = 600;
    const int h = 600;

    auto sTitle = dbStr("8-ZipFM | 2026 (c) by benjaminhampe@gmx.de");
    int fl_argc = 1;
    char* fl_argv[1];
    fl_argv[0] = argv[0];


    auto job = new EightZip::compressor::Dialog(w, h, sTitle.c_str());
    job->resizable(job);

    set_window_icon_from_resource(job);
    job->show(fl_argc, fl_argv);

#if 1
    // auto arb = new ArchiveBuilder(w, h, sTitle.c_str());
    // arb->resizable(arb);

    // set_window_icon_from_resource(arb);
    // arb->show(fl_argc, fl_argv);

#else

    auto win = new MainWindow(w, h, sTitle.c_str());
    win->resizable(win);

    set_window_icon_from_resource(win);
    win->show(fl_argc, fl_argv);

    for (int i = 1; i < argc; ++i)
    {
        ui.inList->dropList->addRow(argv[i]);
    }
#endif

    // log_debug("Test: log_debug()");
    // log_error("Test: log_error()");
    // log_info("Test: log_info()");
    // log_warn("Test: log_warn()");
    // log_success("Test: log_success()");

    return Fl::run();
}
