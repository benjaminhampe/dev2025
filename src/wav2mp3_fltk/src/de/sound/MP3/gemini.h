/*

#include <lame/lame.h>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>

bool save_sound_mp3(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    if (sound.m_frames <= 0 || sound.m_sampleRate <= 0 || sound.m_channels <= 0 || sound.m_samples.empty()) {
        return false;
    }

    // 1. Initialize LAME encoder
    lame_global_flags* gfp = lame_init();
    if (!gfp) {
        return false;
    }

    lame_set_in_samplerate(gfp, sound.m_sampleRate);
    lame_set_num_channels(gfp, sound.m_channels);
    lame_set_out_samplerate(gfp, sound.m_sampleRate);

    // High quality mode (0 = best/slowest, 9 = worst/fastest)
    lame_set_quality(gfp, 2); 

    if (lame_init_params(gfp) < 0) {
        lame_close(gfp);
        return false;
    }

    // 2. Open output file
    std::ofstream outFile(uri, std::ios::binary);
    if (!outFile.is_open()) {
        lame_close(gfp);
        return false;
    }

    const float* floatSamples = reinterpret_cast<const float*>(sound.m_samples.data());

    // 3. Chunked Encoding
    const size_t kChunkFrames = 4096;
    // LAME recommended MP3 output buffer size formula: 1.25 * num_samples + 7200
    const size_t mp3BufferSize = static_cast<size_t>(1.25 * (kChunkFrames * sound.m_channels) + 7200);
    std::vector<uint8_t> mp3Buffer(mp3BufferSize);

    size_t framesRemaining = static_cast<size_t>(sound.m_frames);
    size_t frameOffset = 0;

    while (framesRemaining > 0) {
        int framesToProcess = static_cast<int>(std::min(framesRemaining, kChunkFrames));
        const float* chunkPtr = floatSamples + (frameOffset * sound.m_channels);

        int bytesEncoded = lame_encode_buffer_interleaved_ieee_float(
            gfp,
            chunkPtr,
            framesToProcess,
            mp3Buffer.data(),
            static_cast<int>(mp3Buffer.size()));

        if (bytesEncoded < 0) {
            lame_close(gfp);
            return false;
        }

        if (bytesEncoded > 0) {
            outFile.write(reinterpret_cast<const char*>(mp3Buffer.data()), bytesEncoded);
        }

        frameOffset += framesToProcess;
        framesRemaining -= framesToProcess;
    }

    // 4. Flush remaining frames in the encoder
    int flushBytes = lame_encode_flush(gfp, mp3Buffer.data(), static_cast<int>(mp3Buffer.size()));
    if (flushBytes > 0) {
        outFile.write(reinterpret_cast<const char*>(mp3Buffer.data()), flushBytes);
    }

    // 5. Cleanup
    lame_close(gfp);
    outFile.close();

    return true;
}

*/