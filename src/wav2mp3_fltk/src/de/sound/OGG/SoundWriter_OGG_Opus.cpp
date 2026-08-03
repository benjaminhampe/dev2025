#include <de/sound/OGG/SoundWriter_OGG_Opus.h>

#include <opus/opus.h>
#include <ogg/ogg.h>

namespace de {
namespace sound {

namespace {
} // end namespace.

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

    int err = 0;
    OpusEncoder* enc = opus_encoder_create(
        48000,
        sound.m_channels,
        OPUS_APPLICATION_AUDIO,
        &err);
    if (!enc || err != OPUS_OK)
    {
        DE_ERROR("opus_encoder_create failed, err = ", err)
        return false;
    }

    opus_encoder_ctl(enc, OPUS_SET_BITRATE(options.bitrate * 1000));
    opus_encoder_ctl(enc, OPUS_SET_VBR(1));

    int preskip = 0;
    opus_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&preskip));

    ogg_stream_state os;
    ogg_stream_init(&os, 12345);

    // --- OpusHead ---
    {
        unsigned char head[19];
        ::memcpy(head, "OpusHead", 8);
        head[8]  = 1;
        head[9]  = static_cast<unsigned char>(sound.m_channels);
        *reinterpret_cast<uint16_t*>(head + 10) = static_cast<uint16_t>(preskip);
        *reinterpret_cast<uint32_t*>(head + 12) = 48000;
        *reinterpret_cast<uint16_t*>(head + 16) = 0;
        head[18] = 0;

        ogg_packet header{};
        header.packet     = head;
        header.bytes      = sizeof(head);
        header.b_o_s      = 1;
        header.e_o_s      = 0;
        header.granulepos = 0;
        header.packetno   = 0;

        ogg_stream_packetin(&os, &header);

        ogg_page og;
        while (ogg_stream_flush(&os, &og))
        {
            file.write(og.header, og.header_len);
            file.write(og.body, og.body_len);
        }
    }

    // --- OpusTags ---
    {
        const char* vendor  = "Abenton";
        const char* comment = "ENCODER=SoundWriter_OGG_Opus";

        std::string tags;
        tags.append("OpusTags", 8);

        uint32_t vendor_len = static_cast<uint32_t>(::strlen(vendor));
        tags.append(reinterpret_cast<const char*>(&vendor_len), 4);
        tags.append(vendor, vendor_len);

        uint32_t comment_count = 1;
        tags.append(reinterpret_cast<const char*>(&comment_count), 4);

        uint32_t comment_len = static_cast<uint32_t>(::strlen(comment));
        tags.append(reinterpret_cast<const char*>(&comment_len), 4);
        tags.append(comment, comment_len);

        ogg_packet tagpkt{};
        tagpkt.packet     = reinterpret_cast<unsigned char*>(tags.data());
        tagpkt.bytes      = static_cast<long>(tags.size());
        tagpkt.b_o_s      = 0;
        tagpkt.e_o_s      = 0;
        tagpkt.granulepos = 0;
        tagpkt.packetno   = 1;

        ogg_stream_packetin(&os, &tagpkt);

        ogg_page og;
        while (ogg_stream_flush(&os, &og))
        {
            file.write(og.header, og.header_len);
            file.write(og.body, og.body_len);
        }
    }

    options.onProgress(5);

    // --- Audio data (Vorbis-style Ogg handling) ---
    constexpr int FRAME = 960; // 20 ms @ 48 kHz
    int64_t cFrames  = FRAME;
    int64_t cSamples = cFrames * sound.m_channels;
    TAlignedVector<float>   chunkBuf(cSamples);
    TAlignedVector<uint8_t> out(8192);

    bool    eos          = false;
    int64_t pos          = 0;                 // samples per channel
    int64_t totalFrames  = sound.m_frames;    // samples per channel
    int64_t granulepos   = 0;                 // samples per channel
    int     packetno     = 2;
    ogg_page og;

    while (pos < totalFrames)
    {
        int64_t desired = std::min<int64_t>(cFrames, totalFrames - pos);

        int64_t converted = sound.read_frames(
            converter,
            chunkBuf.data(),
            desired,
            pos);

        if (converted < 1)
        {
            break;
        }

        // pad last frame to FRAME for Opus
        int opusFrames = static_cast<int>(converted);
        if (opusFrames < FRAME)
        {
            ::memset(chunkBuf.data() + opusFrames * sound.m_channels,
                     0,
                     (FRAME - opusFrames) * sound.m_channels * sizeof(float));
            opusFrames = FRAME;
        }

        int ret = opus_encode_float(
            enc,
            chunkBuf.data(),
            opusFrames,
            out.data(),
            static_cast<opus_int32>(out.size()));

        if (ret < 0)
        {
            DE_ERROR("opus_encode_float failed, ret = ", ret)
            break;
        }

        pos        += converted;
        granulepos += converted;

        ogg_packet op{};
        op.packet     = out.data();
        op.bytes      = ret;
        op.b_o_s      = 0;
        op.e_o_s      = (pos >= totalFrames);
        op.granulepos = granulepos;
        op.packetno   = packetno++;

        ogg_stream_packetin(&os, &op);

        while (!eos)
        {
            int ok = ogg_stream_pageout(&os, &og);
            if (ok == 0)
            {
                break;
            }
            file.write(og.header, og.header_len);
            file.write(og.body, og.body_len);

            if (ogg_page_eos(&og))
            {
                eos = true;
            }
        }

        // flush incomplete page like Vorbis
        while (ogg_stream_flush(&os, &og))
        {
            file.write(og.header, og.header_len);
            file.write(og.body, og.body_len);
        }

        options.onProgress(85.0 * double(pos) / double(totalFrames));
    }

    options.onProgress(95);

    // final flush
    while (ogg_stream_flush(&os, &og))
    {
        file.write(og.header, og.header_len);
        file.write(og.body, og.body_len);
    }

    ogg_stream_clear(&os);
    opus_encoder_destroy(enc);

    options.onProgress(100);

    return true;
}


