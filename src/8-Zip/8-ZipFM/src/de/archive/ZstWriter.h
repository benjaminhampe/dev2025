#pragma once
#include "TarWriter.h"
#include "ZstHeader.h"

// =====================================================
struct ZstWriter
// =====================================================
{
    struct Cfg
    {
        int num_threads = 8;

        int64_t blockSize = 8 * 1024 * 1024;

        // ZstPreset preset;

        int32_t compressionLevel = 3;

        // typedef std::function<void(const de::FileInfo& /* fileInfo */, uint32_t)> FN_onNextFile;

        // FN_onNextFile onNextFile;

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

    ZstWriter();
    ~ZstWriter();
    void close();
    bool init(const Cfg& cfg, TarWriter* tarWriter);
    int64_t process(uint8_t* __restrict out, int64_t outSize);
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
