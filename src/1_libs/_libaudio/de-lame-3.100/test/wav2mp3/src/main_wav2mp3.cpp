// main.cpp — Single-buffer adaptive WAV→MP3 streaming
// Efficient, format-aware, uses lame_encode_buffer_interleaved_ieee_float
// when WAV is float, otherwise uses lame_encode_buffer_interleaved.

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdint>
#include <lame/lame.h>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

static bool convert(const char* inWav,
                    const char* outMp3,
                    int bitrate,
                    int lameQuality)
{
    drwav wav;
    if (!drwav_init_file(&wav, inWav, nullptr)) {
        std::cerr << "Cannot open WAV\n";
        return false;
    }

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, wav.sampleRate);
    lame_set_num_channels(lame, wav.channels);
    lame_set_brate(lame, bitrate);
    lame_set_quality(lame, lameQuality);
    lame_init_params(lame);

    FILE* out = fopen(outMp3, "wb");
    if (!out) {
        std::cerr << "Cannot open MP3 output\n";
        drwav_uninit(&wav);
        lame_close(lame);
        return false;
    }

    const size_t FRAMES = 1152 * 16;
    const size_t SAMPLES = FRAMES * wav.channels;

    // Single raw byte buffer
    std::vector<uint8_t> raw(SAMPLES * wav.bitsPerSample / 8);

    // MP3 output buffer
    std::vector<unsigned char> mp3Buf(1.25 * SAMPLES + 7200);

    bool isFloat = (wav.translatedFormatTag == DR_WAVE_FORMAT_IEEE_FLOAT);
    bool isPCM16 = (wav.bitsPerSample == 16 && wav.translatedFormatTag == DR_WAVE_FORMAT_PCM);

    while (true) {
        size_t framesRead = drwav_read_pcm_frames(&wav, FRAMES, raw.data());
        if (framesRead == 0)
            break;

        int bytes = 0;

        if (isFloat) {
            // Interpret raw bytes as float32
            float* f = reinterpret_cast<float*>(raw.data());
            bytes = lame_encode_buffer_interleaved_ieee_float(
                lame,
                f,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }
        else if (isPCM16) {
            // Interpret raw bytes as int16_t
            int16_t* s = reinterpret_cast<int16_t*>(raw.data());
            bytes = lame_encode_buffer_interleaved(
                lame,
                s,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }
        else {
            // Other formats: convert in-place into float32
            float* f = reinterpret_cast<float*>(raw.data());
            size_t samples = framesRead * wav.channels;

            if (wav.bitsPerSample == 24)
            {
                // 24-bit → float
                for (size_t i = 0; i < samples; i++)
                {
                    uint8_t* p = raw.data() + i * 3;
                    int32_t v = (p[0] | (p[1] << 8) | (p[2] << 16));
                    if (v & 0x800000) v |= ~0xFFFFFF;
                    f[i] = (float)v / 8388607.0f;
                }
            }
            else if (wav.bitsPerSample == 32 && wav.translatedFormatTag == DR_WAVE_FORMAT_PCM)
            {
                int32_t* p = reinterpret_cast<int32_t*>(raw.data());
                for (size_t i = 0; i < samples; i++)
                    f[i] = (float)p[i] / 2147483647.0f;
            }
            else
            {
                std::cerr << "Unsupported WAV format\n";
                fclose(out);
                drwav_uninit(&wav);
                lame_close(lame);
                return false;
            }

            bytes = lame_encode_buffer_interleaved_ieee_float(
                lame,
                f,
                (int)framesRead,
                mp3Buf.data(),
                (int)mp3Buf.size()
            );
        }

        if (bytes > 0)
            fwrite(mp3Buf.data(), 1, bytes, out);
    }

    int flushBytes = lame_encode_flush(lame, mp3Buf.data(), (int)mp3Buf.size());
    if (flushBytes > 0)
        fwrite(mp3Buf.data(), 1, flushBytes, out);

    fclose(out);
    drwav_uninit(&wav);
    lame_close(lame);

    return true;
}

int main(int argc, char** argv)
{
    if (argc < 5) {
        std::cout << "Usage: wav2mp3 <in.wav> <out.mp3> <bitrate> <quality>\n";
        return 1;
    }

    if (!convert(argv[1], argv[2], std::stoi(argv[3]), std::stoi(argv[4])))
        return 1;

    return 0;
}