bool
save_sound_ogg_opus_v2(
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

    int err = 0;
    OpusEncoder* enc = opus_encoder_create(
        48000,
        sound.m_channels,
        OPUS_APPLICATION_AUDIO,
        &err);
    if (!enc || err != OPUS_OK)
    {
        DE_ERROR("opus_encoder_create failed, err = ", err)
        return false;
    }

    opus_encoder_ctl(enc, OPUS_SET_BITRATE(options.bitrate));
    opus_encoder_ctl(enc, OPUS_SET_VBR(1));

    int preskip = 0;
    opus_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&preskip));

    ogg_stream_state os;
    ogg_stream_init(&os, 12345);

    // --- OpusHead ---
    {
        unsigned char head[19];
        ::memcpy(head, "OpusHead", 8);
        head[8]  = 1;
        head[9]  = static_cast<unsigned char>(sound.m_channels);
        *reinterpret_cast<uint16_t*>(head + 10) = static_cast<uint16_t>(preskip);
        *reinterpret_cast<uint32_t*>(head + 12) = 48000;
        *reinterpret_cast<uint16_t*>(head + 16) = 0;
        head[18] = 0;

        ogg_packet pkt{};
        pkt.packet     = head;
        pkt.bytes      = sizeof(head);
        pkt.b_o_s      = 1;
        pkt.e_o_s      = 0;
        pkt.granulepos = 0;
        pkt.packetno   = 0;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }
    }

    // --- OpusTags ---
    {
        const char* vendor  = "Abenton";
        const char* comment = "ENCODER=SoundWriter_OGG_Opus";

        std::string tags;
        tags.append("OpusTags", 8);

        uint32_t vendor_len = static_cast<uint32_t>(::strlen(vendor));
        tags.append(reinterpret_cast<const char*>(&vendor_len), 4);
        tags.append(vendor, vendor_len);

        uint32_t comment_count = 1;
        tags.append(reinterpret_cast<const char*>(&comment_count), 4);

        uint32_t comment_len = static_cast<uint32_t>(::strlen(comment));
        tags.append(reinterpret_cast<const char*>(&comment_len), 4);
        tags.append(comment, comment_len);

        ogg_packet pkt{};
        pkt.packet     = reinterpret_cast<unsigned char*>(tags.data());
        pkt.bytes      = static_cast<long>(tags.size());
        pkt.b_o_s      = 0;
        pkt.e_o_s      = 0;
        pkt.granulepos = 0;
        pkt.packetno   = 1;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }
    }

    // --- Audio data ---
    constexpr int FRAME = 960; // 20 ms @ 48 kHz
    TAlignedVector<float>   chunkBuf(FRAME * sound.m_channels, 0.0f);
    TAlignedVector<uint8_t> out(8192, 0x00);

    int      packetno    = 2;
    ogg_page page{};
    int64_t  pos         = 0;              // samples per channel
    int64_t  totalFrames = sound.m_frames; // samples per channel

    while (pos + FRAME <= totalFrames)
    {
        int64_t todo = FRAME;

        int64_t got = sound.read_frames(
            converter,
            chunkBuf.data(),
            todo,
            pos);

        if (got != todo)
            break;

        int ret = opus_encode_float(
            enc,
            chunkBuf.data(),
            FRAME,
            out.data(),
            static_cast<opus_int32>(out.size()));

        if (ret < 0)
        {
            DE_ERROR("opus_encode_float failed, ret = ", ret)
            break;
        }

        pos += FRAME;

        ogg_packet pkt{};
        pkt.packet     = out.data();
        pkt.bytes      = ret;
        pkt.b_o_s      = 0;
        pkt.e_o_s      = 0;
        pkt.granulepos = pos;
        pkt.packetno   = packetno++;

        ogg_stream_packetin(&os, &pkt);

        while (ogg_stream_pageout(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }

        options.onProgress(1 + (98.0 * double(pos) / double(totalFrames)));
    }

    // last partial frame (pad to FRAME, mark e_o_s)
    if (pos < totalFrames)
    {
        int64_t remaining = totalFrames - pos;
        int64_t todo      = std::min<int64_t>(remaining, FRAME);

        int64_t got = sound.read_frames(
            converter,
            chunkBuf.data(),
            todo,
            pos);

        if (got > 0)
        {
            if (got < FRAME)
            {
                ::memset(chunkBuf.data() + got * sound.m_channels,
                         0,
                         (FRAME - got) * sound.m_channels * sizeof(float));
            }

            int ret = opus_encode_float(
                enc,
                chunkBuf.data(),
                FRAME,
                out.data(),
                static_cast<opus_int32>(out.size()));

            if (ret >= 0)
            {
                pos += got;

                ogg_packet pkt{};
                pkt.packet     = out.data();
                pkt.bytes      = ret;
                pkt.b_o_s      = 0;
                pkt.e_o_s      = 1;
                pkt.granulepos = pos;
                pkt.packetno   = packetno++;

                ogg_stream_packetin(&os, &pkt);

                while (ogg_stream_pageout(&os, &page))
                {
                    file.write(page.header, page.header_len);
                    file.write(page.body, page.body_len);
                }
            }
        }
    }

    // final flush
    while (ogg_stream_flush(&os, &page))
    {
        file.write(page.header, page.header_len);
        file.write(page.body, page.body_len);
    }

    ogg_stream_clear(&os);
    opus_encoder_destroy(enc);

    options.onProgress(100);

    return true;
}


