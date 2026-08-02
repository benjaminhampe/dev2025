#include <de/sound/MP3/SoundWriter_MP3.h>

#include <lame/lame.h>

namespace de {
namespace sound {

bool
save_sound_mp3(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    if (sound.empty())
    {
        DE_ERROR("Sound empty")
        return false;
    }

    if (sound.m_channels > 2)
    {
        DE_WARN("channels > 2")
    }

    auto srcType = sound.m_sampleType;
    auto dstType = SampleType::F32;
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter to F32, ", sound.str(), ", uri = ", uri)
        return false;
    }

    options.onProgress(1);

    lame_t ctx = lame_init();
    if (!ctx)
    {
        DE_ERROR("!ctx")
        return false;
    }

    // if (is_lame_global_flags_valid(ctx) < 0)
    // {
    //     DE_ERROR("No converter to F32, ", sound.str(), ", uri = ", uri)
    //     return false;
    // }

    lame_set_in_samplerate(ctx, sound.m_sampleRate);
    lame_set_out_samplerate(ctx, sound.m_sampleRate);
    lame_set_num_channels(ctx, sound.m_channels);
    lame_set_num_samples(ctx, sound.m_frames);
    //lame_set_input_format(ctx, LAME_INPUT_FLOAT);

    //lame_set_brate( m_lame, bitRate );
    if (sound.m_channels == 1 )
    {
        lame_set_mode(ctx, MONO );
    }
    else if (sound.m_channels == 2 )
    {
        lame_set_mode(ctx, STEREO );
    }

    bool options_useVBR = false;
    if (options_useVBR)
    {
        lame_set_VBR(ctx, vbr_default);
        lame_set_brate(ctx, options.bitrate);
        lame_set_VBR_q(ctx, options.quality); // 0 (highest) to 9 (lowest)
        //lame_set_VBR_min_bitrate_kbps( m_lame, 64 );
        //lame_set_VBR_max_bitrate_kbps( m_lame, 192 );
    }
    else
    {
        lame_set_VBR(ctx, vbr_off);
        lame_set_brate(ctx, options.bitrate);
        lame_set_quality(ctx, 2); // options.quality); // 0-best, 9-worst
    }

    lame_set_disable_reservoir(ctx, 1); //bit reservoir has to be disabled for seamless streaming
    // lame_set_bWriteVbrTag(ctx, 1);
    //lame_set_asm_optimizations()

    if (lame_init_params(ctx) < 0)
    {
        DE_ERROR("lame_init_params() failed. ", uri)
        lame_close(ctx);
        return false;
    }

    File file(uri,eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot write MP3 ", uri)
        lame_close(ctx);
        return false;
    }

    const int chunkFrames = lame_get_framesize(ctx);
    const int chunkSamples = chunkFrames * sound.m_channels;
    de::TAlignedVector<float> chunkBuf(chunkSamples, 0.0f);

    const int encodeBytes = static_cast<size_t>(1.25 * chunkSamples + 7200); // lame_get_size_mp3buffer(ctx);
    de::TAlignedVector<uint8_t> encodeBuf(encodeBytes, 0x00);

    DE_DEBUG("chunkFrames(",chunkFrames,")")
    DE_DEBUG("encodeBytes(",encodeBytes,")")

    int64_t frameCount = sound.m_frames;
    int64_t frameIndex = 0;

    bool bCancelFlag = false;

    int i = 0;

    while (!bCancelFlag && frameIndex < frameCount)
    {
        const int64_t availFrames = frameCount - frameIndex;
        const int64_t queryFrames = std::min<int64_t>(availFrames,chunkFrames);

        int64_t converted = sound.read_frames(
            converter,
            chunkBuf.data(),
            queryFrames,
            frameIndex);

        if (converted < 1)
            break;

        frameIndex += converted;

        options.onProgress(1 + std::lround(97.0 *
                    double(frameIndex) / double(frameCount)));

        const int encoded = lame_encode_buffer_interleaved_ieee_float(
                ctx,
                chunkBuf.data(),
                static_cast<int>(converted),
                encodeBuf.data(),
                static_cast<int>(encodeBuf.size())
            );

        if (encoded > 0)
        {
            if (encoded > encodeBuf.size())
            {
                DE_ERROR("encoded(",encoded,") > encodeBuf(",encodeBuf.size(),")")
            }
            else
            {
                file.write(encodeBuf.data(), encoded);
            }
        }

        DE_BENNI("i(",i,"), "
                 "frameIndex(",frameIndex,"), "
                 "framesCount(",sound.m_frames,"), "
                 "converted(",converted,"), "
                 "encoded(",encoded,")")

        i++;
    }

