#include <de/sound/AIFF/SoundReader_AIFF.h>

#include <de/sound/SoundUtil.h>

namespace de {
namespace sound {
namespace {

// ============================================================================
// AIFF 80-bit IEEE Extended Float → double
//
// AIFF speichert die Sample-Rate NICHT als Integer.
// Stattdessen als 80-bit Extended Float (Motorola 68k Format).
//
// Struktur:
//   Byte 0: Sign + oberer Teil des Exponenten
//   Byte 1: unterer Teil des Exponenten
//   Byte 2–5: obere 32 Bit der Mantisse
//   Byte 6–9: untere 32 Bit der Mantisse
//
// Wert = Mantisse * 2^(Exponent - 16383)
// Mantisse = hiMant * 2^-31 + loMant * 2^-63
// ============================================================================

static double read_ieee_extended(File & file)
{
    uint8_t b[10];
    file.read(b, 10);

    bool negative = (b[0] & 0x80) != 0;
    int16_t exponent = ((b[0] & 0x7F) << 8) | b[1];

    uint32_t hiMant =
        (uint32_t(b[2]) << 24) |
        (uint32_t(b[3]) << 16) |
        (uint32_t(b[4]) << 8)  |
        (uint32_t(b[5]));

    uint32_t loMant =
        (uint32_t(b[6]) << 24) |
        (uint32_t(b[7]) << 16) |
        (uint32_t(b[8]) << 8)  |
        (uint32_t(b[9]));

    if (exponent == 0 && hiMant == 0 && loMant == 0)
        return 0.0;

    double mantissa =
        hiMant * std::pow(2.0, -31.0) +
        loMant * std::pow(2.0, -63.0);

    double value = mantissa * std::pow(2.0, exponent - 16383);

    if (negative)
        value = -value;

    return value;
}

} // end namespace.

// ============================================================================
bool load_sound_aiff(Sound & sound, const std::string& uri)
// ============================================================================
{
    File file(uri, eFileMode::Read);
    if (!file.is_open())
    {
        DE_ERROR("Cannot read AIFF, ",uri)
        return false;
    }

    // ------------------------------------------------------------------------
    // FORM Chunk (AIFF/AIFC Header)
    // ------------------------------------------------------------------------
    char id[4];
    file.read(id, 4);
    if (memcmp(id, "FORM", 4) != 0)
    {
        DE_ERROR("No FORM in ", uri)
        return false;
    }

    uint32_t formSize = 0;
    file.read_u32_be(&formSize);
    (void)formSize;

    bool isAIFC = false;

    file.read(id, 4);
         if (memcmp(id, "AIFF", 4) == 0) isAIFC = false;
    else if (memcmp(id, "AIFC", 4) == 0) isAIFC = true;
    else
    {
        DE_ERROR("No AIFF or AIFC in ", uri)
        return false;
    }

    // ------------------------------------------------------------------------
    // COMM + SSND Daten
    // ------------------------------------------------------------------------
    uint16_t channels = 0;
    uint32_t frames = 0;
    uint16_t bitDepth = 0;
    double sampleRate = 0.0;

    int64_t ssndDataPos = 0;

    bool haveCOMM = false;
    bool haveSSND = false;

    // ------------------------------------------------------------------------
    // Chunk-Loop
    // ------------------------------------------------------------------------
    while (!haveCOMM || !haveSSND)
    {
        if (file.read(id, 4) != 4)
            break;

        uint32_t chunkSize = 0;
        file.read_u32_be(&chunkSize);

        int64_t chunkStart = file.tell();

        // COMM Chunk
        if (memcmp(id, "COMM", 4) == 0)
        {
            file.read_u16_be(&channels);
            file.read_u32_be(&frames);
            file.read_u16_be(&bitDepth);
            sampleRate = read_ieee_extended(file);

            if (isAIFC)
            {
                char compType[4];
                file.read(compType, 4);

                // AIFC float formats
                if (memcmp(compType, "fl32", 4) == 0)
                {
                    sound.m_sampleType = SampleType::F32;
                    bitDepth = 32; // override PCM bitDepth
                }
                else if (memcmp(compType, "fl64", 4) == 0)
                {
                    sound.m_sampleType = SampleType::F64;
                    bitDepth = 64; // override PCM bitDepth
                }
            }

            haveCOMM = true;
        }
        // SSND Chunk
        else if (memcmp(id, "SSND", 4) == 0)
        {
            uint32_t ssndOffset = 0;
            uint32_t blockSize = 0;
            file.read_u32_be(&ssndOffset);
            file.read_u32_be(&blockSize);
            ssndDataPos = file.tell() + ssndOffset;
            haveSSND = true;
        }

        // AIFF Chunks sind byte-aligned → bei ungerader Größe 1 Byte skippen
        file.seek(chunkStart + chunkSize + (chunkSize & 1), eSeekMode::Set);
    }

    if (!haveCOMM || !haveSSND)
    {
        DE_ERROR("No COMM or SSND in ",uri)
        return false;
    }

    // ------------------------------------------------------------------------
    // Sound-Struktur füllen
    // ------------------------------------------------------------------------
    sound.m_uri = uri;
    sound.m_channels = channels;
    sound.m_frames = frames;
    sound.m_sampleRate = int32_t(sampleRate);
    sound.m_flags = 0; // interleaved
/*
    switch (bitDepth)
    {
        case 8:  sound.m_sampleType = SampleType::S8;  break;
        case 16: sound.m_sampleType = SampleType::S16; break;
        case 24: sound.m_sampleType = SampleType::S24; break;
        case 32: sound.m_sampleType = SampleType::S32; break;
        default: sound.m_sampleType = SampleType::Unknown; break;
    }
*/
    // Only assign PCM types if not already set by AIFC float detection
    if (sound.m_sampleType == SampleType::Unknown)
    {
        switch (bitDepth)
        {
            case 8:  sound.m_sampleType = SampleType::S8;  break;
            case 16: sound.m_sampleType = SampleType::S16; break;
            case 24: sound.m_sampleType = SampleType::S24; break;
            case 32: sound.m_sampleType = SampleType::S32; break;
            case 64: sound.m_sampleType = SampleType::F64; break; // rare PCM64
            default: break;
        }
    }

    size_t bytesPerSample = bitDepth / 8;
    size_t totalBytes = size_t(frames) * channels * bytesPerSample;

    sound.m_samples.resize(totalBytes);
    uint8_t* __restrict__ p = sound.m_samples.data();
    int64_t n = sound.sampleCount();

    // ------------------------------------------------------------------------
    // PCM lesen (Big-Endian → Little-Endian)
    // ------------------------------------------------------------------------
    file.seek(ssndDataPos, eSeekMode::Set);
    file.read(p,totalBytes);

    if (sound.m_sampleType == SampleType::F32)
    {
        SoundUtil::byteSwap32_inplace(p,n);    // FLOAT32 (AIFC)
    }
    else if (sound.m_sampleType == SampleType::F64)
    {
        SoundUtil::byteSwap64_inplace(p,n);    // FLOAT64 (AIFC)
    }
    else if (bitDepth == 8)
    {
        // Nothing todo.    // AIFF (PCM-8)
    }
    else if (bitDepth == 16)
    {
        SoundUtil::byteSwap16_inplace(p,n);    // AIFF (PCM-16)
    }
    else if (bitDepth == 24)
    {
        SoundUtil::byteSwap24_inplace(p,n);    // AIFF (PCM-24)
    }
    else if (bitDepth == 32)
    {
        SoundUtil::byteSwap32_inplace(p,n);    // AIFF (PCM-32)
    }

    return true;
}

} // end namespace sound.
} // end namespace de.