bool
save_sound_ogg_opus_v1(
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

    SampleType srcType = sound.m_sampleType;
    SampleType dstType = SampleType::F32;
    auto converter = SampleTypeConverter::getConverter(srcType, dstType);
    if (!converter)
    {
        DE_ERROR("No converter ", srcType.str(), " -> ", dstType.str(), ". ", uri)
        return false;
    }

    DE_BENNI("Converter ", srcType.str(), " -> ", dstType.str(), ". ", uri)

    File file(uri, eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot open output file, ", uri)
        return false;
    }

    options.onProgress(1);

    int err = 0;
    OpusEncoder* enc = opus_encoder_create(
        48000,
        sound.m_channels,
        OPUS_APPLICATION_AUDIO,
        &err);
    if (!enc || err != OPUS_OK)
    {
        DE_ERROR("opus_encoder_create failed, err = ", err)
        return false;
    }

    opus_encoder_ctl(enc, OPUS_SET_BITRATE(options.bitrate));
    opus_encoder_ctl(enc, OPUS_SET_VBR(1)); // or options.vbr

    int preskip = 0;
    opus_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&preskip));

    ogg_stream_state os;
    ogg_stream_init(&os, 12345); // deterministic stream id

    // --- OpusHead ---
    {
        unsigned char head[19];
        ::memcpy(head, "OpusHead", 8);
        head[8]  = 1; // version
        head[9]  = static_cast<unsigned char>(sound.m_channels);
        *reinterpret_cast<uint16_t*>(head + 10) = static_cast<uint16_t>(preskip);
        *reinterpret_cast<uint32_t*>(head + 12) = 48000;
        *reinterpret_cast<uint16_t*>(head + 16) = 0; // gain
        head[18] = 0; // channel mapping family (0 = mono/stereo)

        ogg_packet pkt{};
        pkt.packet     = head;
        pkt.bytes      = sizeof(head);
        pkt.b_o_s      = 1;
        pkt.e_o_s      = 0;
        pkt.granulepos = 0;
        pkt.packetno   = 0;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }
    }

    // --- OpusTags ---
    {
        const char* vendor  = "Abenton";
        const char* comment = "ENCODER=SoundWriter_OGG_Opus";

        std::string tags;
        tags.append("OpusTags", 8);

        uint32_t vendor_len = static_cast<uint32_t>(::strlen(vendor));
        tags.append(reinterpret_cast<const char*>(&vendor_len), 4);
        tags.append(vendor, vendor_len);

        uint32_t comment_count = 1;
        tags.append(reinterpret_cast<const char*>(&comment_count), 4);

        uint32_t comment_len = static_cast<uint32_t>(::strlen(comment));
        tags.append(reinterpret_cast<const char*>(&comment_len), 4);
        tags.append(comment, comment_len);

        ogg_packet pkt{};
        pkt.packet     = reinterpret_cast<unsigned char*>(tags.data());
        pkt.bytes      = static_cast<long>(tags.size());
        pkt.b_o_s      = 0;
        pkt.e_o_s      = 0;
        pkt.granulepos = 0;
        pkt.packetno   = 1;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }
    }

    // --- Audio data ---
    constexpr int FRAME = 960; // 20 ms @ 48 kHz
    TAlignedVector<float>   chunkBuf(FRAME * sound.m_channels, 0.0f);
    TAlignedVector<uint8_t> out(8192, 0x00);

    int      packetno    = 2;
    ogg_page page{};
    bool     bCancelFlag = false;
    int64_t  availFrames = sound.m_frames;
    int64_t  pos         = 0; // samples per channel

    while (!bCancelFlag && availFrames > 0)
    {
        int64_t queryFrames = std::min<int64_t>(FRAME, availFrames);

        int64_t todo = sound.read_frames(
            converter,
            chunkBuf.data(),
            queryFrames,
            pos);

        if (todo <= 0)
            break;

        availFrames -= todo;

        // Opus wants frame sizes in {120,240,480,960,1920,2880}
        int opusFrames = static_cast<int>(todo);
        if (opusFrames != 120 && opusFrames != 240 &&
            opusFrames != 480 && opusFrames != 960 &&
            opusFrames != 1920 && opusFrames != 2880)
        {
            // pad last frame to 960 if shorter
            if (opusFrames < FRAME)
            {
                ::memset(chunkBuf.data() + opusFrames * sound.m_channels,
                         0,
                         (FRAME - opusFrames) * sound.m_channels * sizeof(float));
                opusFrames = FRAME;
            }
        }

        int ret = opus_encode_float(
            enc,
            chunkBuf.data(),
            opusFrames,
            out.data(),
            static_cast<opus_int32>(out.size()));

        if (ret < 0)
        {
            DE_ERROR("opus_encode_float failed, ret = ", ret)
            break;
        }

        int64_t frameEnd = pos + todo;

        ogg_packet pkt{};
        pkt.packet     = out.data();
        pkt.bytes      = ret;
        pkt.b_o_s      = 0;
        pkt.e_o_s      = (frameEnd >= sound.m_frames);
        pkt.granulepos = frameEnd; // samples per channel at 48 kHz
        pkt.packetno   = packetno++;

        ogg_stream_packetin(&os, &pkt);

        while (ogg_stream_pageout(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }

        pos = frameEnd;

        options.onProgress(1 + (98.0 * double(pos) / double(sound.m_frames)));
    }

    // final flush (including e_o_s page)
    while (ogg_stream_flush(&os, &page))
    {
        file.write(page.header, page.header_len);
        file.write(page.body, page.body_len);
    }

    ogg_stream_clear(&os);
    opus_encoder_destroy(enc);

    options.onProgress(100);

    return true;
}

