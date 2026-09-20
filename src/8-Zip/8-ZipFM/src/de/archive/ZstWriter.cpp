#include "ZstWriter.h"

ZstWriter::ZstWriter()
{

}

ZstWriter::~ZstWriter()
{
    close();
}

void ZstWriter::close()
{
    if (m_ctx)
    {
        ZSTD_freeCCtx(m_ctx);
        m_ctx = nullptr;
    }
}

bool ZstWriter::init(const Cfg& cfg, TarWriter* tarWriter)
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
        DE_ERROR("No ZSTD context.")
        return false;
    }

    //DE_INFO("ZSTD_staticSize = ",m_ctx->staticSize)

    // 2. Aktiviere Multithreading (0 bedeutet automatisch alle Kerne, oder z.B. 4)
    // size_t e1 = ZSTD_CCtx_setParameter(m_ctx, ZSTD_c_nbWorkers, m_cfg.num_threads);

    // if (ZSTD_isError(e1))
    // {
    //     // Fehlerbehandlung
    //     DE_ERROR("Invalid ZSTD_c_nbWorkers = ",m_cfg.num_threads, ", e1(",e1,") = ",ZSTD_getErrorName(e1))
    //     ZSTD_freeCCtx(m_ctx);
    //     return false;
    // }

    DE_INFO("ZSTD_num_threads = ",m_cfg.num_threads)

    size_t ok = 0;
    // if (m_cfg.preset.preset == 0)
    // {
        ok = ZSTD_CCtx_setParameter(m_ctx,
                                    ZSTD_c_compressionLevel,
                                    m_cfg.compressionLevel);
    // }
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
        DE_ERROR("Invalid ZSTD_c_compressionLevel = ",m_cfg.compressionLevel)
        ZSTD_freeCCtx(m_ctx);
        return false;
    }

    DE_DEBUG("ZSTD_c_compressionLevel = ",m_cfg.compressionLevel)


    m_state = STATE_TAR;
    return true;
}

int64_t ZstWriter::process(uint8_t* __restrict out, int64_t outSize)
{
    int64_t outWritten = 0;

    // Schleife läuft, solange noch Platz im Ausgabe-Puffer ist
    // UND wir noch nicht fertig sind (STATE_DONE).
    while (outWritten < outSize && m_state != STATE_DONE)
    {
        // 1) TAR INPUT PHASE
        // Nur neue Daten holen, wenn der ZSTD-Input-Buffer komplett leer/verarbeitet ist
        if (m_state == STATE_TAR && m_zin.pos >= m_zin.size)
        {
            m_zin.src = m_iBlob.data();
            m_zin.size = m_tarWriter->process(m_iBlob.data(), m_iBlob.size());
            m_zin.pos = 0;

            if (m_zin.size == 0) {
                // TAR fertig -> wechsle in den ZSTD-Flush-Modus
                m_state = STATE_ZSTD_FLUSH;
            }
        }

        // 2) ZSTD OUTPUT PHASE
        m_zout.dst = out + outWritten;
        m_zout.size = outSize - outWritten;
        m_zout.pos = 0;

        ZSTD_EndDirective mode = (m_state == STATE_ZSTD_FLUSH)
                               ? ZSTD_e_end : ZSTD_e_continue;

        // Merk dir den alten Input-Fortschritt, um unendliche Schleifen zu erkennen
        size_t prevInPos = m_zin.pos;

        size_t ret = ZSTD_compressStream2(m_ctx, &m_zout, &m_zin, mode);

        if (ZSTD_isError(ret)) {
            DE_ERROR("ZSTD: ", ZSTD_getErrorName(ret));
            return outWritten;
        }

        outWritten += m_zout.pos;

        // 3) Check if ZSTD is fully flushed
        if (m_state == STATE_ZSTD_FLUSH && ret == 0) {
            m_state = STATE_DONE;
            break;
        }

        // 4) Echte Endlosschleifen-Prüfung:
        // Wenn ZSTD weder Input konsumiert noch Output generiert hat, kommen wir nicht weiter.
        if (m_zout.pos == 0 && m_zin.pos == prevInPos) {
            break;
        }
    }

    m_byteIndex += outWritten;
    if (m_cfg.onProcessed) {
        m_cfg.onProcessed(m_byteIndex);
    }

    return outWritten;
}

#if 0
int64_t ZstWriter::process(uint8_t* __restrict out, int64_t outSize)
{
    int64_t outWritten = 0;

    while (outWritten <= outSize)
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
            return outWritten;
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
    // End while loop.

    m_byteIndex += outWritten;

    if (m_cfg.onProcessed)
    {
        m_cfg.onProcessed(m_byteIndex);
    }
    return outWritten;
}
#endif

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

// ========================================================

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
