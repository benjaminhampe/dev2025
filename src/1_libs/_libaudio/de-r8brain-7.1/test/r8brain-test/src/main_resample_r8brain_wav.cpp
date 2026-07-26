#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <memory>

#include "r8bbase.h"
#include "CDSPResampler.h"

// ------------------------------------------------------------
// Sound (interleaved float)
// ------------------------------------------------------------
struct Sound {
    int sampleRate = 0;
    int channels   = 0;
    std::vector<float> samples; // interleaved
};

// ------------------------------------------------------------
// Robust WAV loader: 16/24/32-bit PCM + float32 + extensible
// Always outputs float32 interleaved
// ------------------------------------------------------------
static void load_wav_any(const char* path, Sound& out) {
    FILE* f = fopen(path, "rb");
    if (!f) throw std::runtime_error("cannot open wav");

    auto read_u32 = [&](uint32_t& v){ fread(&v,4,1,f); };
    auto read_u16 = [&](uint16_t& v){ fread(&v,2,1,f); };

    char id[4];
    fread(id,1,4,f);
    if (memcmp(id,"RIFF",4)!=0) throw std::runtime_error("not RIFF");

    fseek(f, 8, SEEK_SET);
    fread(id,1,4,f);
    if (memcmp(id,"WAVE",4)!=0) throw std::runtime_error("not WAVE");

    uint16_t audioFormat = 0;
    uint16_t channels    = 0;
    uint32_t sampleRate  = 0;
    uint16_t bitsPerSample = 0;
    uint32_t dataSize    = 0;

    fseek(f, 12, SEEK_SET);
    while (true) {
        if (fread(id,1,4,f) != 4) throw std::runtime_error("invalid wav");
        uint32_t size; read_u32(size);

        if (memcmp(id,"fmt ",4)==0) {
            read_u16(audioFormat);
            read_u16(channels);
            read_u32(sampleRate);

            uint32_t byteRate; read_u32(byteRate);
            uint16_t blockAlign; read_u16(blockAlign);
            read_u16(bitsPerSample);

            if (audioFormat == 0xFFFE) {
                uint16_t extSize; read_u16(extSize);
                fseek(f, extSize, SEEK_CUR);
            } else {
                fseek(f, size - 16, SEEK_CUR);
            }
        }
        else if (memcmp(id,"data",4)==0) {
            dataSize = size; // HIER: size ist dataSize
            break;
        }
        else {
            fseek(f, size, SEEK_CUR);
        }
    }

    out.channels   = channels;
    out.sampleRate = sampleRate;

    const int frames = dataSize / (bitsPerSample/8) / channels;
    out.samples.resize(frames * channels);

    if (audioFormat == 1) { // PCM
        if (bitsPerSample == 16) {
            for (int i = 0; i < frames * channels; i++) {
                int16_t v;
                fread(&v,2,1,f);
                out.samples[i] = float(v / 32768.0f);
            }
        }
        else if (bitsPerSample == 24) {
            for (int i = 0; i < frames * channels; i++) {
                uint8_t b[3];
                fread(b,1,3,f);
                int32_t v = (int32_t(b[0])      ) |
                            (int32_t(b[1]) << 8 ) |
                            (int32_t(b[2]) << 16);
                // sign extend 24-bit
                if (v & 0x00800000) v |= 0xFF000000;
                out.samples[i] = float(v / 2147483648.0f);
            }
        }
        else if (bitsPerSample == 32) {
            for (int i = 0; i < frames * channels; i++) {
                int32_t v;
                fread(&v,4,1,f);
                out.samples[i] = float(v / 2147483648.0f);
            }
        }
        else {
            throw std::runtime_error("unsupported PCM bit depth");
        }
    }
    else if (audioFormat == 3) { // float32
        fread(out.samples.data(), sizeof(float), frames * channels, f);
    }
    else {
        throw std::runtime_error("unsupported WAV format");
    }

    fclose(f);
}