#if 0
bool
save_sound_ogg_opus_v0(
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
        DE_WARN("Opus wants 48000 sound data, resample first. ", uri)
    }

    SampleType srcType = sound.m_sampleType;
    SampleType dstType = SampleType::F32;
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter ",srcType.str()," -> ",dstType.str(),". ", uri)
        return false;
    }

    DE_BENNI("Converter ",srcType.str()," -> ",dstType.str(),". ", uri)


    File file(uri, eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot open output file, ",uri)
        return false;
    }

    options.onProgress(1);

    int err = 0;
    OpusEncoder* enc = opus_encoder_create(48000, sound.m_channels, OPUS_APPLICATION_AUDIO, &err);
    if (!enc)
    {
        DE_ERROR("opus_encoder_create failed")
        return false;
    }

    opus_encoder_ctl(enc, OPUS_SET_BITRATE(options.bitrate)); // Average Bitrate, not a max.
    opus_encoder_ctl(enc, OPUS_SET_VBR(1)); // options.vbr

    ogg_stream_state os;
    ogg_stream_init(&os, 12345); // deterministic stream id

    // --- Write OpusHead ---
    {
        unsigned char head[19];
        memcpy(head, "OpusHead", 8);
        head[8] = 1;               // version
        head[9] = sound.m_channels;
        *reinterpret_cast<uint16_t*>(head + 10) = 312; // pre-skip
        *reinterpret_cast<uint32_t*>(head + 12) = 48000;
        *reinterpret_cast<uint16_t*>(head + 16) = 0;   // gain
        head[18] = 0;              // channel mapping

        ogg_packet pkt{};
        pkt.packet = head;
        pkt.bytes = sizeof(head);
        pkt.b_o_s = 1;
        pkt.e_o_s = 0;
        pkt.granulepos = 0;
        pkt.packetno = 0;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }
    }

    // --- Write OpusTags ---
    {
        // "OpusTags"
        // vendor_length (LE uint32)
        // vendor_string (vendor_length bytes)
        // user_comment_list_length (LE uint32)
        // comments...

        const char* vendor = "Abenton";
#if 0
        std::string tags = "OpusTags";
        tags.push_back(0);
        tags.push_back(0);
        tags.push_back(0);
        tags.push_back(0); // vendor length
        tags.append(vendor);
        tags.append("\x01\x00\x00\x00"); // 1 tag
        tags.append("ENCODER=SoundWriter_OGG_Opus");
#else
        std::string tags;
        tags.append("OpusTags", 8);

        uint32_t vendor_len = strlen(vendor);
        tags.append(reinterpret_cast<char*>(&vendor_len), 4);
        tags.append(vendor, vendor_len);

        uint32_t comment_count = 1;
        tags.append(reinterpret_cast<char*>(&comment_count), 4);

        const char* comment = "ENCODER=SoundWriter_OGG_Opus";
        uint32_t comment_len = strlen(comment);
        tags.append(reinterpret_cast<char*>(&comment_len), 4);
        tags.append(comment, comment_len);
#endif
        ogg_packet pkt{};
        pkt.packet = (unsigned char*)tags.data();
        pkt.bytes = tags.size();
        pkt.b_o_s = 0;
        pkt.e_o_s = 0;
        pkt.granulepos = 0;
        pkt.packetno = 1;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }
    }

    constexpr int FRAME = 960; // 20ms @ 48k
    //const int chunkFrames = 960; // 20ms @ 48k
    //const int chunkSamples = FRAME * sound.m_channels;
    TAlignedVector<float> chunkBuf(FRAME * sound.m_channels, 0.0f);

    // --- Encode PCM frames ---
    TAlignedVector<uint8_t> out(4096 * sound.m_channels, 0x00);

    int packetno = 2;
    ogg_page page;

    bool bCancelFlag = false;
    int64_t availFrames = sound.m_frames;
    int64_t pos = 0;

    while (!bCancelFlag && availFrames > 0)
    {
        int64_t queryFrames = std::min<int64_t>(FRAME, availFrames);

        int64_t todo = sound.read_frames(
                                converter,
                                chunkBuf.data(),
                                queryFrames,
                                pos);

        availFrames -= todo;

        if (todo < 1)
            break;

        int ret = opus_encode_float(enc,
                                    chunkBuf.data(),
                                    todo,
                                    out.data(),
                                    out.size());
        if (ret < 0)
        {
            DE_ERROR("opus_encode_float failed, ret = ", ret)
            break;
        }

        ogg_packet pkt{};
        pkt.packet = out.data();
        pkt.bytes = ret;
        pkt.b_o_s = 0;
        pkt.e_o_s = (pos + todo >= sound.m_frames);
        //Opus granulepos = total PCM samples per channel at 48 kHz
#if 0
        pkt.granulepos = (pos + todo) * sound.m_channels;
#else
        pkt.granulepos = pos + todo;   // NOT multiplied by channels
#endif
        pkt.packetno = packetno++;

        ogg_stream_packetin(&os, &pkt);

        while (ogg_stream_pageout(&os, &page))
        {
            file.write(page.header, page.header_len);
            file.write(page.body, page.body_len);
        }

        pos += todo;

        options.onProgress(1 + (98.0 * double(pos) / double(sound.m_frames)));
    }

    ogg_stream_clear(&os);
    opus_encoder_destroy(enc);
    options.onProgress(100);

    return true;
}
#endif

} // end namespace sound.
} // end namespace de.

