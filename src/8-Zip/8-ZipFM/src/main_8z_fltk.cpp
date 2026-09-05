#include "8z_App.h"
#include "8z_Builder.h"
#include "8z_FM.h"
#include "8z_Worker.h"
#include "8z_Install.h"
#include "8z_ArgParser.h"
#include <de/win32/win32_LongPath.h>
#include <gui/fltk_CustomFont.h>

const std::string sTitle = dbStr("8-ZipFM | 2026 (c) by benjaminhampe@gmx.de");
const int w = 600;
const int h = 600;

// static int global_handler(int event);

int main(int argc, char** argv)
{
    App::getInstance()->init(argc,argv);

    Job jobLyra;
    bool okLyra = ArgParser::parseLyra(&jobLyra,argc,argv);
    DE_OK("[Lyra ] ok(",okLyra,"), job(", jobLyra.str(),")")

    Job job;
    bool ok = ArgParser::parseBenni(&job,argc,argv);
    DE_OK("[Benni] ok(",ok,"), job(", job.str(),")")

    if (job.bUninstall)
    {
        EightZip_Uninstall();
    }
    else if (job.bInstall)
    {
        EightZip_Install();
    }
    else if (job.bCompress || job.bExtract)
    {
        DE_DEBUG("Fl::screen_scaling_supported() = ",Fl::screen_scaling_supported())
        DE_DEBUG("Fl::screen_scale(0) = ",Fl::screen_scale(0))
        DE_DEBUG("Fl::use_high_res_GL() = ",Fl::use_high_res_GL())
        DE_DEBUG("App::getExeFile() = ",App::getInstance()->getExeFileA())
        DE_DEBUG("App::getExeDir() = ",App::getInstance()->getExeDirA())
        DE_DEBUG("de::win32_isRegistryLongPathAware() = ",de::win32_isRegistryLongPathAware())
        DE_DEBUG("de::win32_isProcessLongPathAware() = ",de::win32_isProcessLongPathAware())

        Fl::use_high_res_GL(0);
        Fl::screen_scale(0, 1.5f);
        Fl::visual(FL_RGB);
        Fl::scheme("none");
        //Fl::scheme("gtk+");
        //Fl::scheme("plastic");
        //Fl::scheme("gleam");
        //Fl::scheme("oxy");
        //Fl::set_font(FL_HELVETICA, "Noto Sans");
        //setFontLiberationSansRegular();
        //Fl::set_font(FL_HELVETICA, "DejaVu Sans");
        //Fl::set_font(FL_HELVETICA, "Noto Emoji");
        //Fl::set_font(FL_FREE_FONT, "Noto Emoji");
        // my_widget->labelfont(FL_FREE_FONT);
        // my_widget->labelsize(20);
        // my_widget->label("🔥 Feuer!");
        // Fl::add_handler(global_handler); // Zoom +/- on TitleBar MouseWheel Scrolling

        // int fl_argc = 1;
        // char* fl_argv[1];
        // fl_argv[0] = argv[0];

        if (job.bCompress)
        {
            auto B = new EightZip::builder::Dialog(w, h, sTitle.c_str());
            B->resizable(B);
            set_window_icon_from_resource(B);
            B->show();

            B->setCallback_onOk([&]()
                {
                    auto W = new EightZip::worker::Dialog(w, h, sTitle.c_str());
                    W->resizable(W);
                    set_window_icon_from_resource(W);
                    W->show();
                    B->hide();
                });

            B->setCallback_onCancel([&]()
                {
                    B->hide();
                });
        }
        else
        {
            auto W = new EightZip::worker::Dialog(w, h, sTitle.c_str());
            W->resizable(W);
            set_window_icon_from_resource(W);
            W->show();
        }
        return Fl::run();
    }
    else
    {
        if (job.bGui)
        {
            auto win = new EightZip::FM::MainWindow(w, h, sTitle.c_str());
            win->resizable(win);

            set_window_icon_from_resource(win);
            win->show();

            for (int i = 1; i < argc; ++i)
            {
                win->addUri(argv[i]);
            }
            return Fl::run();
        }
        else
        {
            DE_ERROR("Nothing todo. Abort program.")
            return 0;
        }
    }

    return 0;
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
