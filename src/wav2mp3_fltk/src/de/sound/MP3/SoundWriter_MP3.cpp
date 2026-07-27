#include <de/sound/MP3/SoundWriter_MP3.h>

#include <lame/lame.h>

namespace de {
namespace sound {

bool
save_sound_mp3_f32(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    auto srcType = sound.m_sampleType;
    auto dstType = SampleType::F32;
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter to F32, ", sound.str(), ", uri = ", uri)
        return false;
    }

    // if (sound.m_sampleType != SampleType::F32)
    // {
    //     DE_ERROR("Only F32 supported (yet) ", uri)
    //     return false;
    // }

    options.onProgress(1);
    //Fl::awake(convert_start_awake, nullptr);

    // New Async
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // ui.cancelFlag = false;

    lame_t ctx = lame_init();
    lame_set_in_samplerate(ctx, sound.m_sampleRate);
    lame_set_num_channels(ctx, sound.m_channels);
    lame_set_brate(ctx, options.bitrate);
    lame_set_quality(ctx, options.quality);
    lame_init_params(ctx);

    File file(uri,eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot write MP3 ", uri)
        lame_close(ctx);
        return false;
    }

    const int64_t FRAMES = 1152 * 16;
    const int64_t SAMPLES = FRAMES * sound.m_channels;

    // Single raw byte buffer
    de::TAlignedVector<float> chunk(SAMPLES);

    // MP3 output buffer
    de::TAlignedVector<uint8_t> mp3Buf(1.25 * SAMPLES + 7200);

    std::size_t frameCount = sound.m_frames;
    std::size_t frameIndex = 0;

    bool bCancelFlag = false;

    while (!bCancelFlag)
    {
        int64_t framesRead = sound.read_frames(converter, chunk.data(), FRAMES, frameIndex);
        if (framesRead < 1)
            break;

        // new
        frameIndex += framesRead;
        if (frameIndex > frameCount)
            frameIndex = frameCount;

        int percent = 1 + std::lroundf(97.0f * static_cast<float>(frameIndex) / static_cast<float>(frameCount));
        options.onProgress(percent);
        //Fl::awake(convert_progress_awake, new float(percent));

        int bytes = 0;

        // Interpret raw bytes as float32
        bytes = lame_encode_buffer_interleaved_ieee_float(
            ctx,
            chunk.data(),
            (int)framesRead,
            mp3Buf.data(),
            (int)mp3Buf.size()
        );

        if (bytes > 0)
            file.write(mp3Buf.data(), bytes);

        // New Async
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    options.onProgress(99);
    //Fl::awake(convert_progress_awake, new float(1.0f));

    int flushBytes = lame_encode_flush(ctx, mp3Buf.data(), (int)mp3Buf.size());
    if (flushBytes > 0)
        file.write(mp3Buf.data(), flushBytes);

    lame_close(ctx);

    // New Async
    //Fl::awake(convert_finished_awake, nullptr);
    options.onProgress(100);

    return true;
}

} // end namespace sound.
} // end namespace de.
