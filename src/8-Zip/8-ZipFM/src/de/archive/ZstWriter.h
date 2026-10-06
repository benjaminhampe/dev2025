#pragma once
#include "ZstPreset.h"

struct ZstCompressFileCfg
{
    volatile std::atomic<bool>* bAbort = nullptr;

    int iPreset = -1; // Auto selects (default) preset.

    typedef std::function<void(uint64_t /* processedBytes */,
                               uint64_t /* compressedBytes */)>
        FN_onProcessed;

    FN_onProcessed onProcessed;
};

/**
 * Compresses a file using multi-threaded Zstd.
 *
 * @param source       Path to the input file (e.g., "archive.tar")
 * @param dest         Path to the output file (e.g., "archive.tar.zst")
 * @param compressionLevel The Zstd compression level (1 to 19, 0 for default)
 * @param nbThreads    Number of threads to use (0 auto-detects based on CPU cores)
 * @return             0 on success, 1 on failure
 */
bool
ZstCompressFileSimple(
    StringA src, // tar source
    StringA dst, // zst destination
    const ZstCompressFileCfg& cfg);

#if 0


/**
 * Compresses a file using multi-threaded Zstd.
 *
 * @param source       Path to the input file (e.g., "archive.tar")
 * @param dest         Path to the output file (e.g., "archive.tar.zst")
 * @param compressionLevel The Zstd compression level (1 to 19, 0 for default)
 * @param nbThreads    Number of threads to use (0 auto-detects based on CPU cores)
 * @return             0 on success, 1 on failure
 */
inline bool
zst_compress_file_mt(
    StringA src,
    StringA dst,
    int compressionLevel,
    int numberOfThreads)
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

    // // Configure the compression parameters
    // // 1. Set the compression level
    // auto errLevel = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compressionLevel);
    // if (ZSTD_isError(errLevel))
    // {
    //     DE_ERROR("Cannot set compressionLevel: ", ZSTD_getErrorName(errLevel))
    //     return false;
    // }

    // // 2. Enable multithreading by setting the number of workers
    // // Passing 0 tells libzstd to automatically use all available CPU cores.
    // auto errThread = ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, nbThreads);
    // if (ZSTD_isError(errThread))
    // {
    //     DE_ERROR("Cannot set threadCount: ", ZSTD_getErrorName(errThread))
    //     return false;
    // }

int maxThreads = std::thread::hardware_concurrency();
compressionLevel = 19;
numberOfThreads = std::max<int>(1, maxThreads - 1);
int jobSize = 2 * 1024 * 1024;
int windowLog = 26; // 2^26 = 64MB window
int longDistanceMatching = 1;

// 1. Drop from level 22 to 19 (19 is the highest standard level)
// Level 19 natively allows multi-threading without a master thread bottleneck.
auto e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, compressionLevel);
if (ZSTD_isError(e))
{
    DE_ERROR("ZSTD_c_compressionLevel: ", ZSTD_getErrorName(e))
}

// 2. Enable your 8 worker threads
e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, numberOfThreads);
if (ZSTD_isError(e))
{
    DE_ERROR("ZSTD_c_nbWorkers: ", ZSTD_getErrorName(e))
}

// 3. FORCE smaller job sizes (e.g., 2MB chunks)
// This overrides the massive default 19-level block and forces data
// to be distributed to all 8 threads instantly.
e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_jobSize, jobSize);
if (ZSTD_isError(e))
{
    DE_ERROR("ZSTD_c_jobSize: ", ZSTD_getErrorName(e))
}

// 4. Force a matching window size (e.g., 64MB or 128MB)
// This acts as a replacement for LDM, ensuring high compression ratios.
e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, windowLog); // 2^26 = 64MB window
if (ZSTD_isError(e))
{
    DE_ERROR("ZSTD_c_windowLog: ", ZSTD_getErrorName(e))
}

// 5. Set maximum ultra compression level
e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_enableLongDistanceMatching, longDistanceMatching);
if (ZSTD_isError(e))
{
    DE_ERROR("ZSTD_c_enableLongDistanceMatching: ", ZSTD_getErrorName(e))
}

// 6. Set the LDM window size to maintain high compression ratio (e.g., 128MB)
//e = ZSTD_CCtx_setParameter(cctx, ZSTD_c_ldmWindowLog, 27);
// if (ZSTD_isError(e))
// {
//     DE_ERROR("ZSTD_c_compressionLevel: ", ZSTD_getErrorName(e))
// }

DE_TRACE("maxThreads = ", maxThreads)
DE_TRACE("ZSTD_c_nbWorkers = ", numberOfThreads)
DE_TRACE("ZSTD_c_compressionLevel = ", compressionLevel)
DE_TRACE("ZSTD_c_jobSize = ", jobSize)
DE_TRACE("ZSTD_c_windowLog = ", windowLog)
DE_TRACE("ZSTD_c_enableLongDistanceMatching = ", longDistanceMatching)

    // Setup streaming buffers using recommended sizes
    size_t const buffInSize = ZSTD_CStreamInSize();
    size_t const buffOutSize = ZSTD_CStreamOutSize();
    de::Blob iBlob( buffInSize );
    de::Blob oBlob( buffOutSize );

    void* const buffIn = iBlob.data();
    void* const buffOut = oBlob.data();

    DE_TRACE("ZSTD_CStreamInSize = ", buffInSize)
    DE_TRACE("ZSTD_CStreamOutSize = ", buffOutSize)

    size_t readLen;
    bool ok = true; // Tracks overall streaming success

    // Main streaming loop
    while ((readLen = m_fin.read(buffIn, buffInSize)) > 0)
    {
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
            }
        }
        while (remainingToFlush > 0); // Keep flushing until 0 tokens remain
    }

    // Cleanup resources
    DE_DEBUG("Return Ok = ",ok)
    return ok;
}

#endif



