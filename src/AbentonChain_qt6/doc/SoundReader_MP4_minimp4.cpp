#include <de/audio/file/MP4/SoundReader_MP4.h>
// #include <vector>
// #include <cmath>

// ---------- FAAD2 ----------
extern "C" {
#include <neaacdec.h>
#include <faad.h>
}

// ---------- minimp4 ----------
extern "C" {
#include <minimp4.h>   // from https://github.com/lieff/minimp4
#include <opus/opus.h>
#include <opusfile.h>
}

#include <alac/ALACDecoder.h>
#include <alac/ALACBitUtilities.h>

/*
Sound::loadFromFile()
    ↓
    DecoderFactory::create(path)
        → returns IAudioDecoder*
            (AAC or ALAC)
    ↓
    decoder->readFileInfo()   // sampleRate, channels, frameCount
    ↓
    preallocate m_samples
    ↓
    decoder->decodeInto(m_samples)
*/

namespace de {
namespace audio {
namespace {

FILE*
fopen_utf8(const std::string& utf8_path, const char* utf8_mode)
{
    std::wstring utf16_path = de_wstr(utf8_path);
    std::wstring utf16_mode = de_wstr(utf8_mode);
    return _wfopen(utf16_path.c_str(), utf16_mode.c_str());
}

class IAudioDecoder
{
public:
    virtual ~IAudioDecoder() = default;

    virtual void readFileInfo(FileInfo& info) = 0;

    // decode into preallocated float buffer
    virtual void decodeInto(float* dst) = 0;
};

class AacDecoder : public IAudioDecoder
{
public:
    std::string m_uri;
    MP4D_demux_t mp4{};
    MP4D_track_t* tr = nullptr;
    unsigned long sampleRate = 0;
    unsigned char channels = 0;
    uint64_t totalFrames = 0;

    AacDecoder(const std::string& uri) : m_uri(uri) {}

    void readFileInfo(FileInfo& info) override
    {
        FILE* f = fopen_utf8(m_uri, "rb");
        if (!f)
        {
            DE_ERROR("Cannot open ", m_uri)
            return;
        }

        fseek(f, 0, SEEK_END);
        int64_t fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (!MP4D_open(&mp4, read_cb, f, fileSize))
            throw std::runtime_error("MP4D_open failed");

        for (unsigned i = 0; i < mp4.track_count; ++i)
            if (isAac(mp4.track[i].object_type_indication))
                tr = &mp4.track[i];

        // init FAAD2 to get sampleRate + channels
        NeAACDecHandle h = NeAACDecOpen();
        NeAACDecInit2(h, tr->dsi, tr->dsi_bytes,
                      &sampleRate, &channels);
        NeAACDecClose(h);

        // AAC frame count = sample_count * 1024 (usually)
        totalFrames = uint64_t(tr->sample_count) * 1024;

        info.sampleRate   = sampleRate;
        info.channelCount = channels;
        info.sampleType   = FileInfo::ST_F32;
        info.frameCount   = totalFrames;
        info.duration     = totalFrames * 1'000'000'000ull / sampleRate;

        fclose(f);
    }

    void decodeInto(float* dst) override
    {
        FILE* f = fopen_utf8(m_uri, "rb");
        if (!f)
        {
            DE_ERROR("Cannot open ", m_uri)
            return;
        }
        NeAACDecHandle h = NeAACDecOpen();
        NeAACDecConfigurationPtr cfg = NeAACDecGetCurrentConfiguration(h);
        cfg->outputFormat = FAAD_FMT_FLOAT;
        NeAACDecSetConfiguration(h, cfg);
        NeAACDecInit2(h, tr->dsi, tr->dsi_bytes,
                      &sampleRate, &channels);

        std::vector<uint8_t> frameBuf;
        uint64_t pos = 0;

        for (unsigned i = 0; i < tr->sample_count; ++i) {
            unsigned fb = 0, ts = 0, dur = 0;
            auto off = MP4D_frame_offset(&mp4, tr - mp4.track,
                                         i, &fb, &ts, &dur);

            frameBuf.resize(fb);
            fseek(f, (long)off, SEEK_SET);
            fread(frameBuf.data(), 1, fb, f);

            NeAACDecFrameInfo fi{};
            float* out = (float*)NeAACDecDecode(h, &fi,
                                                  frameBuf.data(), fb);

            if (fi.error || fi.samples == 0)
                continue;

            memcpy(dst + pos, out,
                   fi.samples * sizeof(float));
            pos += fi.samples;
        }

        NeAACDecClose(h);
        fclose(f);
        MP4D_close(&mp4);
    }

