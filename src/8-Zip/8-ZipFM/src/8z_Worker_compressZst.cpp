#include "8z_Worker_compressZst.h"
#include "8z_Worker_private.h"
#include "8z_Worker_compress.h"
#include <de/archive/ZstWriter.h>

namespace EightZip {
namespace worker {

void workerThread_CompressZst()
{
    if (ui.bRunFlag)
    {
        DE_ERROR("Worker thread already running, abort")
        return; // Already running!
    }

    ui.bRunFlag = true;
    ui.bAbortFlag = false;
    ui.bPauseFlag = false;
    ui.pollProgress = 0.0;

    workerThread_CommonScanInit();

    if (ui.fileInfos.empty())
    {
        DE_ERROR("No files, abort worker thread ",std::this_thread::get_id())
        ui.bRunFlag = false;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Fl::awake(compress_finish_cb,&ui);
        return;
    }

    ui.pollProgress = 0.01;

    DE_BENNI("Begin [zst] compression workerThread ",std::this_thread::get_id())

    const double timeStart = dbTimeInSeconds();

    std::string tarUri = dbMakePosix( ui.job.uri() );
    DE_BENNI("URI ",tarUri)

    std::string tarBaseName = dbFileBase(tarUri);
    DE_BENNI("TAR ArchiveBaseName ",tarBaseName)

    std::string tarDir = dbMakePosix( ui.job.directory );
    //std::wstring tarBaseName = de_wstr(ui.job.baseName);
    //std::wstring tarDir = de_wstr( ui.job.baseDir ); // exeDir + L"\\" + tarBaseName;


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
            ui.pollTimeElapsed = timeElapsed;
            ui.pollTimeRemain = double(totalBytes - processedBytes) / speed;  // v = s/t -> t = s / v
        };

    m_tarWriterCfg.onProcessed =
        [&] (const uint64_t compressedBytes)
        {
            ui.pollCompressed += compressedBytes;
            ui.pollCompressRatio = double(compressedBytes) / double(processedBytes);
        };

    ZstWriter::Cfg m_zstWriterCfg;
    m_zstWriterCfg.compressionLevel = ui.job.iPreset;
    m_zstWriterCfg.onProcessed =
        [&] (const uint64_t compressedBytes)
        {
            ui.pollCompressed += compressedBytes;
            ui.pollCompressRatio = double(compressedBytes) / double(processedBytes);
        };

    TarWriter m_tarWriter;
    //ZstWriter m_zstWriter;
    de::File m_outFile;

    de::Blob m_blob(64*1024*1024); // 16MB WorkBuffer

    if (!m_tarWriter.init(m_tarWriterCfg))
    {
        DE_ERROR("Failed TarWriter config, abort")
        ui.bRunFlag = false;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        m_outFile.close();
        DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
        Fl::awake(compress_finish_cb,&ui);
    }

    // if (!m_zstWriter.init(m_zstWriterCfg, &m_tarWriter))
    // {
    //     DE_ERROR("Failed ZstWriter config, abort")
    //     ui.bRunFlag = false;
    //     ui.pollProgress = 1.0;
    //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
    //     m_outFile.close();
    //     DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
    //     Fl::awake(compress_finish_cb,&ui);
    // }

    if (!m_outFile.open(tarUri,de::eFileMode::Write))
    {
        DE_ERROR("Cannot write file. ",tarUri)
        ui.bRunFlag = false;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        m_outFile.close();
        DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
        Fl::awake(compress_finish_cb,&ui);
    }

    // </TAR_Writer>

    while (ui.bRunFlag)
    {
        if (ui.bAbortFlag)
        {
            DE_ERROR("Abort ThreadLoop")
            break;
        }

        if (ui.bPauseFlag)
        {
            std::this_thread::yield();
            continue;
        }

        // <ZST_Writer>
        uint64_t gotBytes = 0;
        int curIters = 0;
        while (gotBytes == 0 && curIters < maxItersBeforeAbort)
        {
            gotBytes = m_tarWriter.process( m_blob.data(), m_blob.size() );
            curIters++;
        }

        if (curIters >= maxItersBeforeAbort)
        {
            DE_ERROR("Data Loss")
        }

        if (gotBytes > 0)
        {
            m_outFile.write(m_blob.data(),gotBytes);
        }
        else // if (curIters >= maxItersBeforeAbort)
        {
            DE_WARN("EOS")
            break;
        }
        // </ZST_Writer>

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ui.bRunFlag = false;
    ui.pollProgress = 1.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m_outFile.close();
    m_tarWriter.close();

    // </ZST_Writer>
    DE_BENNI("End [tar] creation")

    ui.pollProgress = 0.5;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bool ok = zst_compress_file_mt(
        tarUri,
        tarUri+".zst",
        m_zstWriterCfg.compressionLevel,
        m_zstWriterCfg.num_threads);

    ui.bRunFlag = false;
    ui.pollProgress = 1.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
    Fl::awake(compress_finish_cb,&ui);
}

} // end namespace worker.
} // end namespace EightZip.

