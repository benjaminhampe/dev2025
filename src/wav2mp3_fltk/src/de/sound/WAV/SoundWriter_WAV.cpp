#include <de/sound/WAV/SoundWriter_WAV.h>

namespace de {
namespace sound {
namespace {

static int wav_format_code(SampleType t)
{
    switch (t) {
    case SampleType::F32:
    case SampleType::F64:
        return 3;   // IEEE float
    default:
        return 1;   // PCM
    }
}

} // end namespace

bool
save_sound_wav(
    const Sound& snd,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    if (snd.empty())
    {
        DE_WARN("Got empty sound. ",uri)
        return false;
    }

    if (options.sampleType != SampleType::Unknown)
    {
        DE_WARN("This exporter ignores a conversions demand from user (yet). ",uri)
    }

    const uint32_t sampleRate   = snd.m_sampleRate;
    const uint16_t channels     = snd.m_channels;
    const uint16_t bits         = snd.bytesPerSample() * 8;
    const uint16_t blockAlign   = (bits / 8) * channels;
    const uint32_t byteRate     = sampleRate * blockAlign;
    const uint32_t dataSize     = (uint32_t)snd.m_samples.size();
    const uint16_t formatCode   = wav_format_code(snd.m_sampleType);

    File file(uri, eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot open WAV file for writing, ",uri)
        return false;
    }

    options.onProgress(1);

    auto w32 = [&](uint32_t v){ file.write(&v, 4); };
    auto w16 = [&](uint16_t v){ file.write(&v, 2); };
    //auto w8  = [&](uint8_t  v){ file.write(&v, 1); };

    // RIFF header
    file.write("RIFF", 4);
    w32(36 + dataSize);          // fileSize - 8
    file.write("WAVE", 4);

    // fmt chunk
    file.write("fmt ", 4);
    w32(16);                     // PCM fmt chunk size
    w16(formatCode);             // PCM=1, IEEE float=3
    w16(channels);
    w32(sampleRate);
    w32(byteRate);
    w16(blockAlign);
    w16(bits);

    // data chunk
    file.write("data", 4);
    w32(dataSize);

    options.onProgress(10);

    // raw PCM bytes (already interleaved)
    file.write(snd.m_samples.data(), snd.m_samples.size());

    options.onProgress(100);

    return true;
}


} // end namespace sound.
} // end namespace de.


/*


void writeWav(const Sound& snd, const std::string& path)
{
    const uint32_t sampleRate   = snd.m_sampleRate;
    const uint16_t channels     = snd.m_channels;
    const uint16_t bits         = snd.bytesPerSample() * 8;
    const uint16_t blockAlign   = (bits / 8) * channels;
    const uint32_t byteRate     = sampleRate * blockAlign;
    const uint32_t dataSize     = (uint32_t)snd.m_samples.size();
    const uint16_t formatCode   = wav_format_code(snd.m_sampleType);

    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) throw std::runtime_error("Cannot open WAV file for writing");

    auto w32 = [&](uint32_t v){ std::fwrite(&v, 4, 1, f); };
    auto w16 = [&](uint16_t v){ std::fwrite(&v, 2, 1, f); };
    auto w8  = [&](uint8_t  v){ std::fwrite(&v, 1, 1, f); };

    // RIFF header
    std::fwrite("RIFF", 4, 1, f);
    w32(36 + dataSize);          // fileSize - 8
    std::fwrite("WAVE", 4, 1, f);

    // fmt chunk
    std::fwrite("fmt ", 4, 1, f);
    w32(16);                     // PCM fmt chunk size
    w16(formatCode);             // PCM=1, IEEE float=3
    w16(channels);
    w32(sampleRate);
    w32(byteRate);
    w16(blockAlign);
    w16(bits);

    // data chunk
    std::fwrite("data", 4, 1, f);
    w32(dataSize);

    // raw PCM bytes (already interleaved)
    std::fwrite(snd.m_samples.data(), 1, snd.m_samples.size(), f);

    std::fclose(f);
}

template <typename T>
void write_wav(
    const std::string& path,
    const T* samples,
    uint32_t frames,
    uint16_t channels,
    uint32_t sampleRate,
    SampleType type)
{
    const int bps = bytes_per_sample(type);
    const uint32_t byteRate = sampleRate * channels * bps;
    const uint16_t blockAlign = channels * bps;
    const uint32_t dataSize = frames * blockAlign;

    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) throw std::runtime_error("cannot open file");

    auto w32 = [&](uint32_t v){ std::fwrite(&v, 4, 1, f); };
    auto w16 = [&](uint16_t v){ std::fwrite(&v, 2, 1, f); };
    auto w8  = [&](uint8_t  v){ std::fwrite(&v, 1, 1, f); };

    // RIFF header
    std::fwrite("RIFF", 4, 1, f);
    w32(36 + dataSize);              // file size - 8
    std::fwrite("WAVE", 4, 1, f);

    // fmt chunk
    std::fwrite("fmt ", 4, 1, f);
    w32(16);                         // PCM fmt chunk size
    w16(wav_format_code(type));      // format code
    w16(channels);
    w32(sampleRate);
    w32(byteRate);
    w16(blockAlign);
    w16(bps * 8);                    // bits per sample

    // data chunk
    std::fwrite("data", 4, 1, f);
    w32(dataSize);

    // Write samples
    if (type == SampleType::S24) {
        // pack 24-bit little endian
        for (uint32_t i = 0; i < frames * channels; i++) {
            int32_t v = samples[i];
            w8((v >> 0) & 0xFF);
            w8((v >> 8) & 0xFF);
            w8((v >> 16) & 0xFF);
        }
    } else {
        // raw little-endian write
        std::fwrite(samples, bps, frames * channels, f);
    }

    std::fclose(f);
}
*/