    static bool isAac(unsigned oti) {
        return oti == 0x40 || oti == 0x66 || oti == 0x67 || oti == 0x68;
    }

    static int read_cb(int64_t o, void* b, size_t s, void* t) {
        FILE* f = (FILE*)t;
        fseek(f, (long)o, SEEK_SET);
        return (int)fread(b, 1, s, f);
    }
};


class AlacDecoder : public IAudioDecoder {
public:
    std::string m_uri;
    MP4D_demux_t mp4{};
    MP4D_track_t* tr = nullptr;
    ALACSpecificConfig cfg{};
    uint64_t totalFrames = 0;

    AlacDecoder(const std::string& uri) : m_uri(uri) {}

    void readFileInfo(FileInfo& info) override
    {
        FILE* f = fopen_utf8(m_uri, "rb");
        if (!f)
        {
            DE_ERROR("Cannot open ", m_uri)
            return;
        }

        fseek(f, 0, SEEK_END);
        int64_t fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (!MP4D_open(&mp4, read_cb, f, fileSize))
            throw std::runtime_error("MP4D_open failed");

        // find ALAC track
        for (unsigned i = 0; i < mp4.track_count; ++i)
            if (mp4.track[i].object_type_indication == 0xA9)
                tr = &mp4.track[i];

        memcpy(&cfg, tr->dsi, sizeof(cfg));

        totalFrames = uint64_t(cfg.frameLength) * tr->sample_count;

        info.sampleRate   = cfg.sampleRate;
        info.channelCount = cfg.numChannels;
        info.sampleType   = FileInfo::ST_F32;
        info.frameCount   = totalFrames;
        info.duration     = totalFrames * 1'000'000'000ull / cfg.sampleRate;

        fclose(f);
    }

    void decodeInto(float* dst) override
    {
        FILE* f = fopen_utf8(m_uri, "rb");
        if (!f)
        {
            DE_ERROR("Cannot open ", m_uri)
            return;
        }

        ALACDecoder dec;
        dec.Init(tr->dsi, tr->dsi_bytes);

        std::vector<uint8_t> frameBuf;
        std::vector<int32_t> pcm(cfg.frameLength * cfg.numChannels);
        BitBuffer bits;

        uint64_t pos = 0;

        for (unsigned i = 0; i < tr->sample_count; ++i) {
            unsigned fb = 0, ts = 0, dur = 0;
            auto off = MP4D_frame_offset(&mp4, tr - mp4.track,
                                         i, &fb, &ts, &dur);

            frameBuf.resize(fb);
            fseek(f, (long)off, SEEK_SET);
            fread(frameBuf.data(), 1, fb, f);

            BitBufferInit(&bits, frameBuf.data(), fb);

            uint32_t out = 0;
            dec.Decode(&bits, (uint8_t*)pcm.data(),
                       cfg.frameLength, cfg.numChannels, &out);

            const float scale = 1.0f / 2147483648.0f;
            for (uint32_t s = 0; s < out * cfg.numChannels; ++s)
                dst[pos++] = pcm[s] * scale;
        }

        fclose(f);
        MP4D_close(&mp4);
    }

    static int read_cb(int64_t o, void* b, size_t s, void* t) {
        FILE* f = (FILE*)t;
        fseek(f, (long)o, SEEK_SET);
        return (int)fread(b, 1, s, f);
    }
};


class OpusDecoder : public IAudioDecoder {
public:
    std::string m_uri;
    OggOpusFile* of = nullptr;
    int channels = 0;
    opus_int64 pcm_total = 0;
    opus_int32 sampleRate = 48000; // Opus is always 48k internally

    OpusDecoder(const std::string& p) : m_uri(p) {}

    void readFileInfo(FileInfo& info) override
    {
        int err = 0;
        of = op_open_file(m_uri.c_str(), &err);
        if (!of)
            throw std::runtime_error("Opus open failed");

        channels = op_channel_count(of, -1);
        pcm_total = op_pcm_total(of, -1);

        info.sampleRate   = sampleRate;
        info.channelCount = channels;
        info.sampleType   = FileInfo::ST_F32;
        info.frameCount   = pcm_total;
        info.duration     = pcm_total * 1'000'000'000ull / sampleRate;
    }