    options.onProgress(99);

    // 🔥 Flush
    const int encoded = lame_encode_flush(
                        ctx,
                        encodeBuf.data(),
                        static_cast<int>(encodeBuf.size())
                    );
    if (encoded > 0)
    {
        if (encoded > encodeBuf.size())
        {
            DE_ERROR("encoded(",encoded,") > encodeBuf(",encodeBuf.size(),")")
        }
        else
        {
            file.write(encodeBuf.data(), encoded);
        }
        DE_BENNI("encoded(",encoded,")")
        DE_BENNI("encodeBuf(",encodeBuf.size(),")")
    }

    // Write final VBR / Xing Header tag to the beginning of the stream
    // de::TAlignedVector<uint8_t> vbrTagBuf(7200, 0x00);
    // size_t tagSize = lame_get_lametag_frame(ctx, vbrTagBuf.data(), vbrTagBuf.size());
    // if (tagSize > 0)
    // {
    //     file.seek(0);
    //     file.write(vbrTagBuf.data(), tagSize);
    // }

    lame_close(ctx);
    options.onProgress(100);
    return true;
}

} // end namespace sound.
} // end namespace de.


#if 0

bool save_mp3_from_interleaved_f32(
    const float* pcmInterleaved,   // [frames * channels]
    int64_t      frameCount,
    int          sampleRate,
    int          channels,
    int          bitrateKbps,
    const char*  outPath)
{
    if (!pcmInterleaved || frameCount <= 0 || channels < 1 || !outPath)
        return false;

    lame_t gfp = lame_init();
    if (!gfp)
        return false;

    // Encoder configuration
    lame_set_in_samplerate(gfp, sampleRate);
    lame_set_num_channels(gfp, channels);
    lame_set_brate(gfp, bitrateKbps);
    lame_set_quality(gfp, 2); // high quality

    // IMPORTANT: enable float input
    lame_set_input_format(gfp, LAME_INPUT_FLOAT);

    if (lame_init_params(gfp) < 0)
    {
        lame_close(gfp);
        return false;
    }

    std::ofstream fout(outPath, std::ios::binary);
    if (!fout.is_open())
    {
        lame_close(gfp);
        return false;
    }

    // LAME tells us the correct MP3 buffer size
    const int mp3BufferSize = lame_get_size_mp3buffer(gfp);
    std::vector<unsigned char> mp3Buffer(mp3BufferSize);



    // LAME tells us how many samples per channel per encode call
    const int lameFrameSize = lame_get_framesize(gfp);

    int64_t framesDone = 0;

    while (framesDone < frameCount)
    {
        const int64_t framesLeft = frameCount - framesDone;
        const int64_t framesThis = (framesLeft > lameFrameSize) ? lameFrameSize : framesLeft;

        const float* pcmChunk = pcmInterleaved + framesDone * channels;

        const int bytesOut = lame_encode_buffer_interleaved_ieee_float(
            gfp,
            pcmChunk,
            static_cast<int>(framesThis), // frames per channel
            mp3Buffer.data(),
            mp3BufferSize);

        if (bytesOut < 0)
        {
            lame_close(gfp);
            return false;
        }

        if (bytesOut > 0)
        {
            fout.write(reinterpret_cast<const char*>(mp3Buffer.data()), bytesOut);
            if (!fout.good())
            {
                lame_close(gfp);
                return false;
            }
        }

        framesDone += framesThis;
    }

    // Flush remaining MP3 data
    const int flushBytes = lame_encode_flush_nogap(
        gfp,
        mp3Buffer.data(),
        mp3BufferSize);

    if (flushBytes < 0)
    {
        lame_close(gfp);
        return false;
    }

    if (flushBytes > 0)
    {
        fout.write(reinterpret_cast<const char*>(mp3Buffer.data()), flushBytes);
        if (!fout.good())
        {
            lame_close(gfp);
            return false;
        }
    }

    fout.close();
    lame_close(gfp);
    return true;
}





