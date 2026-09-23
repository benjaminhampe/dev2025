#include "8z_App.h"
#include "8z_Builder.h"
#include "8z_FM.h"
#include "8z_Worker.h"
#include "8z_Install.h"
#include "8z_ArgParser.h"
#include "8z_TarTree.h"
#include <de/win32/win32_Get_Explorer_Selection.h>
#include <de/win32/win32_Set_Window_Icon.h>
#include <de/win32/win32_LongPath.h>
#include <gui/fltk_CustomFont.h>
#include <de/ScanDirectory.h>

const std::string sTitle = dbStr("8-Zip | 2026 (c) by benjaminhampe@gmx.de");
const int w = 600;
const int h = 600;

// static int global_handler(int event);

// ---------------- worker ----------------
void testScanImpl(de::FileInfos & fileInfos, const std::vector<std::string>& filesIn)
{
    const double timeScanBeg = dbTimeInSeconds();

    uint64_t pollFileCount = 0;
    uint64_t pollTotalBytes = 0;

    DE_TRACE("[2.1]")

    fileInfos.clear();

    for (size_t i = 0; i < filesIn.size(); ++i)
    {
        DE_WARN("Scan [",i,"] ",filesIn[i])
        auto optFileInfo = de::ScanFileInfo(de_wstr(filesIn[i]));
        if (optFileInfo)
        {
            const de::FileInfo& fileInfo = *optFileInfo;
            DE_WARN("Add [",i,"] ",fileInfo.str())

            fileInfos.emplace_back( fileInfo );

            if (fileInfo.isDir())
            {
                de::ScanDirectory(fileInfos,fileInfo.uri(),true);
            }

            pollFileCount = fileInfos.size();
            pollTotalBytes = de::TOTAL_FILE_SIZE(fileInfos);
        }
        else
        {
            DE_WARN("Skip [",i,"] ",filesIn[i])
        }
    }

    DE_TRACE("[2.4]")

    //<debug>

    const double timeScanEnd = dbTimeInSeconds();
    const auto t = dbStrSeconds(timeScanEnd - timeScanBeg);
    const auto s = dbStr("[Scan] Needed ",t,", "
                    "fileInfos(",fileInfos.size(),"), "
                    "files(",NUM_FILES(fileInfos),"), "
                    "dirs(",NUM_DIRECTORIES(fileInfos),")");
    //async_log_ok(s);
    DE_OK(s)

    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //</debug>

    DE_TRACE("[2.5]")

    //<trace>
    for (size_t i = 0; i < std::min<size_t>(fileInfos.size(),1000); ++i)
    {
        //async_log_trace(ui.fileInfos[i].str().c_str());
        DE_TRACE("[",i,"] ", fileInfos[i].str())
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //</trace>

    DE_TRACE("[2.6]")
}

void testScan()
{
    de::FileInfos fileInfos;
    std::vector<std::string> filesIn;
    filesIn.emplace_back("C:\\Users\\firestarter\\Downloads\\_3_zst/17000Midis[www.Jwejem.com]");
    testScanImpl(fileInfos,filesIn);
};

int main(int argc, char** argv)
{
    volatile AutoCoInitialize autoCOM;

    if (!App::getInstance()->parseCommandLine(argc,argv))
    {
        return 0;
    }

    // EightZip_Install();
    // EightZip_InstallExePath();

    // testScan();

    const auto& job = App::getInstance()->getJob();
    if (job.bRestartExplorer)
    {
        EightZip_restartExplorer();
    }
    else if (job.bUpdate)
    {
        EightZip_InstallExePath();
    }
    else if (job.bUninstall)
    {
        EightZip_Uninstall();
    }
    else if (job.bInstall)
    {
        EightZip_Install();
        EightZip_InstallExePath();
        // EightZip_restartExplorer();
    }
    else if (job.bCompress || job.bExtract || job.bTarTree)
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
            auto B = new EightZip::builder::Builder(w, h, sTitle.c_str());
            B->setJob(job);
            B->resizable(B);
            set_window_icon_from_resource(B);
            B->show();

            B->setCallback_onOk([&]()
                {
                    Job jobB = B->getJob();
                    auto W = new EightZip::worker::Worker(jobB, w, h, sTitle.c_str());
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
        else if (job.bExtract)
        {
            auto W = new EightZip::worker::Worker(job, w, h, sTitle.c_str());
            W->resizable(W);
            set_window_icon_from_resource(W);
            W->show();
        }
        else if (job.bTarTree)
        {
            std::string uri;
            if (job.filesIn.empty())
            {
                uri = job.filesIn[0].uriA();
            }
            auto W = new TarTree(uri,200,200,600,600);
            W->resizable(W);
            //set_window_icon_from_resource(W);
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
