#include "MainWindow.h"
#include <de/SetWindowIcon.h>
#include "../res/resource.h"

int main(int argc, char** argv)
{
    // volatile AutoCoInitialize autoCOM;

    if (!App::getInstance()->parseCommandLine(argc,argv))
    {
        return 0;
    }

    auto title = App::getInstance()->getTitle();

    // DE_DEBUG("Fl::screen_scaling_supported() = ",Fl::screen_scaling_supported())
    // DE_DEBUG("Fl::screen_scale(0) = ",Fl::screen_scale(0))
    // DE_DEBUG("Fl::use_high_res_GL() = ",Fl::use_high_res_GL())
    // DE_DEBUG("App::getExeFile() = ",App::getInstance()->getExeFileA())
    // DE_DEBUG("App::getExeDir() = ",App::getInstance()->getExeDirA())
    // DE_DEBUG("de::win32_isRegistryLongPathAware() = ",de::win32_isRegistryLongPathAware())
    // DE_DEBUG("de::win32_isProcessLongPathAware() = ",de::win32_isProcessLongPathAware())

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

    MainWindow win(800, 600);
    win.resizable(win);
    SetWindowIcon_from_resource(&win,aaaa);
    win.label(title.c_str());
    win.show();
    // win.show(argc, argv);

    return Fl::run();
}
