#include <de/sound/WAV/SoundReader_WAV.h>

namespace de {
namespace sound {
namespace {

#pragma pack(push, 1)

struct WavRiffHeader {
    char     chunk_id[4];     // "RIFF"
    uint32_t chunk_size;
    char     format[4];       // "WAVE"
};

struct WavFmtChunk {
    char     subchunk1_id[4]; // "fmt "
    uint32_t subchunk1_size;  // 16 for PCM
    uint16_t audio_format;    // 1 = PCM, 3 = float
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

struct WavDataChunk {
    char     subchunk2_id[4]; // "data"
    uint32_t subchunk2_size;
};
#pragma pack(pop)

} // end namespace.

/*
    Summary table (sample types WAV can contain)
    Sample Type	Supported in WAV?	Notes
    U8	Yes	Standard PCM
    S8	Yes	Less common
    S12	Yes	Rare, but valid
    S16	Yes	CD standard
    S24	Yes	Studio standard
    S32	Yes	Integer PCM
    F32	Yes	DAW standard
    F64	Yes	High precision
    A‑law	Yes	Telephony
    µ‑law	Yes	Telephony
    MS ADPCM	Yes	Compressed
    IMA ADPCM	Yes	Compressed
*/

bool load_sound_wav(Sound& sound, const std::string& uri)
{
    File file(uri, eFileMode::Read);
    if (!file.is_open())
    {
        DE_ERROR("Cannot read WAV. ",uri)
        return false;
    }

    WavRiffHeader riff;
    file.read(&riff, sizeof(riff));
    if (strncmp(riff.chunk_id, "RIFF", 4) != 0 ||
        strncmp(riff.format, "WAVE", 4) != 0)
    {
        DE_ERROR("Malformed header.")
        return false;
    }

    WavFmtChunk fmt;
    file.read(&fmt, sizeof(fmt));
    if (strncmp(fmt.subchunk1_id, "fmt ", 4) != 0)
    {
        DE_ERROR("Malformed fmt-Chunk.")
        return false;
    }

    // PCM oder Float
    if (fmt.audio_format != 1 && fmt.audio_format != 3)
    {
        DE_ERROR("Unsupported format (only PCM oder Float).")
        return false;
    }

    // Suche nach "data"-Chunk
    WavDataChunk data;
    while (true)
    {
        auto ret = file.read(&data, sizeof(data));
        if (ret != sizeof(data))
        {
            DE_ERROR("No data-Chunk")
            return false;
        }
        if (strncmp(data.subchunk2_id, "data", 4) == 0)
            break;

        // Unbekannter Chunk → überspringen
        file.seek(data.subchunk2_size, eSeekMode::Cur);
    }

    const int64_t nFrames = data.subchunk2_size / fmt.block_align;
    const int64_t nSamples = nFrames * int(fmt.num_channels);
    sound.m_uri = uri;
    sound.m_channels = fmt.num_channels;
    sound.m_frames = nFrames;
    sound.m_sampleRate = fmt.sample_rate;
    sound.m_sampleType = SampleType::Unknown;

    const int bits = fmt.bits_per_sample;

    // Integer PCM
    if (fmt.audio_format == 1)
    {
        if (bits == 8)
        {
            sound.m_sampleType = SampleType::U8;
        }
        else if (bits == 16)
        {
            sound.m_sampleType = SampleType::S16;
        }
        else if (bits == 24)
        {
            sound.m_sampleType = SampleType::S24;
        }
        else if (bits == 32)
        {
            sound.m_sampleType = SampleType::S32;
        }
        else
        {
            DE_ERROR("Unsupported WAV integer PCM ", bits)
            return false;
        }
    }
    // Float PCM
    else if (fmt.audio_format == 3)
    {
        if (bits == 32)
        {
            sound.m_sampleType = SampleType::F32;
        }
        else if (bits == 64)
        {
            sound.m_sampleType = SampleType::F64;
        }
        else
        {
            DE_ERROR("Unsupported WAV float PCM ", bits)
            return false;
        }
    }

    const int64_t nBytes = nSamples * bits / 8;
    sound.m_samples.resize(nBytes);
    uint8_t* __restrict__ pBytes = sound.m_samples.data();
    file.read(pBytes,nBytes);
    return true;
}

} // end namespace sound.
} // end namespace de.
