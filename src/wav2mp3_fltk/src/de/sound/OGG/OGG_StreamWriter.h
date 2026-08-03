#pragma once
#include <ogg/ogg.h>
#include <de/Core.h>

/*
ogg.beginStream();
ogg.pushPacket(encodedBytes, frameCount);
ogg.pushPacket(encodedBytes, frameCount);
ogg.finish();
*/

class OggWriter
{
public:
    OggWriter(File& file, int serial = 12345)
        : m_file(file)
    {
        ogg_stream_init(&m_os, serial);
    }

    ~OggWriter()
    {
        ogg_stream_clear(&m_os);
    }

    void beginStream()
    {
        m_started    = false;
        m_eos        = false;
        m_packetno   = 0;
        m_granulepos = 0;
    }

    // Caller provides encoded packet + frameCount (decoded PCM samples per channel)
    void pushPacket(const std::vector<uint8_t>& data, int frameCount)
    {
        writePacket(data, frameCount, false);
    }

    void finish()
    {
        // Mark last packet as EOS
        writePacket({}, 0, true);
        flushIncompletePages();
    }

private:
    void writePacket(const std::vector<uint8_t>& data, int frameCount, bool lastPacket)
    {
        // frameCount = decoded PCM samples per channel
        m_granulepos += frameCount;

        ogg_packet pkt{};
        pkt.packet     = const_cast<unsigned char*>(data.data());
        pkt.bytes      = data.size();
        pkt.b_o_s      = m_started ? 0 : 1;
        pkt.e_o_s      = lastPacket ? 1 : 0;
        pkt.granulepos = m_granulepos;
        pkt.packetno   = m_packetno++;

        m_started = true;

        ogg_stream_packetin(&m_os, &pkt);

        writeFullPages();
        flushIncompletePages();
    }

    void writeFullPages()
    {
        ogg_page og;
        while (!m_eos)
        {
            int ok = ogg_stream_pageout(&m_os, &og);
            if (ok == 0)
                break;

            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);

            if (ogg_page_eos(&og))
                m_eos = true;
        }
    }

    void flushIncompletePages()
    {
        ogg_page og;
        while (ogg_stream_flush(&m_os, &og))
        {
            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);
        }
    }

private:
    File& m_file;
    ogg_stream_state m_os{};

    bool        m_started    = false;
    bool        m_eos        = false;
    ogg_int64_t m_packetno   = 0;
    ogg_int64_t m_granulepos = 0;
};

// -----------------------------------------------------------------------------
// Opus encode options
// -----------------------------------------------------------------------------
struct OpusEncodeOptions
{
    int bitrate      = 128000; // bps
    bool vbr         = true;
    int complexity   = 5;      // 0..10
    std::string vendor = "Abenton";

    // generic meta tags: key=value (e.g. "ENCODER","SoundWriter_OGG_Opus")
    std::vector<std::pair<std::string,std::string>> tags;
};

// -----------------------------------------------------------------------------
// Encoded Opus packet + frameCount (decoded samples per channel)
// -----------------------------------------------------------------------------
struct OpusPacket
{
    std::vector<uint8_t> data;
    int frameCount = 0; // decoded PCM samples per channel represented by this packet
};

// -----------------------------------------------------------------------------
// Opus encoder that respects Opus frame requirements and buffers PCM
// -----------------------------------------------------------------------------
class SoundOpusEncoder
{
public:
    SoundOpusEncoder(int sampleRate,
                     int channels,
                     const OpusEncodeOptions& opts)
        : m_sampleRate(sampleRate)
        , m_channels(channels)
        , m_opts(opts)
    {
        int err = 0;
        m_enc = opus_encoder_create(
            m_sampleRate,
            m_channels,
            OPUS_APPLICATION_AUDIO,
            &err);

        // basic controls
        opus_encoder_ctl(m_enc, OPUS_SET_BITRATE(m_opts.bitrate));
        opus_encoder_ctl(m_enc, OPUS_SET_VBR(m_opts.vbr ? 1 : 0));
        opus_encoder_ctl(m_enc, OPUS_SET_COMPLEXITY(m_opts.complexity));

        // get lookahead (preskip)
        opus_encoder_ctl(m_enc, OPUS_GET_LOOKAHEAD(&m_preskip));

        m_frameSize = 960; // 20 ms @ 48 kHz (recommended)
    }