#if 0


void workerThread_CompressZst()
{
    if (ui.bRunFlag)
    {
        DE_ERROR("Worker thread already running, abort")
        return; // Already running!
    }

    ui.bRunFlag = true;
    ui.bAbortFlag = false;
    ui.bPauseFlag = false;
    ui.pollProgress = 0.0;

    workerThread_CommonScanInit();

    if (ui.fileInfos.empty())
    {
        DE_ERROR("No files, abort worker thread ",std::this_thread::get_id())
        ui.bRunFlag = false;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Fl::awake(compress_finish_cb,&ui);
        return;
    }

    ui.pollProgress = 0.01;

    DE_BENNI("Begin [zst] compression workerThread ",std::this_thread::get_id())

    const double timeStart = dbTimeInSeconds();

    std::string tarUri = dbMakePosix( ui.job.uri() );
    DE_BENNI("URI ",tarUri)

    std::string tarBaseName = dbFileBase(tarUri);
    DE_BENNI("TAR ArchiveBaseName ",tarBaseName)

    std::string tarDir = dbMakePosix( ui.job.directory );
    //std::wstring tarBaseName = de_wstr(ui.job.baseName);
    //std::wstring tarDir = de_wstr( ui.job.baseDir ); // exeDir + L"\\" + tarBaseName;


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
            ui.pollTimeElapsed = timeElapsed;
            ui.pollTimeRemain = double(totalBytes - processedBytes) / speed;  // v = s/t -> t = s / v
        };

    ZstWriter::Cfg m_zstWriterCfg;
    m_zstWriterCfg.compressionLevel = ui.job.iPreset;
    m_zstWriterCfg.onProcessed =
        [&] (const uint64_t compressedBytes)
        {
            ui.pollCompressed += compressedBytes;
            ui.pollCompressRatio = double(compressedBytes) / double(processedBytes);
        };

    TarWriter m_tarWriter;
    ZstWriter m_zstWriter;
    de::File m_outFile;

    de::Blob m_blob(16*1024*1024); // 16MB WorkBuffer

    if (!m_tarWriter.configure(m_tarWriterCfg))
    {
        DE_ERROR("Failed TarWriter config, abort")
        ui.bRunFlag = false;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        m_outFile.close();
        DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
        Fl::awake(compress_finish_cb,&ui);
    }

    if (!m_zstWriter.init(m_zstWriterCfg, &m_tarWriter))
    {
        DE_ERROR("Failed ZstWriter config, abort")
        ui.bRunFlag = false;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        m_outFile.close();
        DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
        Fl::awake(compress_finish_cb,&ui);
    }

    if (!m_outFile.open(tarUri,de::eFileMode::Write))
    {
        DE_ERROR("Cannot write file. ",tarUri)
        ui.bRunFlag = false;
        ui.pollProgress = 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        m_outFile.close();
        DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
        Fl::awake(compress_finish_cb,&ui);
    }

    // </ZST_Writer>

    while (ui.bRunFlag)
    {
        if (ui.bAbortFlag)
        {
            DE_ERROR("Abort ThreadLoop")
            break;
        }

        if (ui.bPauseFlag)
        {
            std::this_thread::yield();
            continue;
        }

        // <ZST_Writer>
        uint64_t gotBytes = 0;
        int curIters = 0;
        while (gotBytes == 0 && curIters < maxItersBeforeAbort)
        {
            gotBytes = m_zstWriter.process( m_blob.data(), m_blob.size() );
            curIters++;
        }

        if (curIters >= maxItersBeforeAbort)
        {
            DE_ERROR("Data Loss")
        }

        if (gotBytes > 0)
        {
            m_outFile.write(m_blob.data(),gotBytes);
        }
        else // if (curIters >= maxItersBeforeAbort)
        {
            DE_WARN("EOS")
            break;
        }
        // </ZST_Writer>

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ui.bRunFlag = false;
    ui.pollProgress = 1.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // <ZST_Writer>
    m_zstWriter.close();
    //m_tarWriter.close();
    m_outFile.close();
    // </ZST_Writer>
    DE_BENNI("End [zst] compression workerThread ",std::this_thread::get_id())
    Fl::awake(compress_finish_cb,&ui);
}

#endif