#if 0

// ============================================================================
// Hilfsfunktionen: Big-Endian lesen
// ============================================================================

static uint16_t read_be16(FILE* f)
{
    uint8_t b[2];
    fread(b, 1, 2, f);
    return (uint16_t(b[0]) << 8) | uint16_t(b[1]);
}

static uint32_t read_be32(FILE* f)
{
    uint8_t b[4];
    fread(b, 1, 4, f);
    return (uint32_t(b[0]) << 24) |
           (uint32_t(b[1]) << 16) |
           (uint32_t(b[2]) << 8)  |
           (uint32_t(b[3]));
}

// ============================================================================
// AIFF 80-bit IEEE Extended Float → double
//
// AIFF speichert die Sample-Rate NICHT als Integer.
// Stattdessen als 80-bit Extended Float (Motorola 68k Format).
//
// Struktur:
//   Byte 0: Sign + oberer Teil des Exponenten
//   Byte 1: unterer Teil des Exponenten
//   Byte 2–5: obere 32 Bit der Mantisse
//   Byte 6–9: untere 32 Bit der Mantisse
//
// Wert = Mantisse * 2^(Exponent - 16383)
// Mantisse = hiMant * 2^-31 + loMant * 2^-63
// ============================================================================

static double read_ieee_extended(FILE* f)
{
    uint8_t b[10];
    fread(b, 1, 10, f);

    bool negative = (b[0] & 0x80) != 0;
    int16_t exponent = ((b[0] & 0x7F) << 8) | b[1];

    uint32_t hiMant =
        (uint32_t(b[2]) << 24) |
        (uint32_t(b[3]) << 16) |
        (uint32_t(b[4]) << 8)  |
        (uint32_t(b[5]));

    uint32_t loMant =
        (uint32_t(b[6]) << 24) |
        (uint32_t(b[7]) << 16) |
        (uint32_t(b[8]) << 8)  |
        (uint32_t(b[9]));

    if (exponent == 0 && hiMant == 0 && loMant == 0)
        return 0.0;

    double mantissa =
        hiMant * std::pow(2.0, -31.0) +
        loMant * std::pow(2.0, -63.0);

    double value = mantissa * std::pow(2.0, exponent - 16383);

    if (negative)
        value = -value;

    return value;
}

