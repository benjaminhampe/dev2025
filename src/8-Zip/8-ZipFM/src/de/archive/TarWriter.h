#pragma once
#include <de/archive/FileInfo.h>
#include <de/archive/TarHeader.h>

struct TarWriter
{
    struct Cfg
    {
        const FileInfos* fileInfos = nullptr;

        std::string baseDir;

        std::string archiveBaseName;

        //int32_t blockSize = 16 * 512; // Should be multiple of 512 bytes, makes tar things easier.

        typedef std::function<void(const FileInfo& /* fileInfo */, uint32_t)> FN_onNextFile;

        FN_onNextFile onNextFile;

        typedef std::function<void(const uint64_t /* byteCount */)> FN_onProcessed;

        FN_onProcessed onProcessed;
    };

    Cfg m_cfg;

    de::File m_file;

    de::Blob m_head; // 129kB (should be enough for full NT Path with 32k utf16 chars = 4*32k utf8 chars + 2*512
    de::Blob m_data;

    uint64_t m_headByte = 0; // num bytes of header written
    uint64_t m_headSize = 0; // num bytes of total header bytes that need to be written.

    uint64_t m_fileIndex = 0;
    uint64_t m_fileCount = 0;

    uint64_t m_fileByte = 0;
    uint64_t m_fileSize = 0;

    uint64_t m_paddByte = 0;
    uint64_t m_paddSize = 0;

    uint64_t m_endByte = 0;     // We need to write 2 empty 512 blocks...
    uint64_t m_endSize = 1024;  // ... to indicate End Of TAR file.

    uint64_t m_byteIndex = 0;
    uint64_t m_byteCount = 0;

    int m_state = 0; // 0 = create header
                     // 1 = write header
                     // 2 = write file data
                     // 3 = write file padding

    bool m_bOpen = false;

    // TarWriter()
    // {
    // };

    // ~TarWriter()
    // {
    //     close();
    // }

    bool configure(const Cfg& cfg);

    // Benni statemachine with 4+1 states now. Hope it is well designed.

    int64_t process(uint8_t* __restrict__ out, int64_t outSize);
};