/*


// opus_ogg_encoder.cpp


#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>


// -------------------------------------------------------------
// Hilfsfunktionen: Ogg-Seiten schreiben
// -------------------------------------------------------------
static bool write_ogg_page(FILE* f, ogg_page* page)
{
    if (fwrite(page->header, 1, page->header_len, f) != (size_t)page->header_len)
        return false;
    if (fwrite(page->body, 1, page->body_len, f) != (size_t)page->body_len)
        return false;
    return true;
}

// -------------------------------------------------------------
// OpusHead erzeugen (minimal)
// -------------------------------------------------------------
static void build_opus_head(std::string& out, int channels, int preskip, int input_rate)
{
    out.clear();
    out.reserve(19);

    // Magic
    out.append("OpusHead", 8);

    // Version
    out.push_back(1); // version = 1

    // Channel count
    out.push_back((char)channels);

    // Pre-skip (little endian 16-bit)
    uint16_t ps = (uint16_t)preskip;
    out.push_back((char)(ps & 0xFF));
    out.push_back((char)((ps >> 8) & 0xFF));

    // Input sample rate (little endian 32-bit)
    uint32_t ir = (uint32_t)input_rate;
    out.push_back((char)(ir & 0xFF));
    out.push_back((char)((ir >> 8) & 0xFF));
    out.push_back((char)((ir >> 16) & 0xFF));
    out.push_back((char)((ir >> 24) & 0xFF));

    // Output gain (0)
    out.push_back(0);
    out.push_back(0);

    // Channel mapping (0 = single stream, no mapping)
    out.push_back(0);
}

// -------------------------------------------------------------
// OpusTags erzeugen (minimal, ohne User-Comments)
// -------------------------------------------------------------
static void build_opus_tags(std::string& out, const char* vendor)
{
    out.clear();
    out.append("OpusTags", 8);

    // Vendor string length (LE32)
    uint32_t vlen = (uint32_t)std::strlen(vendor);
    out.push_back((char)(vlen & 0xFF));
    out.push_back((char)((vlen >> 8) & 0xFF));
    out.push_back((char)((vlen >> 16) & 0xFF));
    out.push_back((char)((vlen >> 24) & 0xFF));

    // Vendor string
    out.append(vendor, vlen);

    // User comment list length = 0 (LE32)
    out.push_back(0);
    out.push_back(0);
    out.push_back(0);
    out.push_back(0);
}

// -------------------------------------------------------------
// Hauptfunktion: float-PCM → Ogg Opus
// pcm: interleaved float samples, 48kHz
// num_samples: pro Kanal * channels (also total samples)
// -------------------------------------------------------------
bool encode_opus_ogg(const float* pcm,
                     int num_samples,   // total samples (frames * channels)
                     int sample_rate,   // sollte 48000 sein
                     int channels,
                     const char* path)
{
    if (!pcm || num_samples <= 0 || channels <= 0)
        return false;

    // libopus: Encoder erzeugen
    int err = 0;
    OpusEncoder* enc = opus_encoder_create(sample_rate,
                                           channels,
                                           OPUS_APPLICATION_AUDIO,
                                           &err);
    if (!enc || err != OPUS_OK)
        return false;

    // Optional: Bitrate setzen
    opus_encoder_ctl(enc, OPUS_SET_BITRATE(128000));

    // libogg: Stream initialisieren
    ogg_stream_state os;
    int serialno = 1; // für echte Anwendung: random oder unique
    if (ogg_stream_init(&os, serialno) != 0) {
        opus_encoder_destroy(enc);
        return false;
    }

    FILE* f = std::fopen(path, "wb");
    if (!f) {
        ogg_stream_clear(&os);
        opus_encoder_destroy(enc);
        return false;
    }

    // ---------------------------------------------------------
    // 1) OpusHead als ersten Ogg-Packet (BOS)
    // ---------------------------------------------------------
    std::string head;
    int preskip = 0; // minimal, kein Lookahead
    build_opus_head(head, channels, preskip, sample_rate);

    ogg_packet pkt_head{};
    pkt_head.packet = (unsigned char*)head.data();
    pkt_head.bytes  = (long)head.size();
    pkt_head.b_o_s  = 1;
    pkt_head.e_o_s  = 0;
    pkt_head.granulepos = 0;
    pkt_head.packetno   = 0;

    ogg_stream_packetin(&os, &pkt_head);

    ogg_page page;
    while (ogg_stream_flush(&os, &page)) {
        if (!write_ogg_page(f, &page)) {
            std::fclose(f);
            ogg_stream_clear(&os);
            opus_encoder_destroy(enc);
            return false;
        }
    }

    // ---------------------------------------------------------
    // 2) OpusTags als zweiter Packet
    // ---------------------------------------------------------
    std::string tags;
    build_opus_tags(tags, "libopus");

    ogg_packet pkt_tags{};
    pkt_tags.packet = (unsigned char*)tags.data();
    pkt_tags.bytes  = (long)tags.size();
    pkt_tags.b_o_s  = 0;
    pkt_tags.e_o_s  = 0;
    pkt_tags.granulepos = 0;
    pkt_tags.packetno   = 1;

    ogg_stream_packetin(&os, &pkt_tags);

    while (ogg_stream_flush(&os, &page)) {
        if (!write_ogg_page(f, &page)) {
            std::fclose(f);
            ogg_stream_clear(&os);
            opus_encoder_destroy(enc);
            return false;
        }
    }

    // ---------------------------------------------------------
    // 3) Audio-Pakete encodieren
    // Framegröße: 960 Samples (20 ms @ 48k)
    // ---------------------------------------------------------
    const int frame_size = 960;
    const int samples_per_frame = frame_size * channels;

    std::vector<float> frame(samples_per_frame);
    std::vector<unsigned char> encoded(4096);

    opus_int64 granulepos = 0;
    long packetno = 2;

    int total_frames = num_samples / samples_per_frame;

    for (int i = 0; i < total_frames; ++i) {
        const float* src = pcm + i * samples_per_frame;
        std::memcpy(frame.data(), src, samples_per_frame * sizeof(float));

        int nb_bytes = opus_encode_float(enc,
                                         frame.data(),
                                         frame_size,
                                         encoded.data(),
                                         (opus_int32)encoded.size());
        if (nb_bytes < 0) {
            std::fclose(f);
            ogg_stream_clear(&os);
            opus_encoder_destroy(enc);
            return false;
        }

        granulepos += frame_size; // bei 48k: Samples pro Kanal

        ogg_packet pkt{};
        pkt.packet = encoded.data();
        pkt.bytes  = nb_bytes;
        pkt.b_o_s  = 0;
        pkt.e_o_s  = 0;
        pkt.granulepos = granulepos;
        pkt.packetno   = packetno++;

        ogg_stream_packetin(&os, &pkt);

        while (ogg_stream_pageout(&os, &page)) {
            if (!write_ogg_page(f, &page)) {
                std::fclose(f);
                ogg_stream_clear(&os);
                opus_encoder_destroy(enc);
                return false;
            }
        }
    }

    // ---------------------------------------------------------
    // 4) EOS-Paket (leeres Packet mit e_o_s)
    // ---------------------------------------------------------
    ogg_packet pkt_eos{};
    pkt_eos.packet = nullptr;
    pkt_eos.bytes  = 0;
    pkt_eos.b_o_s  = 0;
    pkt_eos.e_o_s  = 1;
    pkt_eos.granulepos = granulepos;
    pkt_eos.packetno   = packetno++;

    ogg_stream_packetin(&os, &pkt_eos);

    while (ogg_stream_flush(&os, &page)) {
        if (!write_ogg_page(f, &page)) {
            std::fclose(f);
            ogg_stream_clear(&os);
            opus_encoder_destroy(enc);
            return false;
        }
    }

    std::fclose(f);
    ogg_stream_clear(&os);
    opus_encoder_destroy(enc);
    return true;
}


// opus_save.cpp
#include <opus/opus.h>
#include <ogg/ogg.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>

void opus_save(const std::string& path,
               const float* pcm,
               size_t frames,
               int channels,
               int bitrate = 128000)
{
    int err = 0;
    OpusEncoder* enc = opus_encoder_create(48000, channels, OPUS_APPLICATION_AUDIO, &err);
    if (!enc)
        throw std::runtime_error("opus_encoder_create failed");

    opus_encoder_ctl(enc, OPUS_SET_BITRATE(bitrate)); // Average Bitrate, not a max.
    opus_encoder_ctl(enc, OPUS_SET_VBR(1));

    FILE* fp = std::fopen(path.c_str(), "wb");
    if (!fp)
        throw std::runtime_error("cannot open output file");

    ogg_stream_state os;
    ogg_stream_init(&os, 12345); // deterministic stream id

    // --- Write OpusHead ---
    {
        unsigned char head[19];
        memcpy(head, "OpusHead", 8);
        head[8] = 1;               // version
        head[9] = channels;
        *reinterpret_cast<uint16_t*>(head + 10) = 312; // pre-skip
        *reinterpret_cast<uint32_t*>(head + 12) = 48000;
        *reinterpret_cast<uint16_t*>(head + 16) = 0;   // gain
        head[18] = 0;              // channel mapping

        ogg_packet pkt{};
        pkt.packet = head;
        pkt.bytes = sizeof(head);
        pkt.b_o_s = 1;
        pkt.e_o_s = 0;
        pkt.granulepos = 0;
        pkt.packetno = 0;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page)) {
            fwrite(page.header, 1, page.header_len, fp);
            fwrite(page.body, 1, page.body_len, fp);
        }
    }

    // --- Write OpusTags ---
    {
        const char* vendor = "Copilot";
        std::string tags = "OpusTags";
        tags.push_back(0); tags.push_back(0); tags.push_back(0); tags.push_back(0); // vendor length
        tags.append(vendor);
        tags.append("\x01\x00\x00\x00"); // 1 tag
        tags.append("ENCODER=Copilot");

        ogg_packet pkt{};
        pkt.packet = (unsigned char*)tags.data();
        pkt.bytes = tags.size();
        pkt.b_o_s = 0;
        pkt.e_o_s = 0;
        pkt.granulepos = 0;
        pkt.packetno = 1;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page)) {
            fwrite(page.header, 1, page.header_len, fp);
            fwrite(page.body, 1, page.body_len, fp);
        }
    }

    // --- Encode PCM frames ---
    constexpr int FRAME = 960; // 20ms @ 48k
    std::vector<unsigned char> out(4096);

    size_t pos = 0;
    int packetno = 2;
    ogg_page page;

    while (pos < frames) {
        size_t todo = std::min<size_t>(FRAME, frames - pos);

        int ret = opus_encode_float(enc,
                                    pcm + pos * channels,
                                    todo,
                                    out.data(),
                                    out.size());
        if (ret < 0)
            throw std::runtime_error("opus_encode_float failed");

        ogg_packet pkt{};
        pkt.packet = out.data();
        pkt.bytes = ret;
        pkt.b_o_s = 0;
        pkt.e_o_s = (pos + todo >= frames);
        pkt.granulepos = (pos + todo) * channels;
        pkt.packetno = packetno++;

        ogg_stream_packetin(&os, &pkt);

        while (ogg_stream_pageout(&os, &page)) {
            fwrite(page.header, 1, page.header_len, fp);
            fwrite(page.body, 1, page.body_len, fp);
        }

        pos += todo;
    }

    ogg_stream_clear(&os);
    opus_encoder_destroy(enc);
    fclose(fp);
}


void opus_save(const std::string& path,
               const float* pcm,
               size_t frames,
               int channels,
               int bitrate = 128000)
{
    int err = 0;
    OpusEncoder* enc = opus_encoder_create(48000, channels, OPUS_APPLICATION_AUDIO, &err);
    if (!enc)
        throw std::runtime_error("opus_encoder_create failed");

    opus_encoder_ctl(enc, OPUS_SET_BITRATE(bitrate));
    opus_encoder_ctl(enc, OPUS_SET_VBR(1));

    FILE* fp = std::fopen(path.c_str(), "wb");
    if (!fp)
        throw std::runtime_error("cannot open output file");

    ogg_stream_state os;
    ogg_stream_init(&os, 12345); // deterministic stream id

    // --- Write OpusHead ---
    {
        unsigned char head[19];
        memcpy(head, "OpusHead", 8);
        head[8] = 1;               // version
        head[9] = channels;
        *reinterpret_cast<uint16_t*>(head + 10) = 312; // pre-skip
        *reinterpret_cast<uint32_t*>(head + 12) = 48000;
        *reinterpret_cast<uint16_t*>(head + 16) = 0;   // gain
        head[18] = 0;              // channel mapping

        ogg_packet pkt{};
        pkt.packet = head;
        pkt.bytes = sizeof(head);
        pkt.b_o_s = 1;
        pkt.e_o_s = 0;
        pkt.granulepos = 0;
        pkt.packetno = 0;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page)) {
            fwrite(page.header, 1, page.header_len, fp);
            fwrite(page.body, 1, page.body_len, fp);
        }
    }

    // --- Write OpusTags ---
    {
        const char* vendor = "Copilot";
        std::string tags = "OpusTags";
        tags.push_back(0); tags.push_back(0); tags.push_back(0); tags.push_back(0); // vendor length
        tags.append(vendor);
        tags.append("\x01\x00\x00\x00"); // 1 tag
        tags.append("ENCODER=Copilot");

        ogg_packet pkt{};
        pkt.packet = (unsigned char*)tags.data();
        pkt.bytes = tags.size();
        pkt.b_o_s = 0;
        pkt.e_o_s = 0;
        pkt.granulepos = 0;
        pkt.packetno = 1;

        ogg_stream_packetin(&os, &pkt);

        ogg_page page;
        while (ogg_stream_flush(&os, &page)) {
            fwrite(page.header, 1, page.header_len, fp);
            fwrite(page.body, 1, page.body_len, fp);
        }
    }

    // --- Encode PCM frames ---
    constexpr int FRAME = 960; // 20ms @ 48k
    std::vector<unsigned char> out(4096);

    size_t pos = 0;
    int packetno = 2;
    ogg_page page;

    while (pos < frames) {
        size_t todo = std::min<size_t>(FRAME, frames - pos);

        int ret = opus_encode_float(enc,
                                    pcm + pos * channels,
                                    todo,
                                    out.data(),
                                    out.size());
        if (ret < 0)
            throw std::runtime_error("opus_encode_float failed");

        ogg_packet pkt{};
        pkt.packet = out.data();
        pkt.bytes = ret;
        pkt.b_o_s = 0;
        pkt.e_o_s = (pos + todo >= frames);
        pkt.granulepos = (pos + todo) * channels;
        pkt.packetno = packetno++;

        ogg_stream_packetin(&os, &pkt);

        while (ogg_stream_pageout(&os, &page)) {
            fwrite(page.header, 1, page.header_len, fp);
            fwrite(page.body, 1, page.body_len, fp);
        }

        pos += todo;
    }

    ogg_stream_clear(&os);
    opus_encoder_destroy(enc);
    fclose(fp);
}
*/
