// UnifiedDecoder.cpp
#include "UnifiedDecoder2.h"

#if 0
#include "Ap4.h"
#include "Ap4File.h"
#include "Ap4Track.h"
#include "Ap4Sample.h"
#include "Ap4SampleDescription.h"
#include "Ap4Mp4AudioInfo.h"
#include "Ap4EsdsAtom.h"
#include "alac/ALACBitUtilities.h"

// #include "Ap4.h"
// #include "Ap4File.h"
// #include "Ap4Track.h"
// #include "Ap4Sample.h"
// #include "Ap4SampleDescription.h"
// #include "Ap4MpegAudioSampleEntry.h"
// #include "Ap4EsdsAtom.h"

namespace de {
namespace audio {

// ---------------- ctor/dtor ----------------

UnifiedDecoder::UnifiedDecoder() = default;

UnifiedDecoder::~UnifiedDecoder()
{
    close();
}

// ---------------- init AAC ----------------

bool UnifiedDecoder::initAac(const AacConfig& cfg, const FileInfo& baseInfo, PacketReader reader)
{
    close();

    if (!cfg.ascData || cfg.ascSize == 0 || !reader)
        return false;

    m_reader = reader;
    m_codec  = CodecType::AAC;

    m_aac = NeAACDecOpen();
    if (!m_aac)
        return false;

    NeAACDecConfigurationPtr c = NeAACDecGetCurrentConfiguration(m_aac);
    c->outputFormat = FAAD_FMT_FLOAT;
    NeAACDecSetConfiguration(m_aac, c);

    long sr = 0;
    unsigned char ch = 0;

    // faad2 signature:
    // long NeAACDecInit2(NeAACDecHandle hDecoder,
    //                    unsigned char *pBuffer,
    //                    unsigned long SizeOfDecoderSpecificInfo,
    //                    unsigned long *samplerate,
    //                    unsigned char *channels);
    long r = NeAACDecInit2(m_aac,
                           const_cast<u8*>(cfg.ascData),
                           (unsigned long)cfg.ascSize,
                           (unsigned long*)&sr,
                           &ch);
    if (r < 0)
        return false;

    info = baseInfo;
    info.sampleRate   = (u32)sr;
    info.channelCount = (u16)ch;
    info.sampleType   = FileInfo::ST_F32;

    return true;
}

// ---------------- init ALAC ----------------

bool UnifiedDecoder::initAlac(const AlacConfig& cfg, const FileInfo& baseInfo, PacketReader reader)
{
    close();

    if (!cfg.cookieData || cfg.cookieSize == 0 || !reader)
        return false;

    m_reader = reader;
    m_codec  = CodecType::ALAC;

    m_alac = new ALACDecoder();

    // ALACDecoder expects a BitBuffer defined in ALACDecoder.h
    BitBuffer bits;
    BitBufferInit(&bits,
                  const_cast<u8*>(cfg.cookieData),
                  (u32)cfg.cookieSize);
    m_alac->Init(&bits);

    info = baseInfo;
    info.sampleRate   = cfg.sampleRate;
    info.channelCount = cfg.channels;
    info.sampleType   = FileInfo::ST_F32;

    m_bitDepth = cfg.bitDepth;
    m_alacSpf  = cfg.samplesPerFrame;

    m_alacIntBuf.resize(m_alacSpf * info.channelCount);

    return true;
}

// ---------------- init Opus ----------------

bool UnifiedDecoder::initOpus(const OpusConfig& cfg, const FileInfo& baseInfo, PacketReader reader)
{
    close();

    if (!reader || cfg.channels == 0)
        return false;

    m_reader = reader;
    m_codec  = CodecType::OPUS;

    int err = 0;
    m_opus = opus_decoder_create((opus_int32)cfg.sampleRate,
                                 (int)cfg.channels,
                                 &err);
    if (err != OPUS_OK)
        return false;

    info = baseInfo;
    info.sampleRate   = cfg.sampleRate;
    info.channelCount = cfg.channels;
    info.sampleType   = FileInfo::ST_F32;

    return true;
}

// ---------------- fastScanFrameCount ----------------

bool UnifiedDecoder::fastScanFrameCount()
{
    if (!m_reader)
        return false;

    if (m_codec != CodecType::OPUS)
        return info.frameCount != 0;

    const u32 maxFrames = 5760;
    std::vector<float> tmp(maxFrames * info.channelCount);

    u64 totalFrames = 0;

    // IMPORTANT: this assumes m_reader starts from the beginning and is used only here.
    for (;;)
    {
        const u8* data = nullptr;
        u32 size = 0;
        if (!m_reader(data, size))
            break;

        int f = opus_decode_float(m_opus,
                                  data,
                                  (opus_int32)size,
                                  tmp.data(),
                                  maxFrames,
                                  0);
        if (f > 0)
            totalFrames += (u64)f;
    }

    info.frameCount = totalFrames;
    return true;
}

// ---------------- decodeNextPacket ----------------

bool UnifiedDecoder::decodeNextPacket(float*& out, u32& frames)
{
    out    = nullptr;
    frames = 0;

    if (!m_reader)
        return false;

    const u8* data = nullptr;
    u32 size = 0;
    if (!m_reader(data, size))
        return false;

    if (m_codec == CodecType::AAC)
    {
        NeAACDecFrameInfo fi;

        // faad2 signature:
        // void* NeAACDecDecode2(NeAACDecHandle hDecoder,
        //                       NeAACDecFrameInfo *hInfo,
        //                       unsigned char *buffer,
        //                       unsigned long buffer_size,
        //                       void **sample_buffer,
        //                       unsigned long sample_buffer_size);
        void* sampleBuf = nullptr;
        NeAACDecDecode2(m_aac,
                        &fi,
                        const_cast<u8*>(data),
                        (unsigned long)size,
                        &sampleBuf,
                        0);

        if (fi.error != 0 || !sampleBuf || fi.channels == 0)
            return false;

        u32 totalSamples = fi.samples;
        frames = totalSamples / fi.channels;

        float* src = static_cast<float*>(sampleBuf);
        float* dst = new float[totalSamples];
        std::memcpy(dst, src, totalSamples * sizeof(float));
        out = dst;
        return true;
    }

    if (m_codec == CodecType::ALAC)
    {
        u32 outFrames = 0;

        // ALACDecoder::Decode signature (Apple ref):
        // void Decode(BitBuffer *bits,
        //             int32_t *outBuffer,
        //             uint32_t numFrames,
        //             uint32_t *outNumFrames,
        //             uint32_t flags);
        BitBuffer bits;
        BitBufferInit(&bits,
                      const_cast<u8*>(data),
                      size);

        m_alac->Decode(&bits,
                       m_alacIntBuf.data(),
                       m_alacSpf,
                       &outFrames,
                       0);

        if (outFrames == 0)
            return false;

        frames = outFrames;
        u32 totalSamples = outFrames * info.channelCount;
        float* dst = new float[totalSamples];

        const float scale = 1.0f / float(1u << (m_bitDepth - 1));
        for (u32 i = 0; i < totalSamples; ++i)
            dst[i] = m_alacIntBuf[i] * scale;

        out = dst;
        return true;
    }

    if (m_codec == CodecType::OPUS)
    {
        const u32 maxFrames = 5760;
        u32 totalSamplesMax = maxFrames * info.channelCount;
        float* dst = new float[totalSamplesMax];

        int f = opus_decode_float(m_opus,
                                  data,
                                  (opus_int32)size,
                                  dst,
                                  maxFrames,
                                  0);
        if (f < 0)
        {
            delete[] dst;
            return false;
        }

        frames = (u32)f;
        out    = dst;
        return true;
    }

    return false;
}

// ---------------- stream ----------------

bool UnifiedDecoder::stream(const StreamCallback& cb, u32 chunkFrames)
{
    if (!cb || !m_reader)
        return false;

    m_chunkBuf.clear();
    m_chunkBuf.reserve(chunkFrames * info.channelCount);

    while (true)
    {
        float* pkt = nullptr;
        u32 frames = 0;
        if (!decodeNextPacket(pkt, frames))
            break;

        u32 totalSamples = frames * info.channelCount;
        u32 offset = (u32)m_chunkBuf.size();
        m_chunkBuf.resize(offset + totalSamples);
        std::memcpy(m_chunkBuf.data() + offset, pkt,
                    totalSamples * sizeof(float));
        delete[] pkt;

        u64 availableFrames = m_chunkBuf.size() / info.channelCount;
        while (availableFrames >= chunkFrames)
        {
            cb(m_chunkBuf.data(), chunkFrames, info.channelCount);

            u32 usedSamples      = chunkFrames * info.channelCount;
            u32 remainingSamples = (u32)m_chunkBuf.size() - usedSamples;
            if (remainingSamples > 0)
                std::memmove(m_chunkBuf.data(),
                             m_chunkBuf.data() + usedSamples,
                             remainingSamples * sizeof(float));
            m_chunkBuf.resize(remainingSamples);
            availableFrames = m_chunkBuf.size() / info.channelCount;
        }
    }

    // flush tail
    u64 remainingFrames = m_chunkBuf.size() / info.channelCount;
    if (remainingFrames > 0)
        cb(m_chunkBuf.data(), remainingFrames, info.channelCount);

    return true;
}

// ---------------- close ----------------

void UnifiedDecoder::close()
{
    if (m_aac)
    {
        NeAACDecClose(m_aac);
        m_aac = nullptr;
    }
    if (m_alac)
    {
        delete m_alac;
        m_alac = nullptr;
    }
    if (m_opus)
    {
        opus_decoder_destroy(m_opus);
        m_opus = nullptr;
    }

    m_codec    = CodecType::UNKNOWN;
    m_reader   = nullptr;
    m_bitDepth = 0;
    m_alacSpf  = 0;
    m_chunkBuf.clear();
    m_alacIntBuf.clear();
}



#if 0
// ----------------- open -----------------

bool UnifiedDecoder::open(const std::string& uri)
{
    close();

    if (AP4_FileByteStream::Create(uri.c_str(),
                                   AP4_FileByteStream::STREAM_MODE_READ,
                                   m_input) != AP4_SUCCESS)
        return false;

    m_file  = new AP4_File(*m_input, true);
    m_movie = m_file->GetMovie();
    if (!m_movie) return false;

    m_track = m_movie->GetTrack(AP4_Track::TYPE_AUDIO);
    if (!m_track) return false;

    m_sampleCount = m_track->GetSampleCount();
    m_sampleIndex = 0;

    AP4_SampleDescription* sd  = m_track->GetSampleDescription(0);
    auto* asd = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);
    if (!asd) return false;

