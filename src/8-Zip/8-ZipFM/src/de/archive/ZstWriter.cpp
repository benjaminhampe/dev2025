#include "ZstWriter.h"

#include <zstd.h>

/*
#include <stdio.h>
#include <time.h>

// Use standard clock_gettime for cross-platform microsecond precision
struct timespec start, end;
clock_gettime(CLOCK_MONOTONIC, &start);

size_t const cSize = ZSTD_compressCCtx(cctx, cBuffer, cCapacity, srcBuffer, srcSize, 19);

clock_gettime(CLOCK_MONOTONIC, &end);

// Calculate elapsed time in seconds
double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
double mbps = ((double)srcSize / (1024.0 * 1024.0)) / elapsed;

printf("Compressed in: %.4f seconds\n", elapsed);
printf("Throughput:    %.2f MB/s\n", mbps);
*/
struct AutoLibzstdCC
{
    ZSTD_CCtx* m_ctx;

    AutoLibzstdCC()
        : m_ctx{ nullptr }
    {
        open();
    }
    ~AutoLibzstdCC()
    {
        close();
    }

    bool is_open() const
    {
        return (m_ctx != nullptr);
    }

    void open()
    {
        close();
        m_ctx = ZSTD_createCCtx();
    }

    void close()
    {
        if (m_ctx)
        {
            ZSTD_freeCCtx(m_ctx);
            m_ctx = nullptr;
        }
    }


};


bool
ZstCompressFileSimple(
    StringA src, // tar source
    StringA dst, // zst destination
    const ZstCompressFileCfg& cfg)
{
    de::File m_fin(src,de::eFileMode::Read);
    de::File m_fout(dst,de::eFileMode::Write);
    if (!m_fin.is_open()) { DE_ERROR("Cannot read ",src) return false; }
    if (!m_fout.is_open()) { DE_ERROR("Cannot write ",dst) return false; }

    AutoLibzstdCC zst;
    if (!zst.is_open())
    {
        DE_ERROR("No ZSTD_createCCtx()")
        return false;
    }

    auto cctx = zst.m_ctx;

    applyCCtxPreset(cctx,cfg.iPreset);

    ZSTD_CCtx_setPledgedSrcSize(cctx, static_cast<uint64_t>(m_fin.size()) );

    // Setup streaming buffers using recommended sizes
    size_t const buffInSize = ZSTD_CStreamInSize();
    size_t const buffOutSize = ZSTD_CStreamOutSize();
    DE_TRACE("ZSTD_CStreamInSize = ", buffInSize)
    DE_TRACE("ZSTD_CStreamOutSize = ", buffOutSize)

    de::Blob iBlob( buffInSize );
    de::Blob oBlob( buffOutSize );
    void* const buffIn = iBlob.data();
    void* const buffOut = oBlob.data();

    size_t readLen;
    bool ok = true; // Tracks overall streaming success

    // Main streaming loop
    while ((readLen = m_fin.read(buffIn, buffInSize)) > 0)
    {
        if (cfg.bAbort && cfg.bAbort->load())
        {
            return false;
        }

        ZSTD_inBuffer input = { buffIn, readLen, 0 };

        // Push data to the compressor until the input buffer is fully consumed
        while (input.pos < input.size)
        {
            ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };

            size_t const toRead = ZSTD_compressStream2(zst.m_ctx, &output, &input, ZSTD_e_continue);
            if (ZSTD_isError(toRead))
            {
                DE_ERROR(ZSTD_getErrorName(toRead))
                ok = false;
                break;
            }

            // Write out the compressed data chunk
            if (output.pos > 0)
            {
                m_fout.write(buffOut, output.pos);

                // <gui>
                if (cfg.onProcessed)
                {
                    cfg.onProcessed(static_cast<uint64_t>(m_fin.tell()),
                                    static_cast<uint64_t>(m_fout.tell()));
                }
                // </gui>
            }
        }
        if (!ok) break;
    }

    // Flush and finish the frame
    if (ok)
    {
        ZSTD_inBuffer input = { NULL, 0, 0 };
        size_t remainingToFlush;

        do
        {
            if (cfg.bAbort && cfg.bAbort->load())
            {
                return false;
            }

            ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
            // ZSTD_e_end signs off the frame, creating the final Zstd file trailer
            remainingToFlush = ZSTD_compressStream2(zst.m_ctx, &output, &input, ZSTD_e_end);

            if (ZSTD_isError(remainingToFlush))
            {
                DE_ERROR("Flush error: ", ZSTD_getErrorName(remainingToFlush))
                ok = false;
                break;
            }

            if (output.pos > 0)
            {
                m_fout.write(buffOut, output.pos);
                // <gui>
                if (cfg.onProcessed)
                {
                    cfg.onProcessed(static_cast<uint64_t>(m_fin.tell()),
                                    static_cast<uint64_t>(m_fout.tell()));
                }
                // </gui>
            }
        }
        while (remainingToFlush > 0); // Keep flushing until 0 tokens remain
    }

    // <gui>
    if (cfg.onProcessed)
    {
        cfg.onProcessed(static_cast<uint64_t>(m_fin.tell()),
                        static_cast<uint64_t>(m_fout.tell()));
    }
    // </gui>

    // Cleanup resources
    DE_DEBUG("Return Ok = ",ok)
    return ok;
}
