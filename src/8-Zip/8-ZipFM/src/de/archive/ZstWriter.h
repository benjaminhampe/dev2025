#pragma once
#include "TarWriter.h"
#include "ZstHeader.h"

// =====================================================
struct ZstWriter
// =====================================================
{
    struct Cfg
    {
        int64_t blockSize = 8 * 1024 * 1024;

        ZstPreset preset;

        typedef std::function<void(const FileInfo& /* fileInfo */, uint32_t)> FN_onNextFile;

        FN_onNextFile onNextFile;

        typedef std::function<void(const uint64_t /* byteCount */)> FN_onProcessed;

        FN_onProcessed onProcessed;
    };

    Cfg m_cfg;

    int64_t m_byteIndex = 0;
    int64_t m_byteCount = 0;
    int64_t m_callCount = 0;

    enum eState
    {
        STATE_TAR = 0, // → TAR liefert Daten

        STATE_ZSTD_FLUSH, // → TAR ist fertig, ZSTD muss noch flushen

        STATE_DONE, // → alles fertig
    };

    int m_state = 0;

    TarWriter* m_tarWriter;
    ZSTD_CCtx* m_ctx;

    de::Blob m_iBlob;
    de::Blob m_oBlob;

    ZSTD_inBuffer m_zin;
    ZSTD_outBuffer m_zout;

    void close()
    {
        if (m_ctx)
        {
            ZSTD_freeCCtx(m_ctx);
            m_ctx = nullptr;
        }
    }

    bool init(const Cfg& cfg, TarWriter* tarWriter)
    {
        if (!tarWriter)
        {
            DE_ERROR("No tarWriter")
            return false;
        }
        m_cfg = cfg;

        m_byteIndex = 0;
        m_byteCount = 0;
        m_callCount = 0;
        m_tarWriter = tarWriter;

        m_iBlob.resize( m_cfg.blockSize );
        m_oBlob.resize( m_cfg.blockSize );

        m_ctx = ZSTD_createCCtx();
        if (!m_ctx)
        {
            DE_ERROR("No ZStd context.")
            return false;
        }

        size_t ok = 0;
        if (m_cfg.preset.algo == 0)
        {
            ok = ZSTD_CCtx_setParameter(m_ctx, ZSTD_c_compressionLevel, m_cfg.preset.level);
        }
        // else if (m_cfg.preset.algo == ZSTD_fast)
        // {
        //     ok = ZSTD_CCtx_setParameter(m_ctx, ZSTD_fast, m_cfg.preset.level);
        // }
        // else
        // {
        //     ok = ZSTD_CCtx_setParameter(m_ctx, ZSTD_c_compressionLevel, 3);
        // }

        if (ZSTD_isError(ok))
        {
            DE_ERROR("Invalid ZSTD_c_compressionLevel = ",m_cfg.preset.level)
            ZSTD_freeCCtx(m_ctx);
            return false;
        }

        DE_DEBUG("ZSTD_c_compressionLevel = ",m_cfg.preset.level)

        m_state = STATE_TAR;

        return true;
    }

    // Benni statemachine with 4+1 states now. Hope it is well designed.


    int64_t process(uint8_t* __restrict out, int64_t outSize)
    {
        int64_t outWritten = 0;

        while (outWritten < outSize)
        {
            // 1) TAR INPUT PHASE
            if (m_state == STATE_TAR)
            {
                m_zin.src  = m_iBlob.data();
                m_zin.size = m_tarWriter->process(m_iBlob.data(), m_iBlob.size());
                m_zin.pos  = 0;

                if (m_zin.size == 0)
                {
                    // TAR finished → switch to ZSTD flush mode
                    m_state = STATE_ZSTD_FLUSH;
                }
            }

            // 2) ZSTD OUTPUT PHASE
            m_zout.dst  = out + outWritten;
            m_zout.size = outSize - outWritten;
            m_zout.pos  = 0;

            ZSTD_EndDirective mode =
                (m_state == STATE_ZSTD_FLUSH)
                ? ZSTD_e_end
                : ZSTD_e_continue;

            size_t ret = ZSTD_compressStream2(
                m_ctx,
                &m_zout,
                &m_zin,
                mode);

            if (ZSTD_isError(ret))
            {
                DE_ERROR("ZSTD: ", ZSTD_getErrorName(ret));
                return -1;
            }

            outWritten += m_zout.pos;

            // 3) Check if ZSTD is fully flushed
            if (m_state == STATE_ZSTD_FLUSH && ret == 0)
            {
                m_state = STATE_DONE;
                break;
            }

            // 4) If no output was produced, break to avoid infinite loop
            if (m_zout.pos == 0)
            {
                break;
            }
        }

        return outWritten;
    }

    /* BAD
    int64_t process(uint8_t* __restrict__ out, int64_t outSize)
    {


        int64_t nReadBytes = m_tarWriter->process(m_iBlob.data(), m_iBlob.size());
        if (nReadBytes > 0)
        {
            int64_t has = std::min<int64_t>(tarDataSize - readBytes, inBlob.size());
            int64_t got = tarFile.read(inBlob.data(), has);

            readBytes += got;

            options.onProgress( 100.0 * double(readBytes) / double(tarDataSize) );

            // Input setzen
            zin.src = inBlob.data();
            zin.size = got;
            zin.pos = 0;

            // Streamen
            while (zin.pos < zin.size)
            {
                zout.dst = outBuf.data();
                zout.size = outBuf.size();
                zout.pos = 0;

                size_t ret = ZSTD_compressStream2(
                                cctx,
                                &zout,
                                &zin,
                                ZSTD_e_continue);

                if (ZSTD_isError(ret))
                {
                    DE_ERROR("ZSTD: ", ZSTD_getErrorName(ret))
                    return false;
                }

                // out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
                zstFile.write(outBuf.data(), zout.pos);
            }
        }
        else
        {
            DE_BENNI("EOS, m_callCount = ",m_callCount)
            return 0;
        }



        m_callCount++;
    }
    */
};