    info.sampleRate   = asd->GetSampleRate();
    info.channelCount = (u16)asd->GetChannelCount();
    info.sampleType   = FileInfo::ST_F32; // we always output float32
    info.duration     = (u64)m_track->GetDuration() * 1000000000ULL /
                        m_track->GetMediaTimeScale();
    info.frameCount   = 0; // may be filled later

    AP4_UI32 fmt = sd->GetFormat();

    if (fmt == AP4_SAMPLE_FORMAT_MP4A) {
        m_codec = AAC;
        return initAac(sd, asd);
    } else if (fmt == AP4_SAMPLE_FORMAT_ALAC) {
        m_codec = ALAC;
        return initAlac(sd, asd);
    } else if (fmt == AP4_SAMPLE_FORMAT_OPUS) {
        m_codec = OPUS;
        return initOpus(sd, asd);
    }

    m_codec = UNKNOWN;
    return false;
}

// ----------------- init AAC -----------------
#if 0
bool UnifiedDecoder::initAac(AP4_SampleDescription* sd, AP4_AudioSampleDescription* /*asd*/)
{
    AP4_EsdsAtom* esds = nullptr;
    sd->GetChild(esds);
    if (!esds) return false;

    const AP4_DataBuffer& asc =
        esds->GetDecoderConfigDescriptor()->GetDecoderSpecificInfo()->GetData();

    m_aac = NeAACDecOpen();
    if (!m_aac) return false;

    NeAACDecConfigurationPtr cfg = NeAACDecGetCurrentConfiguration(m_aac);
    cfg->outputFormat = FAAD_FMT_FLOAT;
    NeAACDecSetConfiguration(m_aac, cfg);

    long sr = 0;
    unsigned char ch = 0;
    long r = NeAACDecInit2(m_aac, asc.GetData(), asc.GetDataSize(), &sr, &ch);
    if (r < 0) return false;

    info.sampleRate   = (u32)sr;
    info.channelCount = (u16)ch;
    return true;
}

bool UnifiedDecoder::initAac(AP4_SampleDescription* sd, AP4_AudioSampleDescription* /*asd*/)
{
    auto* mp4a = AP4_DYNAMIC_CAST(AP4_Mp4AudioSampleEntry, sd);
    if (!mp4a)
        return false;

    AP4_EsdsAtom* esds = mp4a->GetEsdsAtom();
    if (!esds)
        return false;

    const AP4_DataBuffer& asc =
        esds->GetDecoderConfigDescriptor()
            ->GetDecoderSpecificInfo()
            ->GetData();

    m_aac = NeAACDecOpen();
    if (!m_aac)
        return false;

    auto cfg = NeAACDecGetCurrentConfiguration(m_aac);
    cfg->outputFormat = FAAD_FMT_FLOAT;
    NeAACDecSetConfiguration(m_aac, cfg);

    long sr = 0;
    unsigned char ch = 0;

    long r = NeAACDecInit2(m_aac,
                           asc.GetData(),
                           asc.GetDataSize(),
                           &sr,
                           &ch);

    if (r < 0)
        return false;

    info.sampleRate   = (u32)sr;
    info.channelCount = (u16)ch;
    return true;
}
#endif

bool UnifiedDecoder::initAac(AP4_SampleDescription* sd)
{
    // AAC sample entry is AP4_MpegAudioSampleEntry
    AP4_MpegAudioSampleEntry* mp4a =
        AP4_DYNAMIC_CAST(AP4_MpegAudioSampleEntry, sd);
    if (!mp4a)
        return false;

    // The sample entry is a container of atoms
    AP4_AtomParent* parent = mp4a->GetParent();
    if (!parent)
        return false;

    // Find ESDS atom
    AP4_EsdsAtom* esds = nullptr;
    for (AP4_List<AP4_Atom>::Item* item = parent->GetChildren().FirstItem();
         item;
         item = item->GetNext()) {

        AP4_Atom* atom = item->GetData();
        esds = AP4_DYNAMIC_CAST(AP4_EsdsAtom, atom);
        if (esds)
            break;
    }

    if (!esds)
        return false;

    AP4_DecoderConfigDescriptor* dcd = esds->GetDecoderConfigDescriptor();
    if (!dcd)
        return false;

    AP4_DecoderSpecificInfoDescriptor* dsi = dcd->GetDecoderSpecificInfo();
    if (!dsi)
        return false;

    const AP4_DataBuffer& asc = dsi->GetData();

    // --- faad2 init ---
    m_aac = NeAACDecOpen();
    if (!m_aac)
        return false;

    NeAACDecConfigurationPtr cfg = NeAACDecGetCurrentConfiguration(m_aac);
    cfg->outputFormat = FAAD_FMT_FLOAT;
    NeAACDecSetConfiguration(m_aac, cfg);

    long sr = 0;
    unsigned char ch = 0;

    long r = NeAACDecInit2(m_aac,
                           asc.GetData(),
                           asc.GetDataSize(),
                           &sr,
                           &ch);
    if (r < 0)
        return false;

    info.sampleRate   = (u32)sr;
    info.channelCount = (u16)ch;
    info.sampleType   = FileInfo::ST_F32;

    return true;
}


// ----------------- init ALAC -----------------

bool UnifiedDecoder::initAlac(AP4_SampleDescription* sd, AP4_AudioSampleDescription* asd)
{
    auto* alacEntry = AP4_DYNAMIC_CAST(AP4_AlacAudioSampleEntry, asd);
    if (!alacEntry) return false;

    const AP4_DataBuffer& cookie = alacEntry->GetCodecSpecificData();

    m_alac = new ALACDecoder();
    BitBuffer bits;
    BitBufferInit(&bits, (u8*)cookie.GetData(), cookie.GetDataSize());
    m_alac->Init(&bits);

    info.sampleRate   = m_alac->mConfig.sampleRate;
    info.channelCount = (u16)m_alac->mConfig.numChannels;

    // allocate a reasonable max (4096 frames * channels)
    m_alacIntBuf.resize(4096 * info.channelCount);
    return true;
}

// ----------------- init Opus -----------------

bool UnifiedDecoder::initOpus(AP4_SampleDescription* sd, AP4_AudioSampleDescription* asd)
{
    auto* opusEntry = AP4_DYNAMIC_CAST(AP4_OpusAudioSampleEntry, asd);
    if (!opusEntry) return false;

    int err = 0;
    info.sampleRate   = 48000; // Opus decode rate
    info.channelCount = (u16)opusEntry->GetChannelCount();

    m_opus = opus_decoder_create((opus_int32)info.sampleRate,
                                 (int)info.channelCount, &err);
    return (err == OPUS_OK);
}

// ----------------- fastScanFrameCount -----------------

bool UnifiedDecoder::fastScanFrameCount()
{
    if (!m_track) return false;

    // AAC + ALAC can be computed without decoding
    if (m_codec == AAC) {
        // assume AAC-LC 1024 samples/frame; refine if you parse ASC
        const u32 frameLen = 1024;
        info.frameCount = (u64)m_sampleCount * frameLen;
        return true;
    }

    if (m_codec == ALAC) {
        auto* sd  = m_track->GetSampleDescription(0);
        auto* asd = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);
        auto* alacEntry = AP4_DYNAMIC_CAST(AP4_AlacAudioSampleEntry, asd);
        if (!alacEntry) return false;

        u32 spf = alacEntry->GetSamplesPerFrame();
        info.frameCount = (u64)m_sampleCount * spf;
        return true;
    }