// ------------------------------------------------------------
// WAV speichern (float32, interleaved)
// ------------------------------------------------------------
static void save_wav_float(const char* path, const Sound& in) {
    FILE* f = fopen(path, "wb");
    if (!f) throw std::runtime_error("cannot write wav");

    uint32_t dataSize = in.samples.size() * sizeof(float);
    uint32_t fmtSize  = 16;
    uint32_t riffSize = 4 + (8 + fmtSize) + (8 + dataSize);

    fwrite("RIFF",1,4,f);
    fwrite(&riffSize,4,1,f);
    fwrite("WAVE",1,4,f);

    fwrite("fmt ",1,4,f);
    fwrite(&fmtSize,4,1,f);

    uint16_t audioFormat = 3; // float32
    fwrite(&audioFormat,2,1,f);

    uint16_t ch = in.channels;
    fwrite(&ch,2,1,f);

    uint32_t sr = in.sampleRate;
    fwrite(&sr,4,1,f);

    uint32_t byteRate = sr * ch * sizeof(float);
    fwrite(&byteRate,4,1,f);

    uint16_t blockAlign = ch * sizeof(float);
    fwrite(&blockAlign,2,1,f);

    uint16_t bps = 32;
    fwrite(&bps,2,1,f);

    fwrite("data",1,4,f);
    fwrite(&dataSize,4,1,f);
    fwrite(in.samples.data(), sizeof(float), in.samples.size(), f);

    fclose(f);
}

// ------------------------------------------------------------
// Deinterleave float -> planar double
// ------------------------------------------------------------
static void deinterleave_float_to_double(const Sound& in,
                                         int startFrame, int frames,
                                         std::vector<std::vector<double>>& outCh)
{
    int chs = in.channels;
    outCh.assign(chs, std::vector<double>(frames));

    const float* src = in.samples.data();

    for (int f = 0; f < frames; f++) {
        int base = (startFrame + f) * chs;
        for (int ch = 0; ch < chs; ch++) {
            outCh[ch][f] = double(src[base + ch]);
        }
    }
}

// ------------------------------------------------------------
// Interleave double -> float append
// ------------------------------------------------------------
static void interleave_double_to_float_append(Sound& out,
                                              const std::vector<std::vector<double>>& inCh,
                                              int frames)
{
    int chs = out.channels;
    out.samples.reserve(out.samples.size() + frames * chs);

    for (int f = 0; f < frames; f++) {
        for (int ch = 0; ch < chs; ch++) {
            out.samples.push_back(float(inCh[ch][f]));
        }
    }
}