// ============================================================================
// Hauptfunktion: AIFF Reader
// ============================================================================

bool SoundReader_AIFF(const std::string& path, Sound& snd)
{
    FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return false;

    char id[4];

    // ------------------------------------------------------------------------
    // FORM Chunk (AIFF/AIFC Header)
    // ------------------------------------------------------------------------
    fread(id, 1, 4, f);
    if (memcmp(id, "FORM", 4) != 0) { fclose(f); return false; }

    uint32_t formSize = read_be32(f);
    (void)formSize;

    fread(id, 1, 4, f);
    bool isAIFC = false;

    if (memcmp(id, "AIFF", 4) == 0) isAIFC = false;
    else if (memcmp(id, "AIFC", 4) == 0) isAIFC = true;
    else { fclose(f); return false; }

    // ------------------------------------------------------------------------
    // COMM + SSND Daten
    // ------------------------------------------------------------------------
    uint16_t channels = 0;
    uint32_t frames = 0;
    uint16_t bitDepth = 0;
    double sampleRate = 0.0;

    long ssndDataPos = 0;
    uint32_t ssndOffset = 0;

    bool haveCOMM = false;
    bool haveSSND = false;

    // ------------------------------------------------------------------------
    // Chunk-Loop
    // ------------------------------------------------------------------------
    while (!haveCOMM || !haveSSND)
    {
        if (fread(id, 1, 4, f) != 4)
            break;

        uint32_t chunkSize = read_be32(f);
        long chunkStart = ftell(f);

        // COMM Chunk
        if (memcmp(id, "COMM", 4) == 0)
        {
            channels = read_be16(f);
            frames   = read_be32(f);
            bitDepth = read_be16(f);
            sampleRate = read_ieee_extended(f);

            if (isAIFC)
            {
                char compType[4];
                fread(compType, 1, 4, f);
            }

            haveCOMM = true;
        }
        // SSND Chunk
        else if (memcmp(id, "SSND", 4) == 0)
        {
            ssndOffset = read_be32(f);
            uint32_t blockSize = read_be32(f);
            ssndDataPos = ftell(f) + ssndOffset;
            haveSSND = true;
        }

        // AIFF Chunks sind byte-aligned → bei ungerader Größe 1 Byte skippen
        fseek(f, chunkStart + chunkSize + (chunkSize & 1), SEEK_SET);
    }

    if (!haveCOMM || !haveSSND)
    {
        fclose(f);
        return false;
    }

    // ------------------------------------------------------------------------
    // Sound-Struktur füllen
    // ------------------------------------------------------------------------
    snd.m_uri = path;
    snd.m_channels = channels;
    snd.m_frames = frames;
    snd.m_sampleRate = int32_t(sampleRate);
    snd.m_flags = 0; // interleaved

    switch (bitDepth)
    {
        case 8:  snd.m_sampleType = SampleType::S8;  break;
        case 16: snd.m_sampleType = SampleType::S16; break;
        case 24: snd.m_sampleType = SampleType::S24; break;
        case 32: snd.m_sampleType = SampleType::S32; break;
        default: snd.m_sampleType = SampleType::Unknown; break;
    }

    size_t bytesPerSample = bitDepth / 8;
    size_t totalBytes = size_t(frames) * channels * bytesPerSample;

    snd.m_samples.resize(totalBytes);

    // ------------------------------------------------------------------------
    // PCM lesen (Big-Endian → Little-Endian)
    // ------------------------------------------------------------------------
    fseek(f, ssndDataPos, SEEK_SET);
    uint8_t* dst = snd.m_samples.data();

    if (bitDepth == 8)
    {
        fread(dst, 1, totalBytes, f);
    }
    else if (bitDepth == 16)
    {
        for (size_t i = 0; i < totalBytes; i += 2)
        {
            uint8_t b[2];
            fread(b, 1, 2, f);
            dst[i]   = b[1];
            dst[i+1] = b[0];
        }
    }
    else if (bitDepth == 24)
    {
        for (size_t i = 0; i < totalBytes; i += 3)
        {
            uint8_t b[3];
            fread(b, 1, 3, f);
            dst[i]   = b[2];
            dst[i+1] = b[1];
            dst[i+2] = b[0];
        }
    }
    else if (bitDepth == 32)
    {
        for (size_t i = 0; i < totalBytes; i += 4)
        {
            uint8_t b[4];
            fread(b, 1, 4, f);
            dst[i]   = b[3];
            dst[i+1] = b[2];
            dst[i+2] = b[1];
            dst[i+3] = b[0];
        }
    }

    fclose(f);
    return true;
}

#endif