    if (m_codec == OPUS) {
        // must decode all packets to know total frames
        AP4_Ordinal savedIndex = m_sampleIndex;
        m_sampleIndex = 0;

        const u32 maxFrames = 5760;
        std::vector<float> tmp(maxFrames * info.channelCount);
        u64 total = 0;

        for (AP4_Ordinal i = 0; i < m_sampleCount; ++i) {
            AP4_Sample sample;
            AP4_DataBuffer buffer;
            if (m_track->ReadSample(i, sample, buffer) != AP4_SUCCESS)
                continue;

            int frames = opus_decode_float(m_opus,
                                           buffer.GetData(),
                                           (opus_int32)buffer.GetDataSize(),
                                           tmp.data(), maxFrames, 0);
            if (frames > 0)
                total += (u64)frames;
        }

        info.frameCount = total;
        m_sampleIndex = savedIndex;
        // reset opus decoder state
        int err = 0;
        opus_decoder_destroy(m_opus);
        m_opus = opus_decoder_create((opus_int32)info.sampleRate,
                                     (int)info.channelCount, &err);
        return (err == OPUS_OK);
    }

    return false;
}

// ----------------- decodeNextPacket -----------------

bool UnifiedDecoder::decodeNextPacket(float*& out, u32& frames)
{
    out    = nullptr;
    frames = 0;

    if (m_sampleIndex >= m_sampleCount)
        return false;

    AP4_Sample sample;
    AP4_DataBuffer buffer;
    if (m_track->ReadSample(m_sampleIndex++, sample, buffer) != AP4_SUCCESS)
        return false;

    const u8* data = buffer.GetData();
    size_t    size = buffer.GetDataSize();

    if (m_codec == AAC) {
        NeAACDecFrameInfo fi;
        float* pcm = (float*)NeAACDecDecode2(m_aac, &fi, data, size);
        if (fi.error != 0 || !pcm || fi.channels == 0)
            return false;

        u32 totalSamples = fi.samples;
        frames = totalSamples / fi.channels;

        float* dst = new float[totalSamples];
        std::memcpy(dst, pcm, totalSamples * sizeof(float));
        out = dst;
        return true;
    }

    if (m_codec == ALAC) {
        u32 outFrames = 0;
        m_alac->Decode(data, (u32)size, m_alacIntBuf.data(), &outFrames);
        if (outFrames == 0)
            return false;

        frames = outFrames;
        u32 totalSamples = outFrames * info.channelCount;
        float* dst = new float[totalSamples];

        const u32 bitDepth = m_alac->mConfig.bitDepth;
        const float scale = 1.0f / float(1u << (bitDepth - 1));

        for (u32 i = 0; i < totalSamples; ++i)
            dst[i] = m_alacIntBuf[i] * scale;

        out = dst;
        return true;
    }

    if (m_codec == OPUS) {
        const u32 maxFrames = 5760;
        u32 totalSamplesMax = maxFrames * info.channelCount;
        float* dst = new float[totalSamplesMax];

        int f = opus_decode_float(m_opus,
                                  data, (opus_int32)size,
                                  dst, maxFrames, 0);
        if (f < 0) {
            delete[] dst;
            return false;
        }

        frames = (u32)f;
        // shrink if you care; caller uses frames * channels
        out = dst;
        return true;
    }

    return false;
}

