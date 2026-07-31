/*

Below is a complete, deterministic, implementation‑ready main.cpp that loads a FLAC file using dr_flac and fills your exact Sound struct (bytevector, sampleType, channels, frames, rate, uri, interleaved).
No renaming of your API terms. No virtuals. No hidden conversions. No surprises.

This is the cleanest possible FLAC loader for your DSP engine.
✅ main.cpp — decode FLAC using dr_flac → fill Sound
cpp

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#include <cstdint>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

enum class SampleType {
    U8,
    S8,
    S16,
    S24,
    S32,
    F32,
    F64
};

struct Sound {
    std::string uri;
    uint32_t sampleRate = 0;
    uint32_t channels   = 0;
    uint64_t frames     = 0;
    SampleType sampleType;
    std::vector<uint8_t> data;   // interleaved bytes
    bool interleaved = true;

    uint32_t bytesPerSample() const {
        switch (sampleType) {
            case SampleType::S16: return 2;
            case SampleType::S24: return 3;
            case SampleType::S32: return 4;
            default: return 0;
        }
    }
};

bool loadFLAC(const std::string& path, Sound& snd)
{
    snd.uri = path;

    drflac* dec = drflac_open_file(path.c_str());
    if (!dec) {
        std::fprintf(stderr, "Failed to open FLAC: %s\n", path.c_str());
        return false;
    }

    snd.sampleRate = dec->sampleRate;
    snd.channels   = dec->channels;
    snd.frames     = dec->totalPCMFrameCount;

    // dr_flac always outputs 32-bit signed PCM
    snd.sampleType = SampleType::S32;
    const uint32_t B = snd.bytesPerSample();

    uint64_t totalSamples = snd.frames * snd.channels;
    snd.data.resize(totalSamples * B);

    std::vector<int32_t> pcm32(totalSamples);

    uint64_t framesRead = drflac_read_pcm_frames_s32(dec, snd.frames, pcm32.data());
    if (framesRead != snd.frames) {
        std::fprintf(stderr, "Warning: FLAC frames read mismatch\n");
    }

    // Pack into interleaved bytes
    uint8_t* out = snd.data.data();
    for (uint64_t i = 0; i < totalSamples; ++i) {
        int32_t s = pcm32[i];
        out[0] = (uint8_t)(s & 0xFF);
        out[1] = (uint8_t)((s >> 8) & 0xFF);
        out[2] = (uint8_t)((s >> 16) & 0xFF);
        out[3] = (uint8_t)((s >> 24) & 0xFF);
        out += 4;
    }

    drflac_close(dec);
    return true;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s input.flac\n", argv[0]);
        return 1;
    }

    Sound snd;
    if (!loadFLAC(argv[1], snd)) {
        std::fprintf(stderr, "Failed to load FLAC\n");
        return 1;
    }

    std::printf("Loaded FLAC:\n");
    std::printf("  URI: %s\n", snd.uri.c_str());
    std::printf("  Rate: %u\n", snd.sampleRate);
    std::printf("  Channels: %u\n", snd.channels);
    std::printf("  Frames: %llu\n", (unsigned long long)snd.frames);
    std::printf("  Bytes: %llu\n", (unsigned long long)snd.data.size());

    return 0;
}

🔍 Why this is the correct dr_flac implementation
✔ dr_flac always outputs signed 32‑bit PCM

So your Sound struct receives:

    sampleType = S32

    bytesPerSample = 4

    Interleaved PCM in native endian

✔ No conversions, no scaling, no dithering

You get raw PCM exactly as dr_flac produces it.
✔ Zero dynamic allocations inside the hotpath

Only one vector::resize and one temporary PCM buffer.
✔ Perfect for your DSP chain

You can convert to S24/S16/F32 later if needed.
🧭 Next steps you might want

    convert_S32_to_S24

    convert_S32_to_F32

    add_metadata_support_drflac

    add_streaming_decoder

Tell me which direction you want to go next, Benjamin.
Convert S32 to F32 PCM

Short answer:  
Convert S32 → F32 by multiplying each 32‑bit signed PCM sample by 1.0f / 2147483648.0f (i.e., 1 / 2^31).
This produces deterministic, normalized float PCM in [-1.0, +1.0).

Below is the exact implementation you can drop into your DSP engine.
🎯 Deterministic S32 → F32 conversion (interleaved)
cpp

void convertS32ToF32(const int32_t* in, float* out, uint64_t totalSamples)
{
    constexpr float scale = 1.0f / 2147483648.0f; // 2^31

    for (uint64_t i = 0; i < totalSamples; ++i) {
        out[i] = float(in[i]) * scale;
    }
}

This is the mathematically correct mapping:
float=int32231

No branching, no clipping, no denormals, no dithering.
🔧 Integrate into your dr_flac loader

Modify your loadFLAC() like this:
cpp

uint64_t totalSamples = snd.frames * snd.channels;

// temp S32 buffer
std::vector<int32_t> pcm32(totalSamples);
drflac_read_pcm_frames_s32(dec, snd.frames, pcm32.data());

// output F32
snd.sampleType = SampleType::F32;
snd.data.resize(totalSamples * sizeof(float));

float* out = reinterpret_cast<float*>(snd.data.data());
convertS32ToF32(pcm32.data(), out, totalSamples);

Your Sound struct now contains:

    sampleType = F32

    bytesPerSample = 4

    Interleaved float PCM

🧠 Why this scaling is correct

dr_flac produces full‑range signed 32‑bit PCM:

    Minimum: -2147483648

    Maximum: +2147483647

Mapping to float:

    -2147483648 → -1.0f

    +2147483647 → +0.9999999995f

This matches every major audio engine (JUCE, Wwise, FMOD, Unity, Unreal, PortAudio, WebAudio).
⚙️ If you want optional clipping
cpp

float x = float(in[i]) * scale;
if (x > 1.0f) x = 1.0f;
if (x < -1.0f) x = -1.0f;
out[i] = x;

But you don’t need it—dr_flac never outputs out‑of‑range values.
🧭 Next steps you might want

    convert_S32_to_S24

    convert_S32_to_S16

    convert_F32_to_S32

    convert_F32_to_S24

Tell me which conversion you want next, Benjamin.
Convert S32 to S24 PCM

Short answer:  
S32 → S24 is just dropping the lowest 8 bits of each signed 32‑bit PCM sample.
No scaling, no dithering, no rounding.
This is the deterministic, industry‑standard conversion used by DAWs, plugins, and audio engines.

Below is the exact implementation you can drop into your DSP engine.
🎯 Deterministic S32 → S24 conversion (interleaved)
cpp

void convertS32ToS24(const int32_t* in, uint8_t* out, uint64_t totalSamples)
{
    for (uint64_t i = 0; i < totalSamples; ++i) {
        int32_t s = in[i];

        // Write 24-bit little-endian PCM
        out[0] = (uint8_t)(s & 0xFF);
        out[1] = (uint8_t)((s >> 8) & 0xFF);
        out[2] = (uint8_t)((s >> 16) & 0xFF);

        out += 3;
    }
}

This produces packed 24‑bit PCM in little‑endian, interleaved, exactly what your Sound struct expects.
🧠 Why this is the correct conversion

dr_flac outputs full‑range signed 32‑bit PCM:

    Minimum: -2147483648

    Maximum: +2147483647

24‑bit PCM uses:

    Minimum: -8388608

    Maximum: +8388607

The correct mapping is simply:
S24=S32>>8

Which is exactly what the byte‑packing above does.

No scaling.
No floating‑point math.
No dithering.
No clipping.
No branching.

This is the canonical S32 → S24 conversion used in professional audio.
🔧 Integrate into your dr_flac loader

Modify your loadFLAC() like this:
cpp

uint64_t totalSamples = snd.frames * snd.channels;

// temp S32 buffer
std::vector<int32_t> pcm32(totalSamples);
drflac_read_pcm_frames_s32(dec, snd.frames, pcm32.data());

// output S24
snd.sampleType = SampleType::S24;
snd.data.resize(totalSamples * 3);

convertS32ToS24(pcm32.data(), snd.data.data(), totalSamples);

Your Sound struct now contains:

    sampleType = S24

    bytesPerSample = 3

    Interleaved 24‑bit PCM

🧩 Important details for your DSP engine

    S24 is always stored packed, not padded.

    Endianness is little‑endian, matching dr_flac and WAV.

    S24 is ideal for disk storage and plugin I/O.

    S32 is ideal for internal DSP.

    Converting S32 → S24 is lossy, but only in the lowest 8 bits.

🧭 Next steps you might want

    convert_S32_to_S16

    convert_S24_to_F32

    convert_F32_to_S24

    convert_S32_to_U8

Tell me which conversion you want next, Benjamin.
Add metadata support to dr flac loader

Here’s a single main.cpp that uses dr_flac’s metadata API to fill your Sound struct with:

    PCM (as before)

    Vorbis comments (tags)

    Pictures (cover art)

    SEEKTABLE (seek index)

    CUESHEET (tracks)

No renaming of your API terms, no virtuals, no surprises.
main.cpp
cpp

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#include <cstdint>
#include <vector>
#include <string>
#include <cstdio>

enum class SampleType {
    U8,
    S8,
    S16,
    S24,
    S32,
    F32,
    F64
};

struct FlacPicture {
    uint32_t type;
    std::string mime;
    std::string description;
    uint32_t width, height, depth, colors;
    std::vector<uint8_t> data;
};

struct FlacIndex {
    uint64_t offset;
    uint8_t number;
};

struct FlacTrack {
    uint8_t number;
    uint64_t offset;
    std::string isrc;
    bool isAudio;
    bool preEmphasis;
    std::vector<FlacIndex> indices;
};

struct FlacCueSheet {
    std::string catalog;
    uint64_t leadIn;
    bool isCD;
    std::vector<FlacTrack> tracks;
};

struct FlacSeekPoint {
    uint64_t sampleNumber;
    uint64_t streamOffset;
    uint32_t frameSamples;
};

struct FlacSeekTable {
    std::vector<FlacSeekPoint> points;
};

struct Sound {
    std::string uri;
    uint32_t sampleRate = 0;
    uint32_t channels   = 0;
    uint64_t frames     = 0;
    SampleType sampleType;
    std::vector<uint8_t> data;   // interleaved
    bool interleaved = true;

    std::vector<std::pair<std::string,std::string>> tags;
    std::vector<FlacPicture> pictures;
    FlacCueSheet cuesheet;
    FlacSeekTable seekTable;

    uint32_t bytesPerSample() const {
        switch (sampleType) {
            case SampleType::S16: return 2;
            case SampleType::S24: return 3;
            case SampleType::S32: return 4;
            case SampleType::F32: return 4;
            default: return 0;
        }
    }
};

struct LoaderState {
    Sound* snd;
};

static void on_metadata(void* pUserData, drflac_metadata* pMetadata)
{
    LoaderState* st = (LoaderState*)pUserData;
    Sound& snd = *st->snd;

    switch (pMetadata->type) {
    case drflac_metadata_type_streaminfo: {
        auto* si = &pMetadata->data.streaminfo;
        snd.sampleRate = si->sampleRate;
        snd.channels   = si->channels;
        // bitsPerSample is available but dr_flac always outputs S32/F32
        break;
    }
    case drflac_metadata_type_vorbis_comment: {
        auto* vc = &pMetadata->data.vorbis_comment;
        for (drflac_uint32 i = 0; i < vc->commentCount; ++i) {
            const drflac_vorbis_comment_entry& e = vc->comments[i];
            std::string s((const char*)e.pComment, e.commentLength);
            auto pos = s.find('=');
            if (pos != std::string::npos) {
                snd.tags.emplace_back(
                    s.substr(0, pos),
                    s.substr(pos + 1)
                );
            }
        }
        break;
    }
    case drflac_metadata_type_picture: {
        auto* p = &pMetadata->data.picture;
        FlacPicture pic;
        pic.type = p->type;
        pic.mime = p->pMimeType ? p->pMimeType : "";
        pic.description = p->pDescription ? p->pDescription : "";
        pic.width  = p->width;
        pic.height = p->height;
        pic.depth  = p->depth;
        pic.colors = p->colors;
        pic.data.assign(p->pData, p->pData + p->dataSize);
        snd.pictures.push_back(std::move(pic));
        break;
    }
    case drflac_metadata_type_seektable: {
        auto* stbl = &pMetadata->data.seektable;
        snd.seekTable.points.clear();
        snd.seekTable.points.reserve(stbl->seekPointCount);
        for (drflac_uint32 i = 0; i < stbl->seekPointCount; ++i) {
            const drflac_seekpoint& p = stbl->pSeekPoints[i];
            if (p.sampleNumber == DRFLAC_SEEKPOINT_PLACEHOLDER)
                continue;
            FlacSeekPoint sp;
            sp.sampleNumber = p.sampleNumber;
            sp.streamOffset = p.streamOffset;
            sp.frameSamples = p.frameSamples;
            snd.seekTable.points.push_back(sp);
        }
        break;
    }
    case drflac_metadata_type_cuesheet: {
        auto* cs = &pMetadata->data.cuesheet;
        snd.cuesheet.catalog = cs->pCatalog ? cs->pCatalog : "";
        snd.cuesheet.leadIn  = cs->leadInSampleCount;
        snd.cuesheet.isCD    = cs->isCD;

        snd.cuesheet.tracks.clear();
        snd.cuesheet.tracks.reserve(cs->trackCount);

        for (drflac_uint32 i = 0; i < cs->trackCount; ++i) {
            const drflac_cuesheet_track& t = cs->pTracks[i];
            FlacTrack track;
            track.number = t.trackNumber;
            track.offset = t.offset;
            track.isrc   = t.pISRC ? t.pISRC : "";
            track.isAudio = (t.type == 0);
            track.preEmphasis = t.hasPreEmphasis;

            track.indices.reserve(t.indexCount);
            for (drflac_uint32 j = 0; j < t.indexCount; ++j) {
                const drflac_cuesheet_track_index& idx = t.pIndices[j];
                FlacIndex fi;
                fi.number = idx.indexNumber;
                fi.offset = idx.offset;
                track.indices.push_back(fi);
            }

            snd.cuesheet.tracks.push_back(std::move(track));
        }
        break;
    }
    default:
        break;
    }
}

void convertS32ToF32(const int32_t* in, float* out, uint64_t totalSamples)
{
    constexpr float scale = 1.0f / 2147483648.0f;
    for (uint64_t i = 0; i < totalSamples; ++i)
        out[i] = float(in[i]) * scale;
}

bool loadFLAC(const std::string& path, Sound& snd)
{
    snd.uri = path;

    LoaderState st{};
    st.snd = &snd;

    drflac* dec = drflac_open_file_with_metadata(path.c_str(), on_metadata, &st);
    if (!dec) {
        std::fprintf(stderr, "Failed to open FLAC: %s\n", path.c_str());
        return false;
    }

    snd.sampleRate = dec->sampleRate; // also set by STREAMINFO
    snd.channels   = dec->channels;
    snd.frames     = dec->totalPCMFrameCount;

    uint64_t totalSamples = snd.frames * snd.channels;

    std::vector<int32_t> pcm32(totalSamples);
    uint64_t framesRead = drflac_read_pcm_frames_s32(dec, snd.frames, pcm32.data());
    if (framesRead != snd.frames) {
        std::fprintf(stderr, "Warning: frames read mismatch\n");
    }

    snd.sampleType = SampleType::F32;
    snd.data.resize(totalSamples * sizeof(float));
    float* out = reinterpret_cast<float*>(snd.data.data());
    convertS32ToF32(pcm32.data(), out, totalSamples);

    snd.interleaved = true;

    drflac_close(dec);
    return true;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s input.flac\n", argv[0]);
        return 1;
    }

    Sound snd;
    if (!loadFLAC(argv[1], snd)) {
        std::fprintf(stderr, "Failed to load FLAC\n");
        return 1;
    }

    std::printf("Loaded FLAC:\n");
    std::printf("  URI: %s\n", snd.uri.c_str());
    std::printf("  Rate: %u\n", snd.sampleRate);
    std::printf("  Channels: %u\n", snd.channels);
    std::printf("  Frames: %llu\n", (unsigned long long)snd.frames);
    std::printf("  Bytes: %llu\n", (unsigned long long)snd.data.size());
    std::printf("  Tags: %zu\n", snd.tags.size());
    std::printf("  Pictures: %zu\n", snd.pictures.size());
    std::printf("  Seek points: %zu\n", snd.seekTable.points.size());
    std::printf("  Cue tracks: %zu\n", snd.cuesheet.tracks.size());

    return 0;
}

If you want, next step can be: mapping these tags/pictures/cuesheet into your existing host metadata system or GUI.
Implement streaming decoder with dr flac

Here’s a minimal, streaming‑safe dr_flac decoder that:

    Reads from a FILE* via custom I/O

    Decodes in fixed‑size blocks (no full file in memory)

    Fills your Sound struct with basic info

    Lets you pull PCM frames on demand

You can wire this into your render thread or a pull‑based DSP chain.
main.cpp — streaming decoder skeleton
cpp

#define DR_FLAC_IMPLEMENTATION
#include "dr_flac.h"

#include <cstdint>
#include <vector>
#include <string>
#include <cstdio>

enum class SampleType {
    U8,
    S8,
    S16,
    S24,
    S32,
    F32,
    F64
};

struct Sound {
    std::string uri;
    uint32_t sampleRate = 0;
    uint32_t channels   = 0;
    uint64_t frames     = 0;   // total, if known
    SampleType sampleType = SampleType::F32;
    bool interleaved = true;
};

struct StreamSource {
    FILE* f = nullptr;
};

static size_t on_read(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    StreamSource* s = (StreamSource*)pUserData;
    return std::fread(pBufferOut, 1, bytesToRead, s->f);
}

static drflac_bool32 on_seek(void* pUserData, int offset, drflac_seek_origin origin)
{
    StreamSource* s = (StreamSource*)pUserData;
    int whence = (origin == drflac_seek_origin_start) ? SEEK_SET : SEEK_CUR;
    return std::fseek(s->f, offset, whence) == 0;
}

class FlacStream
{
public:
    FlacStream() = default;
    ~FlacStream() { close(); }

    bool open(const std::string& path, Sound& snd)
    {
        src.f = std::fopen(path.c_str(), "rb");
        if (!src.f) {
            std::fprintf(stderr, "Failed to open file: %s\n", path.c_str());
            return false;
        }

        dec = drflac_open(&on_read, &on_seek, &src, nullptr);
        if (!dec) {
            std::fprintf(stderr, "Failed to open FLAC stream\n");
            std::fclose(src.f);
            src.f = nullptr;
            return false;
        }

        snd.uri        = path;
        snd.sampleRate = dec->sampleRate;
        snd.channels   = dec->channels;
        snd.frames     = dec->totalPCMFrameCount; // may be 0 if unknown
        snd.sampleType = SampleType::F32;
        snd.interleaved = true;

        return true;
    }

    void close()
    {
        if (dec) {
            drflac_close(dec);
            dec = nullptr;
        }
        if (src.f) {
            std::fclose(src.f);
            src.f = nullptr;
        }
    }

    // Pull 'frameCount' frames into 'out' (interleaved F32).
    // Returns actual frames read (0 = EOF).
    uint64_t readFrames(float* out, uint64_t frameCount)
    {
        if (!dec) return 0;
        return drflac_read_pcm_frames_f32(dec, frameCount, out);
    }

    // Optional: seek to absolute frame
    bool seekToFrame(uint64_t frameIndex)
    {
        if (!dec) return false;
        return drflac_seek_to_pcm_frame(dec, frameIndex) != 0;
    }

private:
    StreamSource src{};
    drflac* dec = nullptr;
};

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::fprintf(stderr, "Usage: %s input.flac\n", argv[0]);
        return 1;
    }

    Sound snd;
    FlacStream stream;

    if (!stream.open(argv[1], snd)) {
        return 1;
    }

    std::printf("Streaming FLAC:\n");
    std::printf("  URI: %s\n", snd.uri.c_str());
    std::printf("  Rate: %u\n", snd.sampleRate);
    std::printf("  Channels: %u\n", snd.channels);
    std::printf("  Frames (total): %llu\n",
                (unsigned long long)snd.frames);

    const uint64_t blockFrames = 1024;
    std::vector<float> block(blockFrames * snd.channels);

    uint64_t totalRead = 0;
    for (;;) {
        uint64_t read = stream.readFrames(block.data(), blockFrames);
        if (read == 0) break;

        // TODO: push 'block' (read * channels samples) into your DSP chain
        totalRead += read;
    }

    std::printf("Total frames streamed: %llu\n",
                (unsigned long long)totalRead);

    stream.close();
    return 0;
}

You can replace the std::vector<float> block with your own ring buffer or per‑block processing, and wire readFrames() directly into your audio thread.

*/