    void decodeInto(float* dst) override
    {
        int framesRead = 0;
        int pos = 0;

        while (true)
        {
            framesRead = op_read_float(of, dst + pos, 4096 * channels, nullptr);
            if (framesRead <= 0)
                break;

            pos += framesRead * channels;
        }

        op_free(of);
    }
};

/*
void dump_mp4_track(const MP4D_track_t& t)
{
    printf("Track:\n");
    printf("  object_type_indication: 0x%02X\n", t.object_type_indication);

    for (unsigned i = 0; i < t.sample_description_count; ++i)
    {
        const MP4D_sample_description_t& sd = t.sample_description[i];

        char tag[5] = {};
        memcpy(tag, &sd.data_format, 4);

        printf("  sample_description[%u].data_format = '%s'\n", i, tag);
    }
}

bool isOpusSampleEntry(const MP4D_track_t& t)
{
    // minimp4 stores sample entry as 4-byte tag
    uint32_t tag = t.sample_entry; // e.g. 'Opus', 'opus', 'dOps'
    return tag == MP4D_FOURCC('O','p','u','s') ||
           tag == MP4D_FOURCC('o','p','u','s') ||
           tag == MP4D_FOURCC('d','O','p','s');
}

#define MP4D_FOURCC(a,b,c,d) \
((uint32_t)(a)<<24 | (uint32_t)(b)<<16 | (uint32_t)(c)<<8 | (uint32_t)(d))

bool isOpusTrack(const MP4D_track_t& t)
{
    for (unsigned i = 0; i < t.sample_description_count; ++i)
    {
        const MP4D_sample_description_t& sd = t.sample_description[i];

        uint32_t fmt = sd.data_format;

        if (fmt == MP4D_FOURCC('O','p','u','s') ||
            fmt == MP4D_FOURCC('o','p','u','s') ||
            fmt == MP4D_FOURCC('d','O','p','s'))
        {
            return true;
        }
    }
    return false;
}

*/

bool isOpusTrack(const MP4D_track_t& t)
{
    if (t.object_type_indication != 0x00)
        return false;

    if (t.dsi_bytes < 8)
        return false;

    // Check for "OpusHead"
    return memcmp(t.dsi, "OpusHead", 8) == 0;
}


// Audio:
//     Codec        object_type_indication	Bemerkung
//     AAC LC       0x40                    funktioniert
//     AAC SBR/HE	0x40                    funktioniert
//     Opus         0xA0                    YouTube Standard
//     Vorbis       0xDD                    selten
//     FLAC         0xC0                    selten

// Video
//     Codec        OTI     Bemerkung
//     AVC/H.264	0x21	irrelevant für Audio
//     VP9          0x00	kein Audio
//     AV1          0x00	kein Audio
//     MPEG‑4 Video	0x20
//     H.264/AVC	0x21
//     HEVC/H.265	0x23
void dump_mp4_info(const MP4D_demux_t& mp4, const std::string& uri)
{
    DE_TRACE("MP4 File = ", uri)
    DE_TRACE("MP4 Tracks = ", mp4.track_count)

    for (unsigned i = 0; i < mp4.track_count; ++i)
    {
        const MP4D_track_t& t = mp4.track[i];

        DE_TRACE("Track[",i,"] OTI = ", dbHex(t.object_type_indication))
        DE_TRACE("Track[",i,"] SampleCount = ", t.sample_count)
        DE_TRACE("Track[",i,"] Timescale = ", t.timescale)
        DE_TRACE("Track[",i,"] Duration = ", t.duration)
        DE_TRACE("Track[",i,"] dsi_bytes = ", t.dsi_bytes)

        if (t.dsi_bytes > 0)
        {
            std::ostringstream o;

            for (unsigned j = 0; j < t.dsi_bytes; ++j)
                o << dbHex(t.dsi[j]) << " ";

            DE_TRACE("Track[",i,"] DecoderSpecificInfo = ",o.str())
        }
    }
}

class DecoderFactory
{
public: 
    static int file_read_cb(int64_t o, void* buffer, size_t size, void* user)
    {
        FILE* f = (FILE*)user;
        fseek(f, (long)o, SEEK_SET);
        return (int)fread(buffer, 1, size, f);
    }