    ~SoundOpusEncoder()
    {
        if (m_enc)
            opus_encoder_destroy(m_enc);
    }

    int frameSize() const { return m_frameSize; }
    int preskip()   const { return m_preskip; }
    int sampleRate() const { return m_sampleRate; }
    int channels()   const { return m_channels; }

    // Push PCM (F32 interleaved) into internal buffer, emit full Opus packets
    void pushPcm(const float* pcm,
                 int frameCount,
                 std::vector<OpusPacket>& outPackets)
    {
        // append to buffer
        size_t oldSize = m_pcmBuffer.size();
        m_pcmBuffer.resize(oldSize + size_t(frameCount) * size_t(m_channels));
        ::memcpy(m_pcmBuffer.data() + oldSize,
                 pcm,
                 size_t(frameCount) * size_t(m_channels) * sizeof(float));

        // while we have at least one full frame, encode it
        while (int(m_pcmBuffer.size() / m_channels) >= m_frameSize)
        {
            encodeOneFrame(outPackets);
        }
    }

    // Finish: encode remaining partial frame (padded), frameCount = actual samples
    void finish(std::vector<OpusPacket>& outPackets)
    {
        int totalFrames = int(m_pcmBuffer.size() / m_channels);
        if (totalFrames <= 0)
            return;

        // actual decoded samples per channel
        int actualFrameCount = totalFrames;

        // pad to full frameSize
        m_pcmBuffer.resize(size_t(m_frameSize) * size_t(m_channels), 0.0f);

        std::vector<uint8_t> out(8192);
        int ret = opus_encode_float(
            m_enc,
            m_pcmBuffer.data(),
            m_frameSize,
            out.data(),
            (opus_int32)out.size());

        if (ret > 0)
        {
            out.resize(ret);
            OpusPacket pkt;
            pkt.data       = std::move(out);
            pkt.frameCount = actualFrameCount; // granulepos uses actual samples
            outPackets.push_back(std::move(pkt));
        }

        m_pcmBuffer.clear();
    }

    // Build OpusHead from encoder state + Sound info
    std::vector<uint8_t> makeOpusHead() const
    {
        std::vector<uint8_t> head(19);
        ::memcpy(head.data(), "OpusHead", 8);
        head[8] = 1; // version
        head[9] = (uint8_t)m_channels;

        uint16_t preskipLE = (uint16_t)m_preskip;
        ::memcpy(head.data() + 10, &preskipLE, 2);

        uint32_t srLE = (uint32_t)m_sampleRate;
        ::memcpy(head.data() + 12, &srLE, 4);

        uint16_t gainLE = 0;
        ::memcpy(head.data() + 16, &gainLE, 2);

        head[18] = 0; // channel mapping family (0 = mono/stereo)

        return head;
    }

    // Build OpusTags from vendor + tags
    std::vector<uint8_t> makeOpusTags() const
    {
        std::string tags;
        tags.append("OpusTags", 8);

        // vendor
        const std::string& vendor = m_opts.vendor;
        uint32_t vendor_len = (uint32_t)vendor.size();
        tags.append(reinterpret_cast<const char*>(&vendor_len), 4);
        tags.append(vendor);

        // user tags
        uint32_t count = (uint32_t)m_opts.tags.size();
        tags.append(reinterpret_cast<const char*>(&count), 4);

        for (auto& kv : m_opts.tags)
        {
            std::string entry = kv.first + "=" + kv.second;
            uint32_t len = (uint32_t)entry.size();
            tags.append(reinterpret_cast<const char*>(&len), 4);
            tags.append(entry);
        }

        return std::vector<uint8_t>(tags.begin(), tags.end());
    }

private:
    void encodeOneFrame(std::vector<OpusPacket>& outPackets)
    {
        // encode first frameSize frames from buffer
        std::vector<uint8_t> out(8192);

        int ret = opus_encode_float(
            m_enc,
            m_pcmBuffer.data(),
            m_frameSize,
            out.data(),
            (opus_int32)out.size());

        if (ret > 0)
        {
            out.resize(ret);

            OpusPacket pkt;
            pkt.data       = std::move(out);
            pkt.frameCount = m_frameSize; // decoded samples per channel

            outPackets.push_back(std::move(pkt));
        }

        // remove used samples from buffer
        size_t usedSamples = size_t(m_frameSize) * size_t(m_channels);
        size_t remaining   = m_pcmBuffer.size() - usedSamples;
        if (remaining > 0)
        {
            ::memmove(m_pcmBuffer.data(),
                      m_pcmBuffer.data() + usedSamples,
                      remaining * sizeof(float));
        }
        m_pcmBuffer.resize(remaining);
    }

private:
    OpusEncoder* m_enc       = nullptr;
    int          m_sampleRate = 48000;
    int          m_channels   = 2;
    int          m_frameSize  = 960;
    int          m_preskip    = 0;

