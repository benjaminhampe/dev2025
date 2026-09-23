#include "8z_Worker_compress.h"
#include "8z_Worker_private.h"

namespace EightZip {
namespace worker {

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
        const auto& fileInfo = ui.job.filesIn[i];
        ui.fileInfos.emplace_back( fileInfo );

        if (fileInfo.isDir())
        {
            de::ScanDirectory(ui.fileInfos,fileInfo.uri(),true);
        }
    }

    const uint64_t num_files = NUM_FILES(ui.fileInfos);
    const uint64_t num_dirs = NUM_DIRECTORIES(ui.fileInfos);
    const uint64_t num_bytes = de::TOTAL_FILE_SIZE(ui.fileInfos);

    ui.pollFileCount = num_files;
    ui.pollTotalBytes = num_bytes;

    // DE_TRACE("[2.4]")

    //<debug>

    const double timeScanEnd = dbTimeInSeconds();
    const auto t = dbStrSeconds(timeScanEnd - timeScanBeg);
    const auto s = dbStr("[Scan] Needed ",t,", "
                    "items(",ui.fileInfos.size(),"), "
                    "files(",num_files,"), "
                    "dirs(",num_dirs,"), "
                    "bytes(",num_bytes,")");
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

    // DE_TRACE("[2.6]")
}

} // end namespace worker.
} // end namespace EightZip.