// ----------------- stream -----------------

bool UnifiedDecoder::stream(const StreamCallback& cb, u32 chunkFrames)
{
    if (!cb || !m_track)
        return false;

    m_chunkBuf.clear();
    m_chunkBuf.reserve(chunkFrames * info.channelCount);

    while (true) {
        float* pkt = nullptr;
        u32 frames = 0;
        if (!decodeNextPacket(pkt, frames))
            break;

        u32 totalSamples = frames * info.channelCount;
        u32 offset = (u32)m_chunkBuf.size();
        m_chunkBuf.resize(offset + totalSamples);
        std::memcpy(m_chunkBuf.data() + offset, pkt,
                    totalSamples * sizeof(float));
        delete[] pkt;

        u64 availableFrames = m_chunkBuf.size() / info.channelCount;
        while (availableFrames >= chunkFrames) {
            cb(m_chunkBuf.data(), chunkFrames, info.channelCount);

            u32 usedSamples = chunkFrames * info.channelCount;
            u32 remainingSamples = (u32)m_chunkBuf.size() - usedSamples;
            if (remainingSamples > 0)
                std::memmove(m_chunkBuf.data(),
                             m_chunkBuf.data() + usedSamples,
                             remainingSamples * sizeof(float));
            m_chunkBuf.resize(remainingSamples);
            availableFrames = m_chunkBuf.size() / info.channelCount;
        }
    }

    // flush tail
    u64 remainingFrames = m_chunkBuf.size() / info.channelCount;
    if (remainingFrames > 0)
        cb(m_chunkBuf.data(), (u64)remainingFrames, info.channelCount);

    return true;
}

// ----------------- close -----------------

void UnifiedDecoder::close()
{
    if (m_aac) {
        NeAACDecClose(m_aac);
        m_aac = nullptr;
    }
    if (m_alac) {
        delete m_alac;
        m_alac = nullptr;
    }
    if (m_opus) {
        opus_decoder_destroy(m_opus);
        m_opus = nullptr;
    }

    delete m_file;  m_file  = nullptr;
    m_movie = nullptr;
    m_track = nullptr;

    if (m_input) {
        m_input->Release();
        m_input = nullptr;
    }

    m_sampleIndex = 0;
    m_sampleCount = 0;
    m_codec       = UNKNOWN;
    m_chunkBuf.clear();
    m_alacIntBuf.clear();
}
#endif

} // end namespace audio.
} // end namespace de.

#endif