// ------------------------------------------------------------
// Chunkweises Resampling (r8brain 7.1: double* + double*&)
// ------------------------------------------------------------
static void resampleSoundChunkwise(const Sound& in, Sound& out,
                                   double dstRate, int chunkSize)
{
    const int    chs         = in.channels;
    const double srcRate     = in.sampleRate;
    const double ratio       = dstRate / srcRate;
    const int    totalFrames = int(in.samples.size() / chs);

    std::vector<std::unique_ptr<r8b::CDSPResampler>> resamplers;
    resamplers.reserve(chs);

    for (int ch = 0; ch < chs; ch++) {
        resamplers.emplace_back(
            std::make_unique<r8b::CDSPResampler>(
                srcRate,
                dstRate,
                chunkSize,      // maxBlockSize == chunkSize
                2.0,
                206.91,
                r8b::fprLinearPhase
            )
        );
    }

    out.channels   = chs;
    out.sampleRate = int(dstRate);
    out.samples.clear();

    int pos = 0;
    while (pos < totalFrames) {
        int frames = std::min(chunkSize, totalFrames - pos);

        std::vector<std::vector<double>> inCh;
        deinterleave_float_to_double(in, pos, frames, inCh);

        int outCap = int(std::ceil(frames * ratio)) + 16;
        std::vector<std::vector<double>> outCh(chs, std::vector<double>(outCap));

        int outFrames = 0;

        for (int ch = 0; ch < chs; ch++) {
            double* outPtr = nullptr;

            outFrames = resamplers[ch]->process(
                inCh[ch].data(), // double*
                frames,          // <= maxBlockSize
                outPtr           // double*& (r8brain sets this)
            );

            if (outFrames > 0) {
                for (int i = 0; i < outFrames; i++) {
                    outCh[ch][i] = outPtr[i];
                }
            }
        }

        if (outFrames > 0) {
            interleave_double_to_float_append(out, outCh, outFrames);
        }

        pos += frames;
    }
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main() {
    Sound in;
    load_wav_any("C:/_media/Music/wav/piano-space.wav", in);

    Sound out;
    resampleSoundChunkwise(in, out, 48000.0, 512);

    save_wav_float("C:/_media/Music/wav/piano-space-r8brain_48kHz.wav", out);

    return 0;
}

#if 0

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <memory>

#include "r8bbase.h"
#include "CDSPResampler.h"

// ------------------------------------------------------------
// Sound (interleaved float)
// ------------------------------------------------------------
struct Sound {
    int sampleRate = 0;
    int channels   = 0;
    std::vector<float> samples; // interleaved
};

static void load_wav_any(const char* path, Sound& out) {
    FILE* f = fopen(path, "rb");
    if (!f) throw std::runtime_error("cannot open wav");

    auto read_u32 = [&](uint32_t& v){ fread(&v,4,1,f); };
    auto read_u16 = [&](uint16_t& v){ fread(&v,2,1,f); };

    char id[4];
    fread(id,1,4,f);
    if (memcmp(id,"RIFF",4)!=0) throw std::runtime_error("not RIFF");

    fseek(f, 8, SEEK_SET);
    fread(id,1,4,f);
    if (memcmp(id,"WAVE",4)!=0) throw std::runtime_error("not WAVE");

    // fmt chunk suchen
    uint16_t audioFormat = 0;
    uint16_t channels = 0;
    uint32_t sampleRate = 0;
    uint16_t bitsPerSample = 0;

    fseek(f, 12, SEEK_SET);
    while (true) {
        fread(id,1,4,f);
        uint32_t size; read_u32(size);

        if (memcmp(id,"fmt ",4)==0) {
            read_u16(audioFormat);
            read_u16(channels);
            read_u32(sampleRate);

            uint32_t byteRate; read_u32(byteRate);
            uint16_t blockAlign; read_u16(blockAlign);
            read_u16(bitsPerSample);

            // WAVE_FORMAT_EXTENSIBLE
            if (audioFormat == 0xFFFE) {
                uint16_t extSize; read_u16(extSize);
                fseek(f, extSize, SEEK_CUR);
            } else {
                fseek(f, size - 16, SEEK_CUR);
            }
        }
        else if (memcmp(id,"data",4)==0) {
            break;
        }
        else {
            fseek(f, size, SEEK_CUR);
        }
    }

    out.channels = channels;
    out.sampleRate = sampleRate;

    uint32_t dataSize;
    fread(&dataSize,4,1,f);

    const int frames = dataSize / (bitsPerSample/8) / channels;
    out.samples.resize(frames * channels);

    // PCM → float32
    if (audioFormat == 1) { // PCM
        if (bitsPerSample == 16) {
            for (int i = 0; i < frames * channels; i++) {
                int16_t v;
                fread(&v,2,1,f);
                out.samples[i] = float(v / 32768.0f);
            }
        }
        else if (bitsPerSample == 24) {
            for (int i = 0; i < frames * channels; i++) {
                uint8_t b[3];
                fread(b,1,3,f);
                int32_t v = (b[2] << 24) | (b[1] << 16) | (b[0] << 8);
                out.samples[i] = float(v / 2147483648.0f);
            }
        }
        else if (bitsPerSample == 32) {
            for (int i = 0; i < frames * channels; i++) {
                int32_t v;
                fread(&v,4,1,f);
                out.samples[i] = float(v / 2147483648.0f);
            }
        }
        else {
            throw std::runtime_error("unsupported PCM bit depth");
        }
    }
    else if (audioFormat == 3) { // float32
        fread(out.samples.data(), sizeof(float), frames * channels, f);
    }
    else {
        throw std::runtime_error("unsupported WAV format");
    }

    fclose(f);
}

// ------------------------------------------------------------
// WAV laden (float32, interleaved)
// ------------------------------------------------------------
static void load_wav_float(const char* path, Sound& out) {
    FILE* f = fopen(path, "rb");
    if (!f) throw std::runtime_error("cannot open wav");

    char riff[4];
    fread(riff,1,4,f);
    if (memcmp(riff,"RIFF",4)!=0) throw std::runtime_error("not RIFF");

    fseek(f, 22, SEEK_SET);
    uint16_t ch; fread(&ch,2,1,f);
    out.channels = ch;

    uint32_t sr; fread(&sr,4,1,f);
    out.sampleRate = sr;

    fseek(f, 34, SEEK_SET);
    uint16_t bps; fread(&bps,2,1,f);
    if (bps != 32) throw std::runtime_error("only float32 supported");

    // data chunk suchen
    fseek(f, 12, SEEK_SET);
    char id[4];
    uint32_t size;
    while (true) {
        fread(id,1,4,f);
        fread(&size,4,1,f);
        if (memcmp(id,"data",4)==0) break;
        fseek(f, size, SEEK_CUR);
    }

    size_t samples = size / sizeof(float);
    out.samples.resize(samples);
    fread(out.samples.data(), sizeof(float), samples, f);
    fclose(f);
}

// ------------------------------------------------------------
// WAV speichern (float32, interleaved)
// ------------------------------------------------------------
static void save_wav_float(const char* path, const Sound& in) {
    FILE* f = fopen(path, "wb");
    if (!f) throw std::runtime_error("cannot write wav");

    uint32_t dataSize = in.samples.size() * sizeof(float);
    uint32_t fmtSize  = 16;
    uint32_t riffSize = 4 + (8 + fmtSize) + (8 + dataSize);

    fwrite("RIFF",1,4,f);
    fwrite(&riffSize,4,1,f);
    fwrite("WAVE",1,4,f);

    fwrite("fmt ",1,4,f);
    fwrite(&fmtSize,4,1,f);

    uint16_t audioFormat = 3; // float32
    fwrite(&audioFormat,2,1,f);

    uint16_t ch = in.channels;
    fwrite(&ch,2,1,f);

    uint32_t sr = in.sampleRate;
    fwrite(&sr,4,1,f);

    uint32_t byteRate = sr * ch * sizeof(float);
    fwrite(&byteRate,4,1,f);

    uint16_t blockAlign = ch * sizeof(float);
    fwrite(&blockAlign,2,1,f);

    uint16_t bps = 32;
    fwrite(&bps,2,1,f);

    fwrite("data",1,4,f);
    fwrite(&dataSize,4,1,f);
    fwrite(in.samples.data(), sizeof(float), in.samples.size(), f);

    fclose(f);
}

// ------------------------------------------------------------
// Deinterleave float -> planar double
// ------------------------------------------------------------
static void deinterleave_float_to_double(const Sound& in,
                                         int startFrame, int frames,
                                         std::vector<std::vector<double>>& outCh)
{
    int chs = in.channels;
    outCh.assign(chs, std::vector<double>(frames));

    const float* src = in.samples.data();

    for (int f = 0; f < frames; f++) {
        int base = (startFrame + f) * chs;
        for (int ch = 0; ch < chs; ch++) {
            outCh[ch][f] = double(src[base + ch]);
        }
    }
}

// ------------------------------------------------------------
// Interleave double -> float append
// ------------------------------------------------------------
static void interleave_double_to_float_append(Sound& out,
                                              const std::vector<std::vector<double>>& inCh,
                                              int frames)
{
    int chs = out.channels;
    out.samples.reserve(out.samples.size() + frames * chs);

    for (int f = 0; f < frames; f++) {
        for (int ch = 0; ch < chs; ch++) {
            out.samples.push_back(float(inCh[ch][f]));
        }
    }
}

// ------------------------------------------------------------
// Chunkweises Resampling (r8brain 7.1: double* required)
// ------------------------------------------------------------
static void resampleSoundChunkwise(const Sound& in, Sound& out,
                                   double dstRate, int chunkSize)
{
    const int chs      = in.channels;
    const double srcRate = in.sampleRate;
    const double ratio    = dstRate / srcRate;
    const int totalFrames = int(in.samples.size() / chs);

    // Resampler pro Kanal (NICHT kopierbar → unique_ptr)
    std::vector<std::unique_ptr<r8b::CDSPResampler>> resamplers;
    resamplers.reserve(chs);

    for (int ch = 0; ch < chs; ch++) {
        resamplers.emplace_back(
            std::make_unique<r8b::CDSPResampler>(
                srcRate,
                dstRate,
                chunkSize,
                2.0,
                206.91,
                r8b::fprLinearPhase
            )
        );
    }

    out.channels   = chs;
    out.sampleRate = int(dstRate);
    out.samples.clear();

    int pos = 0;
    while (pos < totalFrames) {
        int frames = std::min(chunkSize, totalFrames - pos);

        // --- float -> double (planar) ---
        std::vector<std::vector<double>> inCh;
        deinterleave_float_to_double(in, pos, frames, inCh);

        // --- Output planar double ---
        int outCap = int(std::ceil(frames * ratio)) + 16;
        std::vector<std::vector<double>> outCh(chs, std::vector<double>(outCap));

        // --- r8brain process() ---
        int outFrames = 0;
        for (int ch = 0; ch < chs; ch++) {

            double* outPtr = outCh[ch].data();   // LVALUE pointer
            outFrames = resamplers[ch]->process(
                inCh[ch].data(),   // double*
                frames,
                outPtr             // double*&
            );
        }

        // --- double -> float (interleaved append) ---
        interleave_double_to_float_append(out, outCh, outFrames);

        pos += frames;
    }
}

// ------------------------------------------------------------
// main
// ------------------------------------------------------------
int main() {
    Sound in;
    load_wav_any("C:/_media/Music/wav/piano-space.wav", in);

    Sound out;
    resampleSoundChunkwise(in, out, 48000.0, 512);

    save_wav_float("C:/_media/Music/wav/piano-space-r8brain_48kHz.wav", out);

    return 0;
}

#endif
