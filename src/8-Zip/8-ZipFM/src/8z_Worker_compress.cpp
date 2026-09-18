#include "8z_Worker_compress.h"
#include "8z_Worker_private.h"

namespace EightZip {
namespace worker {

// ---------------- callback ----------------
void compress_finish_cb(void*)
{
    DE_OK("Finish callback from MainThread ",std::this_thread::get_id())

    Fl::remove_timeout(awake_poll_update);

    if (ui.bAbortFlag)
    {
        DE_ERROR("Aborted.")
        ui.logBox->log_error("Aborted by user.");
        ui.btnPause->label("Pause");
        ui.btnPause->redraw();
    }
    else
    {
        DE_BENNI("Exit Program from Thread ",std::this_thread::get_id())
        if (ui.bAutoCloseWindow)
        {
            ui.window->hide();
        }
    }
}

// ---------------- callback ----------------
void compress_pause_cb(Fl_Widget*, void*)
{
    if (ui.bPauseFlag)
    {
        DE_WARN("Resumed")
        ui.bPauseFlag = false;
        ui.btnPause->label("Pause");
        ui.btnPause->redraw();
    }
    else
    {
        DE_WARN("Paused")
        ui.bPauseFlag = true;
        ui.btnPause->label("Resume");
        ui.btnPause->redraw();
    }
}

// ---------------- callbacks ----------------
void compress_cancel_cb(Fl_Widget*, void*)
{
    if (ui.bAbortFlag)
    {
        DE_WARN("Abort already in progress")
        return;
    }

    int r = fl_choice(
        "\n"
        "Do you like cancel the operation?\n"
        "\n",
        "Cancel operation",  // Button 0
        "Abort this dialog", // Button 1
        nullptr
    );

    if (r == 0) // Cancel operation
    {
        DE_OK("Pressed Cancel")
        ui.bAbortFlag = true;
        ui.logBox->show();
    }
    else if (r == 1) // Abort
    {
        DE_OK("Pressed Abort")
    }
}

// ---------------- worker ----------------
void workerThread_CommonScanInit()
{
    DE_TRACE("[2.1]")

    const double timeScanBeg = dbTimeInSeconds();

    ui.pollProgress = 0.0;
    ui.pollFileIndex = 0;
    ui.pollFileCount = 0;
    ui.pollTotalBytes = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    DE_TRACE("[2.2]")

    // ScanDirectories:

    ui.fileInfos.clear();

    DE_TRACE("[2.3]")

    for (size_t i = 0; i < ui.job.filesIn.size(); ++i)
    {
        auto fileInfo = de::ScanFileInfo(de_wstr(ui.job.filesIn[i]));
        if (fileInfo)
        {
            ui.fileInfos.emplace_back( *fileInfo );

            if (fileInfo->isDir())
            {
                de::ScanDirectory(ui.fileInfos,fileInfo->uri(),true);
            }

            ui.pollFileCount = ui.fileInfos.size();
            ui.pollTotalBytes = de::TOTAL_FILE_SIZE(ui.fileInfos);
        }
    }

    DE_TRACE("[2.4]")

    //<debug>

    const double timeScanEnd = dbTimeInSeconds();
    const auto t = dbStrSeconds(timeScanEnd - timeScanBeg);
    const auto s = dbStr("[Scan] Needed ",t,", "
                    "fileInfos(",ui.fileInfos.size(),"), "
                    "files(",NUM_FILES(ui.fileInfos),"), "
                    "dirs(",NUM_DIRECTORIES(ui.fileInfos),")");
    async_log_ok(s);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //</debug>

    // DE_TRACE("[2.5]")

    //<trace>
    // for (size_t i = 0; i < std::min<size_t>(ui.fileInfos.size(),1000); ++i)
    // {
    //     async_log_trace(ui.fileInfos[i].str().c_str());
    // }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //</trace>

    DE_TRACE("[2.6]")
}

} // end namespace worker.
} // end namespace EightZip.
