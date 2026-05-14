#pragma once

/*

struct Mp4AudioInfo {
    uint32_t sampleRate;
    uint32_t channelCount;
    uint32_t sampleSize;     // ALAC only
    uint32_t frameCount;
    uint64_t durationNs;
    std::string codec;
    std::vector<uint8_t> decoderConfig; // ASC or ALAC cookie
};

bool ParseMp4Audio(const char* path,
                   Mp4AudioInfo& info,
                   std::vector<std::vector<uint8_t>>& frames)
{
    AP4_ByteStream* input = nullptr;
    if (AP4_FileByteStream::Create(path, AP4_FileByteStream::STREAM_MODE_READ, input))
        return false;

    AP4_File file(*input, true);
    AP4_Movie* movie = file.GetMovie();
    if (!movie) return false;

    AP4_Track* track = movie->GetTrack(AP4_Track::TYPE_AUDIO);
    if (!track) return false;

    auto* sd = track->GetSampleDescription(0);
    auto* asd = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);

    info.sampleRate   = asd->GetSampleRate();
    info.channelCount = asd->GetChannelCount();
    info.frameCount   = track->GetSampleCount();
    info.durationNs   = (uint64_t)track->GetDuration() * 1000000000ULL
                        / track->GetMediaTimeScale();

    // Codec detection + decoder config
    if (asd->GetFormat() == AP4_SAMPLE_FORMAT_MP4A) {
        info.codec = "aac";

        AP4_EsdsAtom* esds = nullptr;
        sd->GetChild(esds);
        if (esds) {
            const AP4_DataBuffer& asc = esds->GetDecoderConfigDescriptor()->GetDecoderSpecificInfo()->GetData();
            info.decoderConfig.assign(asc.GetData(), asc.GetData() + asc.GetDataSize());
        }

        info.sampleSize = 0;
    }
    else if (asd->GetFormat() == AP4_SAMPLE_FORMAT_ALAC) {
        info.codec = "alac";

        auto* alac = AP4_DYNAMIC_CAST(AP4_AlacAudioSampleEntry, asd);
        info.sampleSize = alac->GetSampleSize();

        const AP4_DataBuffer& cookie = alac->GetCodecSpecificData();
        info.decoderConfig.assign(cookie.GetData(), cookie.GetData() + cookie.GetDataSize());
    }

    // Extract compressed frames
    frames.reserve(info.frameCount);

    AP4_Sample sample;
    AP4_DataBuffer buffer;

    for (AP4_Ordinal i = 0; i < info.frameCount; i++) {
        if (track->ReadSample(i, sample, buffer) != AP4_SUCCESS)
            continue;

        frames.emplace_back(buffer.GetData(),
                            buffer.GetData() + buffer.GetDataSize());
    }

    input->Release();
    return true;
}

*/