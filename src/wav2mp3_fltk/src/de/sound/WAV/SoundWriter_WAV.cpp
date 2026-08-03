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
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    if (sound.empty())
    {
        DE_WARN("Got empty sound. ",uri)
        return false;
    }

    if (options.sampleType != SampleType::Unknown)
    {
        DE_WARN("This exporter ignores a conversions demand from user (yet). ",uri)
    }

    auto srcType = sound.m_sampleType;
    auto dstType = (options.sampleType != SampleType::Unknown) ? options.sampleType : sound.m_sampleType;
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter ",srcType.str()," -> ",dstType.str(),". ", uri)
        return false;
    }

    const uint64_t frames       = sound.m_frames;
    const uint32_t sampleRate   = sound.m_sampleRate;
    const uint16_t channels     = sound.m_channels;
    const uint16_t bits         = dstType.bitsPerSample();
    const uint16_t blockAlign   = (bits / 8) * channels;
    const uint32_t byteRate     = sampleRate * blockAlign;
    const uint32_t dataSize     = (uint32_t)frames * channels * dstType.bytesPerSample();
    const uint16_t formatCode   = wav_format_code(dstType);

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

    options.onProgress(1); // 1%

    // Chunk
    const int32_t bps = dstType.bytesPerSample();
    const int64_t bpf = sound.m_channels * bps;
    const int64_t chunkFrames = 4096;
    const int64_t chunkSamples = chunkFrames * sound.m_channels;
    const int64_t chunkBytes = chunkSamples * bps;
    de::TAlignedVector<uint8_t> chunkBuf(chunkBytes, 0x00);

    // Loop
    int64_t pos = 0; // in [frames]
    while (pos < sound.m_frames)
    {
        auto availFrames = std::min<int64_t>(chunkFrames,sound.m_frames - pos);
        if (availFrames < 1)
        {
            break; // EOF
        }

        int64_t converted = sound.read_frames(
            converter,
            chunkBuf.data(),
            availFrames,
            pos);

        if (converted < 1)
            break;

        const uint8_t* __restrict__ pSrc = reinterpret_cast<const uint8_t*>(chunkBuf.data());
        const int64_t bytesToWrite = converted * bpf;
        const int32_t bytesWritten = file.write(pSrc, bytesToWrite);
        pos += (bytesWritten / bpf);

        options.onProgress(1 + (98.0 * double(pos) / double(sound.m_frames)));
    }

    // Finish
    options.onProgress(100);
    return true;
}


} // end namespace sound.
} // end namespace de.
