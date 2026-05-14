

bool UnifiedDecoder::openMp4(const char* path)
{
    if (AP4_FileByteStream::Create(path, AP4_FileByteStream::STREAM_MODE_READ, m_input))
        return false;

    m_file = new AP4_File(*m_input, true);
    m_movie = m_file->GetMovie();
    if (!m_movie) return false;

    m_track = m_movie->GetTrack(AP4_Track::TYPE_AUDIO);
    if (!m_track) return false;

    m_sampleCount = m_track->GetSampleCount();
    m_sampleIndex = 0;

    auto* sd  = m_track->GetSampleDescription(0);
    auto* asd = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);

    info.sampleRate = asd->GetSampleRate();
    info.channels   = asd->GetChannelCount();
    info.durationNs = (AP4_UI64)m_track->GetDuration() * 1000000000ULL /
                      m_track->GetMediaTimeScale();

    // Detect codec
    AP4_UI32 fmt = asd->GetFormat();

    if (fmt == AP4_SAMPLE_FORMAT_MP4A) {
        m_codec = AudioCodec::AAC;
        info.codec = AudioCodec::AAC;

        // Get ASC from ESDS
        AP4_EsdsAtom* esds = nullptr;
        sd->GetChild(esds);
        if (!esds) return false;

        const AP4_DataBuffer& asc =
            esds->GetDecoderConfigDescriptor()->GetDecoderSpecificInfo()->GetData();

        m_aac = NeAACDecOpen();
        auto cfg = NeAACDecGetCurrentConfiguration(m_aac);
        cfg->outputFormat = FAAD_FMT_FLOAT;
        NeAACDecSetConfiguration(m_aac, cfg);

        long r = NeAACDecInit2(m_aac, asc.GetData(), asc.GetDataSize(),
                               &info.sampleRate, (unsigned char*)&info.channels);
        if (r < 0) return false;
    }
    else if (fmt == AP4_SAMPLE_FORMAT_ALAC) {
        m_codec = AudioCodec::ALAC;
        info.codec = AudioCodec::ALAC;

        auto* alacEntry = AP4_DYNAMIC_CAST(AP4_AlacAudioSampleEntry, asd);
        info.bitDepth = alacEntry->GetSampleSize();

        const AP4_DataBuffer& cookie = alacEntry->GetCodecSpecificData();

        m_alac = new ALACDecoder();
        BitBuffer bits;
        BitBufferInit(&bits, (uint8_t*)cookie.GetData(), cookie.GetDataSize());
        m_alac->Init(&bits);

        info.sampleRate = m_alac->mConfig.sampleRate;
        info.channels   = m_alac->mConfig.numChannels;

        m_alacIntBuf.resize(4096 * info.channels); // typical max
    }
    else if (fmt == AP4_SAMPLE_FORMAT_OPUS) {
        m_codec = AudioCodec::OPUS;
        info.codec = AudioCodec::OPUS;

        int err = 0;
        info.sampleRate = 48000; // Opus decode rate
        auto* opusEntry = AP4_DYNAMIC_CAST(AP4_OpusAudioSampleEntry, asd);
        info.channels   = opusEntry->GetChannelCount();

        m_opus = opus_decoder_create(info.sampleRate, info.channels, &err);
        if (err != OPUS_OK) return false;
    } else {
        m_codec = AudioCodec::Unknown;
        return false;
    }

    return true;
}






bool UnifiedDecoder::decodeNext(DecodedBlock& out)
{
    if (m_sampleIndex >= m_sampleCount) return false;

    AP4_Sample sample;
    AP4_DataBuffer buffer;
    if (m_track->ReadSample(m_sampleIndex++, sample, buffer) != AP4_SUCCESS)
        return false;

    const uint8_t* data = buffer.GetData();
    size_t size = buffer.GetDataSize();

    out.channels    = info.channels;
    out.sampleRate  = info.sampleRate;
    out.samples     = nullptr;
    out.frameCount  = 0;

    if (m_codec == AudioCodec::AAC) {
        NeAACDecFrameInfo fi;
        float* pcm = (float*)NeAACDecDecode2(m_aac, &fi, data, size);
        if (fi.error != 0 || !pcm) return false;

        int totalSamples = fi.samples;          // all channels
        int frameCount   = totalSamples / fi.channels;

        float* dst = new float[totalSamples];
        std::memcpy(dst, pcm, totalSamples * sizeof(float));

        out.samples    = dst;
        out.frameCount = frameCount;
    }
    else if (m_codec == AudioCodec::ALAC) {
        uint32_t outFrames = 0;
        m_alac->Decode(data, (uint32_t)size, m_alacIntBuf.data(), &outFrames);
        if (outFrames == 0) return false;

        int totalSamples = outFrames * info.channels;
        float* dst = new float[totalSamples];

        const float scale = 1.0f / float(1u << (info.bitDepth - 1));
        for (int i = 0; i < totalSamples; ++i)
            dst[i] = m_alacIntBuf[i] * scale;

        out.samples    = dst;
        out.frameCount = (int)outFrames; // already per channel
    }
    else if (m_codec == AudioCodec::OPUS) {
        // max 120 ms @ 48k = 5760 frames
        int maxFrames = 5760;
        int totalSamples = maxFrames * info.channels;
        float* dst = new float[totalSamples];

        int frameCount = opus_decode_float(m_opus, data, (opus_int32)size,
                                           dst, maxFrames, 0);
        if (frameCount < 0) {
            delete[] dst;
            return false;
        }

        out.samples    = dst;
        out.frameCount = frameCount; // per channel
    }

    return true;
}







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
    if (m_input) { m_input->Release(); m_input = nullptr; }
}