#endif






/*



auto srcType = sound.m_sampleType;
    auto dstType = SampleType::F32;
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter to F32, ", sound.str(), ", uri = ", uri)
        return false;
    }

    options.onProgress(1);

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

    const int64_t FRAMES = 1152 * 16; // 1152 * 16;
    const int64_t SAMPLES = FRAMES * sound.m_channels;


    de::TAlignedVector<float> pcmBuf(SAMPLES);
    de::TAlignedVector<uint8_t> mp3Buf(int(1.25*FRAMES) + 7200);

    std::size_t frameCount = sound.m_frames; // sound.m_samples.size() / sound.m_channels;
    std::size_t frameIndex = 0;

    bool bCancelFlag = false;

    int i = 0;

    while (!bCancelFlag)
    {
        int64_t framesRead = sound.read_frames(
            converter,
            pcmBuf.data(),
            FRAMES,
            frameIndex);

        if (framesRead < 1)
            break;

        frameIndex += framesRead;
        if (frameIndex > frameCount)
            frameIndex = frameCount;

        options.onProgress(1 + std::lround(97.0 *
                    double(frameIndex) / double(frameCount)));

        int mp3Bytes = lame_encode_buffer_interleaved_ieee_float(
                ctx,
                pcmBuf.data(),
                (int)framesRead,
                mp3Buf.data(),
                int(mp3Buf.size())
            );

        if (mp3Bytes > 0)
        {
            if (mp3Bytes > mp3Buf.size())
            {
                DE_ERROR("mp3Bytes(",mp3Bytes,") > mp3Buf(",mp3Buf.size(),")")
                mp3Bytes = mp3Buf.size();
            }
            file.write(mp3Buf.data(), mp3Bytes);
        }

        DE_BENNI("i(",i,"), "
                 "frameIndex(",frameIndex,"), "
                 "framesCount(",sound.m_frames,"), "
                 "framesRead(",framesRead,"), "
                 "mp3Bytes(",mp3Bytes,")")

        i++;
    }

    options.onProgress(99);

    // 🔥 Correct flush size parameter
    // int mp3BufSize = int(7200); // flush only needs 7200 bytes

    int mp3Bytes = lame_encode_flush(
                        ctx,
                        mp3Buf.data(),
                        int(mp3Buf.size()));
    if (mp3Bytes > 0)
    {
        if (mp3Bytes > mp3Buf.size())
        {
            DE_ERROR("mp3Bytes(",mp3Bytes,") > flushBuf.size(",mp3Buf.size(),")")
            mp3Bytes = mp3Buf.size();
        }

        DE_BENNI("mp3Bytes(",mp3Bytes,")")
        DE_BENNI("mp3Buf.size(",mp3Buf.size(),")")

        file.write(mp3Buf.data(), mp3Bytes);
    }

    lame_close(ctx);
    options.onProgress(100);
    return true;





    while (!bCancelFlag)
    {
        int64_t framesRead = sound.read_frames(converter, chunk.data(), FRAMES, frameIndex);
        if (framesRead < 1)
            break;

        frameIndex += framesRead;
        if (frameIndex > frameCount)
            frameIndex = frameCount;

        // Resize MP3 buffer for THIS chunk
        size_t mp3Size = size_t(1.25 * framesRead * sound.m_channels + 7200);
        mp3Buf.resize(mp3Size);

        int bytes = lame_encode_buffer_interleaved_ieee_float(
            ctx,
            chunk.data(),
            (int)framesRead,
            mp3Buf.data(),
            (int)mp3Buf.size()
        );

        if (bytes > 0)
            file.write(mp3Buf.data(), bytes);
    }

    // Large flush buffer
    std::vector<uint8_t> flushBuf(7200 + size_t(1.25 * sound.m_frames * sound.m_channels));
    int flushBytes = lame_encode_flush(ctx, flushBuf.data(), flushBuf.size());
    if (flushBytes > 0)
        file.write(flushBuf.data(), flushBytes);

*/


/*

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
    }

    options.onProgress(99);

    int flushBytes = lame_encode_flush(ctx, mp3Buf.data(), (int)mp3Buf.size());
    if (flushBytes > 0)
        file.write(mp3Buf.data(), flushBytes);

*/