    OpusEncodeOptions m_opts;
    std::vector<float> m_pcmBuffer;
};

// -----------------------------------------------------------------------------
// OggWriter from previous step (frameCount-based granulepos)
// -----------------------------------------------------------------------------
class OggWriter
{
public:
    OggWriter(File& file, int serial = 12345)
        : m_file(file)
    {
        ogg_stream_init(&m_os, serial);
    }

    ~OggWriter()
    {
        ogg_stream_clear(&m_os);
    }

    void beginStream()
    {
        m_started    = false;
        m_eos        = false;
        m_packetno   = 0;
        m_granulepos = 0;
    }

    void pushPacket(const std::vector<uint8_t>& data, int frameCount)
    {
        writePacket(data, frameCount, false);
    }

    void finish()
    {
        // EOS packet (no additional samples)
        writePacket(std::vector<uint8_t>{}, 0, true);
        flushIncompletePages();
    }

private:
    void writePacket(const std::vector<uint8_t>& data,
                     int frameCount,
                     bool lastPacket)
    {
        m_granulepos += frameCount; // frameCount = decoded samples per channel

        ogg_packet pkt{};
        pkt.packet     = const_cast<unsigned char*>(data.data());
        pkt.bytes      = (long)data.size();
        pkt.b_o_s      = m_started ? 0 : 1;
        pkt.e_o_s      = lastPacket ? 1 : 0;
        pkt.granulepos = m_granulepos;
        pkt.packetno   = m_packetno++;

        m_started = true;

        ogg_stream_packetin(&m_os, &pkt);

        writeFullPages();
        flushIncompletePages();
    }

    void writeFullPages()
    {
        ogg_page og;
        while (!m_eos)
        {
            int ok = ogg_stream_pageout(&m_os, &og);
            if (ok == 0)
                break;

            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);

            if (ogg_page_eos(&og))
                m_eos = true;
        }
    }

    void flushIncompletePages()
    {
        ogg_page og;
        while (ogg_stream_flush(&m_os, &og))
        {
            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);
        }
    }

private:
    File& m_file;
    ogg_stream_state m_os{};

    bool        m_started    = false;
    bool        m_eos        = false;
    ogg_int64_t m_packetno   = 0;
    ogg_int64_t m_granulepos = 0;
};

