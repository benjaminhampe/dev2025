#include "8z_Worker_compressTar.h"
#include "8z_Worker_private.h"
#include "8z_Worker_compress.h"
#include <de/archive/TarWriter.h>

namespace EightZip {
namespace worker {

// ---------------- worker ----------------
void workerThread_CompressTar()
{
    DE_TRACE("[1]")

    if (ui.bRunFlag)
    {
        DE_ERROR("TAR Writer Thread already running, abort")
        return; // Already running!
    }

    ui.bRunFlag = true;
    ui.bAbortFlag = false;
    ui.bPauseFlag = false;
    ui.pollProgress = 0.0;

    // DE_TRACE("[2]")

    workerThread_CommonScanInit();

    // DE_TRACE("[3]")

    if (ui.fileInfos.empty())
    {
        DE_ERROR("Nothing todo, abort.")
        ui.bRunFlag = false;
        ui.bAbortFlag = true;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Fl::awake(finish_cb,&ui);
        return;
    }

    ui.pollProgress = 0.01;

    DE_BENNI("Start [TAR] Writer")

    const double timeStart = dbTimeInSeconds();

    //std::wstring exeDir = App::getInstance()->getExeDirW();
    std::string tarUri = dbMakePosix( ui.job.uri() );
    DE_BENNI("TAR Uri = ",tarUri)

    std::string tarBaseName = dbFileBase(tarUri);
    DE_BENNI("TAR BaseName = ",tarBaseName)

    std::string tarDir = dbMakePosix( ui.job.directory );
    DE_BENNI("TAR Dir = ",tarDir)

    double timeElapsed = 0;
    double speed = 0.0;
    uint64_t totalBytes = TOTAL_FILE_SIZE(ui.fileInfos);
    uint64_t processedBytes = 0;
    const int maxItersBeforeAbort = 100;

    ui.pollProgress = 0.02;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // <TAR_Writer>

    TarWriter::Cfg m_tarWriterCfg;
    m_tarWriterCfg.archiveBaseName = tarBaseName;
    m_tarWriterCfg.baseDir = tarDir;

    m_tarWriterCfg.fileInfos = &ui.fileInfos;
    m_tarWriterCfg.onNextFile =
        [&] (const de::FileInfo& fileInfo, uint32_t fileIndex)
        {
            ui.pollFileIndex = fileIndex+1;
            ui.pollProgress = 0.01 + (0.98*double(fileIndex+1) / double(ui.fileInfos.size()));
            ui.pollFile = de_mbstr(fileInfo.fileName());
            ui.pollDir = de_mbstr(fileInfo.dir());
            processedBytes += fileInfo.fileSize();
            timeElapsed = dbTimeInSeconds() - timeStart;
            speed = double(processedBytes) / timeElapsed;
            ui.pollProcessed = processedBytes;
            ui.pollSpeed = speed;
            // ui.pollTimeElapsed = timeElapsed;
            ui.pollTimeRemain = double(totalBytes - processedBytes) / speed;  // v = s/t -> t = s / v
        };

    m_tarWriterCfg.onProcessed =
        [&] (const uint64_t compressedBytes)
        {
            ui.pollCompressed += compressedBytes;
            ui.pollCompressRatio = double(compressedBytes) / double(processedBytes);
        };

    de::Blob m_tarBuffer(16*1024*1024); // 16MB WorkBuffer
    TarWriter m_tarWriter;
    de::File m_tarFile;

    if (!m_tarWriter.init(m_tarWriterCfg))
    {
        DE_ERROR("No TarWriter config")
        goto _exit_compress_tar_thread;
    }

    if (!m_tarFile.open(tarUri,de::eFileMode::Write))
    {
        DE_ERROR("Cannot write Tar file. ",tarUri)
        goto _exit_compress_tar_thread;
    }

    // </TAR_Writer>

    while (ui.bRunFlag)
    {
        if (ui.bAbortFlag)
        {
            DE_ERROR("Abort Loop.")
            break;
        }

        if (ui.bPauseFlag)
        {
            std::this_thread::yield();
            continue;
        }

        // <TAR_Writer>
        uint64_t gotBytes = 0;
        int curIters = 0;
        while (gotBytes == 0 && curIters < maxItersBeforeAbort)
        {
            gotBytes = m_tarWriter.process( m_tarBuffer.data(), m_tarBuffer.size() );
            curIters++;
        }

        if (gotBytes > 0)
        {
            m_tarFile.write(m_tarBuffer.data(),gotBytes);
        }
        else // if (curIters >= maxItersBeforeAbort)
        {
            DE_WARN("EOS")
            break;
        }
        // </TAR_Writer>

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

_exit_compress_tar_thread:
    ui.bRunFlag = false;
    ui.pollProgress = 1.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    m_tarFile.close();
    DE_BENNI("End Worker Thread")
    Fl::awake(finish_cb,&ui);
}


} // end namespace worker.
} // end namespace EightZip.