    static std::unique_ptr<IAudioDecoder>
    create(const std::string& uri)
    {
        DE_WARN("=======================================")
        DE_WARN("Try open ", dbFileName(uri))
        DE_WARN("=======================================")

        FILE* f = fopen_utf8(uri, "rb");
        if (!f)
        {
            DE_ERROR("Cannot open ", uri)
            return nullptr;
        }

        fseek(f, 0, SEEK_END);
        int64_t size = ftell(f);
        fseek(f, 0, SEEK_SET);

        MP4D_demux_t mp4{};
        //MP4D_open(&mp4, file_read_cb, f, size);
        if (MP4D_open(&mp4, file_read_cb, f, size) != 0)
        {
            DE_ERROR("MP4D_open failed ", uri)
            return nullptr;
        }

        fseek(f, 0, SEEK_SET);
        dump_mp4_info(mp4, uri);

        for (unsigned i = 0; i < mp4.track_count; ++i) {
            auto& t = mp4.track[i];

            if (t.object_type_indication == 0xA9)
            {
                DE_OK("Found MP4 codec ALAC ",uri)
                MP4D_close(&mp4);
                fclose(f);
                return std::make_unique<AlacDecoder>(uri);
            }

            if (AacDecoder::isAac(t.object_type_indication))
            {
                DE_OK("Found MP4 codec AAC ",uri)
                MP4D_close(&mp4);
                fclose(f);
                return std::make_unique<AacDecoder>(uri);
            }

            if (isOpusTrack(t))
            {
                DE_OK("Found MP4 codec OPUS ",uri)
                MP4D_close(&mp4);
                fclose(f);
                return std::make_unique<OpusDecoder>(uri);
            }
        }

        DE_ERROR("Unsupported MP4 codec ", uri)
        MP4D_close(&mp4);
        fclose(f);
        return nullptr;
    }
};


} // end namespace.

bool load_sound_mp4_f32(Sound & sound, const std::string & uri )
{
    sound.m_uri = uri;

    auto decoder = DecoderFactory::create(uri);
    if (!decoder)
    {
        DE_ERROR("No decoder ",uri)
        return false;
    }

    // 1) Read metadata
    decoder->readFileInfo(sound.m_fileInfo);

    // 2) Preallocate final buffer
    sound.m_samples.resize(sound.m_fileInfo.frameCount *
                     sound.m_fileInfo.channelCount);

    // 3) Decode directly into final buffer
    decoder->decodeInto(sound.m_samples.data());

    DE_OK(uri)
    return true;
}

} // end namespace audio.
} // end namespace de.

/*
int main() {
    AlacDecoderMinimp4 decoder;

    std::vector<float> allSamples;   // finaler PCM-Buffer
    int finalSampleRate = 0;
    int finalChannels   = 0;

    decoder.decodeFile("test.m4a",
                       [&](const int32_t* samples,
                           int numFrames,
                           int numChannels,
                           int sampleRate)
                       {
                           // Speichere SampleRate & Channels einmalig
                           if (finalSampleRate == 0) {
                               finalSampleRate = sampleRate;
                               finalChannels   = numChannels;
                           }

                           // PCM int32 → float konvertieren
                           // ALAC liefert 24–32 Bit PCM in int32
                           const float scale = 1.0f / 2147483648.0f; // 2^31

                           for (int i = 0; i < numFrames * numChannels; ++i) {
                               float f = samples[i] * scale;
                               allSamples.push_back(f);
                           }
                       }
                       );

    std::cout << "Decoded samples: " << allSamples.size() << "\n";
    std::cout << "Channels: " << finalChannels << "\n";
    std::cout << "SampleRate: " << finalSampleRate << "\n";

    // Jetzt hast du:
    // allSamples = Interleaved float PCM
    // finalChannels = 1/2
    // finalSampleRate = z.B. 44100

    return 0;
}


bool load_sound_mp4_f32(Sound & sound, const std::string & uri )
{
  
    drmp3 mp3;
    if (!drmp3_init_file(&mp3, uri.c_str(), nullptr))
    {
        DE_ERROR("No MP3 file opened. ",uri)
        return false;
    }

    const uint64_t nFrames = drmp3_get_pcm_frame_count(&mp3);
    const uint32_t nChannels = mp3.channels;
    const uint64_t nSamples = nFrames * nChannels;
    sound.m_uri = de::FileSystem::makeAbsolute(uri);
    sound.m_fileInfo.frameCount = nFrames;
    sound.m_fileInfo.channelCount = nChannels;
    sound.m_fileInfo.sampleRate = mp3.sampleRate;
    sound.m_fileInfo.sampleType = FileInfo::ST_F32; // | FileInfo::ST_Interleaved;
    sound.m_samples.resize( nSamples );

    float* __restrict__ dst = reinterpret_cast<float*>(sound.m_samples.data());
    drmp3_read_pcm_frames_f32(&mp3, nFrames, dst);
    drmp3_uninit(&mp3);

    if (nFrames == 0)
    {
        DE_ERROR("No MP3 data")
        return false;
    }

    return true;
}
*/