// -----------------------------------------------------------------------------
// save_sound_ogg_opus using Sound, OggWriter, SoundOpusEncoder
// -----------------------------------------------------------------------------
bool
save_sound_ogg_opus(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    if (sound.empty())
    {
        DE_ERROR("Got empty sound: ", uri)
        return false;
    }

    if (sound.m_sampleRate != 48000)
    {
        DE_WARN("Opus wants 48000 Hz, resample first. ", uri)
    }

    // converter to F32
    SampleType srcType = sound.m_sampleType;
    SampleType dstType = SampleType::F32;
    auto converter = SampleTypeConverter::getConverter(srcType, dstType);
    if (!converter)
    {
        DE_ERROR("No converter ", srcType.str(), " -> ", dstType.str(), ". ", uri)
        return false;
    }

    File file(uri, eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot open output file, ", uri)
        return false;
    }

    options.onProgress(1);

    // build Opus options from SoundSaveOptions
    OpusEncodeOptions opusOpts;
    opusOpts.bitrate    = options.bitrate;
    opusOpts.vbr        = true; // or options.vbr if you have it
    opusOpts.complexity = 5;

    opusOpts.vendor     = "Abenton";

    // basic tags
    opusOpts.tags.emplace_back("ENCODER", "SoundWriter_OGG_Opus");
    // user extras: bpm, etc. Example:
    // opusOpts.tags.emplace_back("BPM", std::to_string(options.bpm));
    // opusOpts.tags.emplace_back("TITLE", options.title);
    // opusOpts.tags.emplace_back("ARTIST", options.artist);
    // cover art would require METADATA_BLOCK_PICTURE in OpusTags (more involved).

    SoundOpusEncoder enc(48000, sound.m_channels, opusOpts);
    OggWriter        ogg(file);

    ogg.beginStream();

    // --- OpusHead ---
    {
        auto head = enc.makeOpusHead();
        ogg.pushPacket(head, 0); // header: no decoded samples
    }

    // --- OpusTags ---
    {
        auto tags = enc.makeOpusTags();
        ogg.pushPacket(tags, 0); // tags: no decoded samples
    }

    options.onProgress(5);

    // --- Audio data ---
    int64_t totalFrames = sound.m_frames;
    int64_t pos         = 0;

    int64_t chunkFrames = 4096;
    int64_t chunkSamples = chunkFrames * sound.m_channels;
    TAlignedVector<float> chunkBuf(chunkSamples);

    while (pos < totalFrames)
    {
        int64_t desired = std::min<int64_t>(chunkFrames, totalFrames - pos);

        int64_t converted = sound.read_frames(
            converter,
            chunkBuf.data(),
            desired,
            pos);

        if (converted < 1)
            break;

        std::vector<OpusPacket> packets;
        enc.pushPcm(chunkBuf.data(), (int)converted, packets);

        for (auto& pkt : packets)
        {
            ogg.pushPacket(pkt.data, pkt.frameCount);
        }

        pos += converted;

        options.onProgress(5 + 85.0 * double(pos) / double(totalFrames));
    }

    // finish encoder: flush remaining partial frame
    {
        std::vector<OpusPacket> packets;
        enc.finish(packets);
        for (auto& pkt : packets)
        {
            ogg.pushPacket(pkt.data, pkt.frameCount);
        }
    }

    ogg.finish();

    options.onProgress(100);

    return true;
}

