#include "8z_Worker_compressZst.h"
#include "8z_Worker_private.h"
#include "8z_Worker_compress.h"
#include <de/archive/ZstWriter.h>

namespace EightZip {
namespace worker {

void workerThread_CompressZst()
{
    const double timeStart = dbTimeInSeconds();
    double timeElapsed = 0;
    double speed = 0.0;
    //uint64_t totalBytes = TOTAL_FILE_SIZE(ui.fileInfos);
    //uint64_t processedBytes = 0;
    const int maxItersBeforeAbort = 100;

    if (ui.bRunFlag)
    {
        DE_ERROR("Worker thread already running, abort")
        return; // Already running!
    }

    ui.bRunFlag = true;
    ui.bAbortFlag = false;
    ui.bPauseFlag = false;
    ui.pollProgress = 0.0;
    // workerThread_CommonScanInit();
    ui.pollFileIndex = 0;
    ui.pollFileCount = 0;
    ui.pollTotalBytes = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    const std::string zstOutputFile = dbMakePosix( ui.job.uri() );
    DE_BENNI("zstOutputFile = ",zstOutputFile)

    std::string zstInputFile;

    uint64_t num_files;
    uint64_t num_dirs;
    uint64_t num_bytes;

    if ((ui.job.filesIn.size() == 1) &&
        (ui.job.filesIn[0].isFile()))
    {
        const auto& singleFile = ui.job.filesIn[0];
        zstInputFile = singleFile.uriA();
        DE_BENNI("zstInputFile = ",zstInputFile)

        num_dirs = 0;
        num_files = 1;
        num_bytes = singleFile.fileSize();

        ui.pollFileIndex = num_files;
        ui.pollFileCount = num_files;
        ui.pollDirIndex = num_dirs;
        ui.pollDirCount = num_dirs;
        ui.pollTotalBytes = num_bytes;
        ui.pollProcessed = num_bytes;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    else
    {
        zstInputFile = dbStrRemoveEnd(zstOutputFile,".zst");
        DE_BENNI("zstInputFile = ",zstInputFile)

        // ScanDirectories:
        const double timeScanBeg = dbTimeInSeconds();
        ui.fileInfos.clear();
        for (size_t i = 0; i < ui.job.filesIn.size(); ++i)
        {
            const auto& fileInfo = ui.job.filesIn[i];
            ui.fileInfos.emplace_back( fileInfo );

            if (fileInfo.isDir())
            {
                de::ScanDirectory(ui.fileInfos,fileInfo.uri(),true);
            }
        }

        num_dirs = NUM_DIRECTORIES(ui.fileInfos);
        num_files = NUM_FILES(ui.fileInfos);
        num_bytes = de::TOTAL_FILE_SIZE(ui.fileInfos);

        //<poll>
        {
            const double timeScanEnd = dbTimeInSeconds();
            const auto t = dbStrSeconds(timeScanEnd - timeScanBeg);
            const auto s = dbStr("[scan] Needed ",t,", "
                            "items(",ui.fileInfos.size(),"), "
                            "files(",num_files,"), "
                            "dirs(",num_dirs,"), "
                            "bytes(",num_bytes,")");
            async_log_ok(s);
        }
        ui.pollFileCount = num_files;
        ui.pollDirCount = num_dirs;
        ui.pollTotalBytes = num_bytes;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //</poll>

        //<trace>
        // for (size_t i = 0; i < std::min<size_t>(ui.fileInfos.size(),1000); ++i)
        // {
        //     async_log_trace(ui.fileInfos[i].str().c_str());
        // }
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //</trace>

        if (ui.fileInfos.empty())
        {
            DE_ERROR("No files, abort worker thread ",std::this_thread::get_id())
            ui.bRunFlag = false;
            ui.pollProgress = 1.0;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            Fl::awake(finish_cb,&ui);
            return;
        }

        // ==========================================
        // TAR writer
        // ==========================================
        ui.pollProgress = 0.01;
        auto e1 = dbStr("[tar] Begin "
                        "File(",zstInputFile,"), "
                        "Thread(",std::this_thread::get_id(),")");
        async_log_ok(e1.c_str());
        DE_OK(e1)

        uint64_t fileIndex = 0;
        uint64_t dirIndex = 0;
        const double timeTarBeg = dbTimeInSeconds();

        WriteTarFileSimpleCfg m_tarCfg;
        m_tarCfg.onNextFile =
            [&] (const de::FileInfo& fileInfo)
            {
                if (fileInfo.isFile())
                {
                    fileIndex++;
                }
                if (fileInfo.isDir())
                {
                    dirIndex++;
                }

                ui.pollFileIndex = fileIndex;
                ui.pollDirIndex = dirIndex;
                ui.pollProgress = 0.01 + (0.98*double(fileIndex+1) / double(num_files));
                ui.pollFile = fileInfo.fileNameA();
                ui.pollDir = fileInfo.dirA();
                // processedBytes += fileInfo.fileSize();
                // timeElapsed = dbTimeInSeconds() - timeStart;
                // speed = double(processedBytes) / timeElapsed;
                // ui.pollProcessed = processedBytes;
                // ui.pollSpeed = speed;
                // ui.pollTimeElapsed = timeElapsed;
                // ui.pollTimeRemain = double(totalBytes - processedBytes) / speed;  // v = s/t -> t = s / v
            };

        m_tarCfg.onProcessed =
            [&] (uint64_t processedBytes)
            {
                // ui.pollFileIndex = fileIndex+1;
                // ui.pollProgress = 0.01 + (0.98*double(fileIndex+1) / double(ui.fileInfos.size()));
                // ui.pollFile = de_mbstr(fileInfo.fileName());
                // ui.pollDir = de_mbstr(fileInfo.dir());
                // processedBytes += fileInfo.fileSize();
                // processedBytes = onProcessedBytes;

                const double timeNow = dbTimeInSeconds();
                ui.pollTimeElapsed = timeNow - timeStart;

                const double timeTar = timeNow - timeTarBeg;
                ui.pollProcessed = processedBytes;
                speed = double(processedBytes) / timeTar;
                ui.pollSpeed = speed;
                ui.pollTimeRemain = double(num_bytes - processedBytes) / speed;  // v = s/t -> t = s / v
                ui.pollProgress = double(processedBytes) / double(num_bytes);
            };

        //std::string tarDir = dbMakePosix( ui.job.directory );
        //std::string tmpUri = tarUri + ".tmp";

        //DE_BENNI("TAR Uri = ",tarUri)
        //DE_BENNI("TMP Uri = ",tmpUri)

        WriteTarFileSimple(zstInputFile, m_tarCfg, ui.fileInfos);

        const auto t = dbStrSeconds(dbTimeInSeconds() - timeTarBeg);
        const auto s = dbStr("[tar] Needed ",t,", "
                            "File(",zstInputFile,"), "
                            "Thread(",std::this_thread::get_id(),")");
        async_log_ok(s.c_str());
        DE_OK(s)
    }

    ui.pollProgress = 0.1;
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    { // Log
        auto s = dbStr("[zst] Begin "
                        "File(",zstOutputFile,"), "
                        "Thread(",std::this_thread::get_id(),")");
        async_log_ok(s.c_str());
        DE_OK(s)
    }

    // ==========================================
    // ZST writer
    // ==========================================
    const double timeZstBeg = dbTimeInSeconds();

    ZstCompressFileCfg m_zstCfg;
    m_zstCfg.onProcessed =
        [&] (uint64_t processedBytes, uint64_t compressedBytes)
        {
            const double timeNow = dbTimeInSeconds();
            ui.pollTimeElapsed = timeNow - timeStart;

            const double timeZst = timeNow - timeZstBeg;
            speed = double(processedBytes) / timeZst;
            ui.pollSpeed = speed;
            ui.pollTimeRemain = double(num_bytes - processedBytes) / speed;  // v = s/t -> t = s / v

            ui.pollCompressed = compressedBytes;
            ui.pollCompressRatio = double(compressedBytes) / double(processedBytes);

            ui.pollProgress = double(processedBytes) / double(num_bytes);
        };

    ZstCompressFileSimple(zstInputFile,zstOutputFile,m_zstCfg);

    ui.bRunFlag = false;
    ui.pollProgress = 1.0;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    { // Log
        const auto t = dbStrSeconds(dbTimeInSeconds() - timeZstBeg);
        const auto s = dbStr("[zst] Needed ",t,", "
                            "File(",zstOutputFile,"), "
                            "Thread(",std::this_thread::get_id(),")");
        async_log_ok(s.c_str());
        DE_OK(s)
    }

    Fl::awake(finish_cb,&ui);
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
