#include "8z_Builder.h"
#include "8z_FM.h"
#include "8z_Worker.h"
#include "8z_Install.h"
#include "8z_ArgParser.h"

static int global_handler(int event);

int main(int argc, char** argv)
{
//<debug>
    DE_DEBUG("argc = ",argc)
    for (int i = 0; i < argc; ++i)
    {
        DE_DEBUG("argv[",i,"] = ", argv[i])
        // DE_DEBUG("argv[",i,"] = ", de_mbstr(argv[i]))
    }

    DE_DEBUG("Fl::screen_scaling_supported() = ",Fl::screen_scaling_supported())
    DE_DEBUG("Fl::screen_scale(0) = ",Fl::screen_scale(0))
    DE_DEBUG("Fl::use_high_res_GL() = ",Fl::use_high_res_GL())
//</debug>

    Job jobL;
    bool okL = ArgParser::parseLyra(&jobL,argc,argv);
    DE_OK("okL = ", okL)
    DE_OK("jobL = ", jobL.str())

    Job jobB;
    bool okB = ArgParser::parseBenni(&jobB,argc,argv);
    DE_OK("okB = ", okB)
    DE_OK("jobB = ", jobB.str())


    Fl::use_high_res_GL(0);
    Fl::screen_scale(0, 1.5f);
    Fl::visual(FL_RGB);
    Fl::set_font(FL_HELVETICA, "Noto Sans");
    Fl::scheme("none");
    // Fl::set_font(FL_HELVETICA, "DejaVu Sans");
    // Fl::set_font(FL_HELVETICA, "Noto Emoji");
    // Fl::set_font(FL_FREE_FONT, "Noto Emoji");
    //Fl::scheme("gtk+");
    //Fl::scheme("plastic");
    //Fl::scheme("gleam");
    //Fl::scheme("oxy");
    // my_widget->labelfont(FL_FREE_FONT);
    // my_widget->labelsize(20);
    // my_widget->label("🔥 Feuer!");
    Fl::add_handler(global_handler); // Zoom +/- on TitleBar MouseWheel Scrolling



    auto sTitle = dbStr("8-ZipFM | 2026 (c) by benjaminhampe@gmx.de");
    // int fl_argc = 1;
    // char* fl_argv[1];
    // fl_argv[0] = argv[0];



    const int w = 600;
    const int h = 600;
    auto job = new EightZip::worker::Dialog(w, h, sTitle.c_str());
    //job->resizable(job);
    set_window_icon_from_resource(job);
    job->show();
    // job->show(fl_argc, fl_argv);

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


static int global_handler(int event)
{
    if (event == FL_MOUSEWHEEL)
    {
        Fl_Window* win = Fl::first_window();
        if (!win)
        {
            DE_WARN("No first window")
            return 0;
        }

        int mx = Fl::event_x_root();
        int my = Fl::event_y_root();

        int x = win->x();
        int y = win->y();
        int w = win->w();
        int h = win->h();

        // Titlebar height (approx; OS-dependent)
        int title_h = 30; // you can refine this per OS

        bool over_title = dbMouseOver(mx,my,x,y,x+w-1,y+title_h-1);

        if (over_title)
        {
            float zoom = Fl::screen_scale(0);

            if (Fl::event_dy() > 0) // Zoom out
            {
                zoom = std::clamp<double>(zoom - 0.10, 0.5, 2.5);
            }
            else if (Fl::event_dy() < 0) // Zoom In
            {
                zoom = std::clamp<double>(zoom + 0.10, 0.5, 2.5);
            }

            Fl::screen_scale(0,zoom);

            DE_WARN("Zoom(",zoom,"), Mouse(",mx,",",my,"), Window(",x,",",y,",",w,",",h,")")
            return 1; // swallow event
        }
    }
    return 0;
}