/*

#pragma once
#include <ogg/ogg.h>
#include "File.hpp"

class OggContainerWriter
{
public:
    OggContainerWriter(File& file, int serial = 12345)
        : m_file(file)
    {
        ogg_stream_init(&m_os, serial);
    }

    ~OggContainerWriter()
    {
        ogg_stream_clear(&m_os);
    }

    // Start a new logical stream
    void begin(const std::string& codec, int sampleRate, int channels)
    {
        m_codec      = codec;
        m_sampleRate = sampleRate;
        m_channels   = channels;

        m_packetno   = 0;
        m_granulepos = 0;
        m_eos        = false;
    }

    // Add metadata (for OpusTags or Vorbis comments)
    void addHeaderField(const std::string& key, const std::string& value)
    {
        m_metadata.emplace_back(key, value);
    }

    // Build and write codec-specific headers
    void writeHeaders()
    {
        if (m_codec == "opus")
            writeOpusHeaders();
        else if (m_codec == "vorbis")
            writeVorbisHeaders();
    }

    // Write encoded audio packet
    void writeAudioPacket(const unsigned char* data, long size, long samplesPerChannel, bool lastPacket)
    {
        m_granulepos += samplesPerChannel;
        writePacketInternal(data, size, false, lastPacket, m_granulepos);
    }

    // Final flush
    void end()
    {
        flushIncompletePages();
    }

private:
    // --- Codec-specific header builders -------------------------------------

    void writeOpusHeaders()
    {
        writeOpusHead();
        writeOpusTags();
    }

    void writeOpusHead()
    {
        unsigned char head[19];
        memcpy(head, "OpusHead", 8);
        head[8]  = 1;
        head[9]  = m_channels;

        uint16_t preskip = 312; // or dynamic if needed
        memcpy(head + 10, &preskip, 2);

        memcpy(head + 12, &m_sampleRate, 4);

        uint16_t gain = 0;
        memcpy(head + 16, &gain, 2);

        head[18] = 0; // channel mapping family

        writePacketInternal(head, sizeof(head), true, false, 0);
    }

    void writeOpusTags()
    {
        std::string tags;
        tags.append("OpusTags", 8);

        std::string vendor = "SoundWriter";
        uint32_t vendor_len = vendor.size();
        tags.append(reinterpret_cast<char*>(&vendor_len), 4);
        tags.append(vendor);

        uint32_t count = m_metadata.size();
        tags.append(reinterpret_cast<char*>(&count), 4);

        for (auto& kv : m_metadata)
        {
            std::string entry = kv.first + "=" + kv.second;
            uint32_t len = entry.size();
            tags.append(reinterpret_cast<char*>(&len), 4);
            tags.append(entry);
        }

        writePacketInternal(
            reinterpret_cast<unsigned char*>(tags.data()),
            tags.size(),
            false,
            false,
            0);
    }

    void writeVorbisHeaders()
    {
        // You already have this logic in your Vorbis encoder.
        // You can move it here exactly the same way.
    }

    // --- Ogg packet writing --------------------------------------------------

    void writePacketInternal(
        const unsigned char* data,
        long size,
        bool isBOS,
        bool isEOS,
        ogg_int64_t granulepos)
    {
        ogg_packet pkt{};
        pkt.packet     = const_cast<unsigned char*>(data);
        pkt.bytes      = size;
        pkt.b_o_s      = isBOS ? 1 : 0;
        pkt.e_o_s      = isEOS ? 1 : 0;
        pkt.granulepos = granulepos;
        pkt.packetno   = m_packetno++;

        ogg_stream_packetin(&m_os, &pkt);

        writeFullPages();
        flushIncompletePages();
    }

    void writeFullPages()
    {
        ogg_page og;
        while (!m_eos)
        {
            int ok = ogg_stream_pageout(&m_os, &og);
            if (ok == 0)
                break;

            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);

            if (ogg_page_eos(&og))
                m_eos = true;
        }
    }

    void flushIncompletePages()
    {
        ogg_page og;
        while (ogg_stream_flush(&m_os, &og))
        {
            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);
        }
    }

private:
    File& m_file;
    ogg_stream_state m_os{};

    std::string m_codec;
    int m_sampleRate = 0;
    int m_channels   = 0;

    std::vector<std::pair<std::string,std::string>> m_metadata;

    ogg_int64_t m_packetno   = 0;
    ogg_int64_t m_granulepos = 0;
    bool        m_eos        = false;
};


// =====================================================================
class OpusCodec
{
public:
    OpusCodec(int sampleRate, int channels)
    {
        int err = 0;
        m_enc = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_AUDIO, &err);
    }

    ~OpusCodec()
    {
        opus_encoder_destroy(m_enc);
    }

    void setBitrate(int bps)
    {
        opus_encoder_ctl(m_enc, OPUS_SET_BITRATE(bps));
    }

    void setVBR(bool enabled)
    {
        opus_encoder_ctl(m_enc, OPUS_SET_VBR(enabled ? 1 : 0));
    }

    void setCVBR(bool enabled)
    {
        opus_encoder_ctl(m_enc, OPUS_SET_VBR_CONSTRAINT(enabled ? 1 : 0));
    }

    void setComplexity(int c)
    {
        opus_encoder_ctl(m_enc, OPUS_SET_COMPLEXITY(c));
    }

    std::vector<uint8_t> encode(const float* pcm, int frameSize)
    {
        std::vector<uint8_t> out(8192);
        int ret = opus_encode_float(m_enc, pcm, frameSize, out.data(), out.size());
        out.resize(ret);
        return out;
    }

    std::vector<uint8_t> makeOpusHead(int channels, int sampleRate)
    {
        std::vector<uint8_t> head(19);
        memcpy(head.data(), "OpusHead", 8);
        head[8] = 1;
        head[9] = channels;

        uint16_t preskip = 312;
        memcpy(head.data() + 10, &preskip, 2);

        memcpy(head.data() + 12, &sampleRate, 4);

        uint16_t gain = 0;
        memcpy(head.data() + 16, &gain, 2);

        head[18] = 0;
        return head;
    }

    std::vector<uint8_t> makeOpusTags(const std::vector<std::pair<std::string,std::string>>& tags)
    {
        std::string out = "OpusTags";
        uint32_t vendor_len = 8;
        out.append(reinterpret_cast<char*>(&vendor_len), 4);
        out.append("SoundWr");

        uint32_t count = tags.size();
        out.append(reinterpret_cast<char*>(&count), 4);

        for (auto& kv : tags)
        {
            std::string entry = kv.first + "=" + kv.second;
            uint32_t len = entry.size();
            out.append(reinterpret_cast<char*>(&len), 4);
            out.append(entry);
        }

        return std::vector<uint8_t>(out.begin(), out.end());
    }

private:
    OpusEncoder* m_enc = nullptr;
};


class OggContainerWriter
{
public:
    OggContainerWriter(File& file, int serial = 12345)
        : m_file(file)
    {
        ogg_stream_init(&m_os, serial);
    }

    ~OggContainerWriter()
    {
        ogg_stream_clear(&m_os);
    }

    void begin()
    {
        m_packetno   = 0;
        m_granulepos = 0;
        m_eos        = false;
    }

    void writeHeaderPacket(const std::vector<uint8_t>& data)
    {
        writePacket(data, true, false, 0);
    }

    void writeAudioPacket(const std::vector<uint8_t>& data, int samplesPerChannel, bool last)
    {
        m_granulepos += samplesPerChannel;
        writePacket(data, false, last, m_granulepos);
    }

    void end()
    {
        flushIncompletePages();
    }

private:
    void writePacket(const std::vector<uint8_t>& data, bool bos, bool eos, ogg_int64_t granulepos)
    {
        ogg_packet pkt{};
        pkt.packet     = const_cast<unsigned char*>(data.data());
        pkt.bytes      = data.size();
        pkt.b_o_s      = bos ? 1 : 0;
        pkt.e_o_s      = eos ? 1 : 0;
        pkt.granulepos = granulepos;
        pkt.packetno   = m_packetno++;

        ogg_stream_packetin(&m_os, &pkt);

        writeFullPages();
        flushIncompletePages();
    }

    void writeFullPages()
    {
        ogg_page og;
        while (!m_eos)
        {
            int ok = ogg_stream_pageout(&m_os, &og);
            if (ok == 0)
                break;

            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);

            if (ogg_page_eos(&og))
                m_eos = true;
        }
    }

    void flushIncompletePages()
    {
        ogg_page og;
        while (ogg_stream_flush(&m_os, &og))
        {
            m_file.write(og.header, og.header_len);
            m_file.write(og.body, og.body_len);
        }
    }

private:
    File& m_file;
    ogg_stream_state m_os{};
    ogg_int64_t m_packetno   = 0;
    ogg_int64_t m_granulepos = 0;
    bool        m_eos        = false;
};

class OggAudioEncoder
{
public:
    OggAudioEncoder(File& file)
        : m_ogg(file)
    {}

    void beginOpus(int sampleRate, int channels)
    {
        m_codec = std::make_unique<OpusCodec>(sampleRate, channels);
        m_ogg.begin();
    }

    void setBitrate(int bps)
    {
        m_codec->setBitrate(bps);
    }

    void setVBR(bool enabled)
    {
        m_codec->setVBR(enabled);
    }

    void addTag(const std::string& key, const std::string& value)
    {
        m_tags.emplace_back(key, value);
    }

    void writeHeaders(int sampleRate, int channels)
    {
        auto head = m_codec->makeOpusHead(channels, sampleRate);
        auto tags = m_codec->makeOpusTags(m_tags);

        m_ogg.writeHeaderPacket(head);
        m_ogg.writeHeaderPacket(tags);
    }

    void writePCM(const float* pcm, int frameSize, bool last)
    {
        auto pkt = m_codec->encode(pcm, frameSize);
        m_ogg.writeAudioPacket(pkt, frameSize, last);
    }

    void end()
    {
        m_ogg.end();
    }

private:
    OggContainerWriter m_ogg;
    std::unique_ptr<OpusCodec> m_codec;
    std::vector<std::pair<std::string,std::string>> m_tags;
};

⭐ Now your Opus save function becomes tiny

OggAudioEncoder enc(file);

enc.beginOpus(48000, sound.m_channels);
enc.setBitrate(options.bitrate);
enc.setVBR(options.vbr);
enc.addTag("ENCODER", "SoundWriter_OGG_Opus");

enc.writeHeaders(48000, sound.m_channels);

while (pos < totalFrames)
{
    enc.writePCM(chunkBuf.data(), FRAME, pos + FRAME >= totalFrames);
    pos += FRAME;
}

enc.end();


*/