/*
inline bool
compress_zstd( const std::string& input_tar,
            const std::string& output_zst)
{
    de::File tarFile(input_tar, de::eFileMode::Read);
    if (!tarFile.is_open())
    {
        DE_ERROR("TAR file not readable. ", input_tar)
        return false;
    }

    de::File zstFile(output_zst, de::eFileMode::Write);
    if (!zstFile.is_open())
    {
        DE_ERROR("ZStd File not writable. ", output_zst)
        return false;
    }

    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    if (!cctx)
    {
        DE_ERROR("No ZStd context. ", output_zst)
        return false;
    }

    size_t ret = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 5);
    if (ZSTD_isError(ret))
    {
        DE_ERROR("Invalid ZStd compression level")
        ZSTD_freeCCtx(cctx);
        return false;
    }

    ZSTD_inBuffer zin;
    ZSTD_outBuffer zout;

    de::Blob outBuf(1 << 20); // 1MB
    de::Blob inBlob(1 << 20);

    // Header schreiben:
    // U32 path length
    // VAR path
    // U64 data length -> so files are limited to 4GB each.
    uint32_t tarNameSize = input_tar.size();
    zstFile.write(&tarNameSize, sizeof(uint32_t));
    zstFile.write(input_tar.data(), input_tar.size());
    uint64_t tarDataSize = static_cast<uint64_t>(tarFile.size());
    zstFile.write(&tarDataSize, sizeof(uint64_t));

    int64_t readBytes = 0;
    while (readBytes < tarDataSize)
    {
        int64_t has = std::min<int64_t>(tarDataSize - readBytes, inBlob.size());
        int64_t got = tarFile.read(inBlob.data(), has);

        readBytes += got;

        options.onProgress( 100.0 * double(readBytes) / double(tarDataSize) );

        // Input setzen
        zin.src = inBlob.data();
        zin.size = got;
        zin.pos = 0;

        // Streamen
        while (zin.pos < zin.size)
        {
            zout.dst = outBuf.data();
            zout.size = outBuf.size();
            zout.pos = 0;

            size_t ret = ZSTD_compressStream2(
                            cctx,
                            &zout,
                            &zin,
                            ZSTD_e_continue);

            if (ZSTD_isError(ret))
            {
                DE_ERROR("ZSTD: ", ZSTD_getErrorName(ret))
                return false;
            }

            // out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
            zstFile.write(outBuf.data(), zout.pos);
        }
    }

    // Final flush
    {
        ZSTD_inBuffer empty = { nullptr, 0, 0 };
        size_t remaining = 1;
        while (remaining)
        {
            zout.dst = outBuf.data();
            zout.size = outBuf.size();
            zout.pos = 0;

            remaining = ZSTD_compressStream2(cctx, &zout, &empty, ZSTD_e_end);
            //out.write(reinterpret_cast<char*>(outBuf.data()), zout.pos);
            zstFile.write(outBuf.data(), zout.pos);
        }
    }

    ZSTD_freeCCtx(cctx);
    return true;
}
*/
