#include "TarWriter.h"

TarWriter::TarWriter()
{
}

TarWriter::~TarWriter()
{
    close();
}

void TarWriter::close()
{

}

bool TarWriter::init(const Cfg& cfg)
{
    if (!cfg.fileInfos)
    {
        DE_ERROR("No FileInfos")
        return false;
    }
    m_cfg = cfg;
    m_head.resize(1024*129);
    m_headByte = 0;
    m_headSize = 0;

    //m_data.resize(1024*1024*8);

    m_fileIndex = 0;
    m_fileCount = m_cfg.fileInfos->size();

    m_fileByte = 0;
    m_fileSize = 0;

    m_paddByte = 0;
    m_paddSize = 0;

    m_endByte = 0;
    m_endSize = 1024;

    m_fileByte = 0;
    m_fileSize = 0;

    m_state = 0;
    m_bOpen = true;
    return m_bOpen;
}

// Benni statemachine with 4+1 states now. Hope it is well designed.

int64_t
TarWriter::process(uint8_t* __restrict__ out, int64_t outSize)
{
    uint64_t doneBytes = 0;

    int i = 0;

    while (doneBytes < outSize)
    {
        const uint64_t remainBytes = outSize - doneBytes;

        uint8_t* __restrict__ p = out + doneBytes;

        // All fileinfos are processed. Finish TAR...
        if (m_fileIndex >= m_fileCount)
        {
            // Write TAR end padding 1024 bytes ...
            if (m_endByte < m_endSize)
            {
                uint64_t availBytes = std::min<uint64_t>(remainBytes, m_endSize - m_endByte);
                std::memset(p, 0, availBytes);
                doneBytes += availBytes;
                m_endByte += availBytes;
            }
            // TAR is done.
            else
            {
                DE_DEBUG("End of TAR stream.")
                m_file.close(); // Close any open file handle.
                break;
            }
        }
        // ForEach FileInfo.
        else
        {
            const de::FileInfo& fi = (*m_cfg.fileInfos)[ m_fileIndex ];

            switch (m_state)
            {
                case 0: // Create header
                {
                    // <Communicate-with-GUI>
                    m_cfg.onNextFile(fi,m_fileIndex);
                    // </Communicate-with-GUI>
                    std::memset(m_head.data(),0,m_head.size());
                    m_headSize = TarUtil::tar_build_header(m_head.data(), fi, m_cfg.baseDir, m_cfg.archiveBaseName);
                    m_headByte = 0;
                    m_state = 1; // Start writing header data
                    if (m_cfg.bDebug)
                    {
                        DE_BENNI("[",i,"] Made header bytes ",m_headSize, ", ", de_mbstr(fi.uri()))
                    }
                    break;
                }
                case 1: // Write header
                {
                    uint64_t availBytes = std::min<uint64_t>(remainBytes, m_headSize - m_headByte);
                    std::memcpy(p,m_head.data() + m_headByte, availBytes);
                    if (m_cfg.bDebug)
                    {
                        DE_DEBUG("[",i,"] Wrote header bytes ",availBytes)
                    }
                    doneBytes += availBytes;
                    m_headByte += availBytes;

                    if (m_headByte >= m_headSize)
                    {
                        if (fi.isFile() && fi.fileSize() > 0)
                        {
                            m_state = 2; // Start writing file data
                            m_fileByte = 0;
                            m_file.close();
                            m_file.open(fi.uri(),de::eFileMode::Read);
                            m_fileSize = fi.fileSize();
                            if (m_cfg.bDebug)
                            {
                                DE_DEBUG("[",i,"] Ok (file) header written. ", de_mbstr(fi.uri()))
                            }
                        }
                        else
                        {
                            m_fileIndex++;
                            m_state = 0; // Directory or empty file is done, go to next fileInfo.
                            if (m_cfg.bDebug)
                            {
                                DE_DEBUG("[",i,"] Ok (dir) header written. ", de_mbstr(fi.uri()))
                            }
                        }
                    }
                    break;
                }
                case 2: // Write FileData
                {
                    uint64_t availBytes = std::min<uint64_t>(remainBytes, m_fileSize - m_fileByte);
                    uint64_t wroteBytes = m_file.read(p,availBytes);
                    if (m_cfg.bDebug)
                    {
                        DE_DEBUG("[",i,"] Wrote file bytes ",wroteBytes)
                    }
                    if (wroteBytes == 0)
                        return doneBytes; // yield, caller must retry

                    m_fileByte += wroteBytes;
                    doneBytes += wroteBytes;

                    if (m_fileByte >= m_fileSize)
                    {
                        // Nur padding wenn (remain > 0)
                        uint64_t remain = m_fileSize % 512ul;
                        m_paddSize = remain ? (512ul - remain) : 0;
                        m_paddByte = 0;

                        if (m_paddSize > 0)
                        {
                            if (m_cfg.bDebug)
                            {
                                DE_DEBUG("[",i,"] Need padd bytes ",m_paddSize)
                            }
                            m_state = 3; // Padd file data to 512 bytes
                        }
                        else
                        {
                            if (m_cfg.bDebug)
                            {
                                DE_DEBUG("[",i,"] No padding needed.")
                            }
                            m_fileIndex++;
                            m_state = 0; // Goto next fileInfo
                        }
                    }
                    break;
                }
                case 3: // Write FileData padding
                {
                    uint64_t availBytes = std::min<uint64_t>(remainBytes, m_paddSize - m_paddByte);
                    std::memset(p,0,availBytes);
                    if (m_cfg.bDebug)
                    {
                        DE_DEBUG("[",i,"] Wrote padd bytes ",availBytes)
                    }
                    m_paddByte += availBytes;
                    doneBytes += availBytes;

                    if (m_paddByte >= m_paddSize)
                    {
                        if (m_cfg.bDebug)
                        {
                            DE_OK("[",i,"] Done FileInfo[",m_fileIndex,"/",m_fileCount,"] ",fi.str())
                        }
                        m_fileIndex++;
                        m_state = 0; // Goto next fileInfo
                    }
                    break;
                }
            }

            i++; // Count iterations
        }
    }

    // <Communicate-with-GUI>
    m_fileByte += doneBytes;
    if (m_cfg.onProcessed)
    {
        m_cfg.onProcessed(m_fileByte);
    }
    // </Communicate-with-GUI>
    return doneBytes; // 0 == (EOS) End of stream
}
