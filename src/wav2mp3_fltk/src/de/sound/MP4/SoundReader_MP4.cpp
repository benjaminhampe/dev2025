#include <de/sound/MP4/SoundReader_MP4.h>
#include <de/sound/MP4/AAC/AAC_Decoder.h>

// #include <Ap4.h>
// #include <Ap4Interfaces.h>
// #include <Ap4DecoderConfigDescriptor.h>

// extern "C" {
// #include <neaacdec.h>
// #include <opus/opus.h>
// }

// #include <alac/ALACDecoder.h>

/*
🎯 MP4 TrackType: 'soun'
        from [moov -> trak -> mdia -> hdlr -> handler_type]
*/

namespace de {
namespace sound {

bool
load_sound_mp4_f32(
    Sound& sound,
    const std::string& uri,
    const de::SoundLoadOptions& options)
{
    DE_BENNI("Parse ",uri)

    de::file::mp4::MP4 mp4;

    if (!de::file::mp4::Parser::parse(uri,mp4))
    {
        DE_ERROR("Parser failed")
        return false;
    }

    // We only decode AAC tracks with lib FAAD2 for now...
    de::file::mp4::MP4::TrackByLibSupport libSupport =
        mp4.getTrackByLibSupport(de::file::mp4::FAAD2);

    if (!libSupport.trak)
    {
        DE_ERROR("No FAAD2 compatible trak")
        return false;
    }

    DE_OK("Got FAAD2 compatible trak")

    if (libSupport.stbls.empty())
    {
        DE_ERROR("No FAAD2 compatible stbl")
        return false;
    }

    DE_OK("Got (",libSupport.stbls.size(),") FAAD2 compatible stbls")

    auto stbl = libSupport.stbls.front();

    // =========================================
    // Collect MP4_SampleSizes:
    // =========================================

    file::mp4::MP4_SampleSizes stsz;

    if (stbl->m_stsz.empty())
    {
        if (stbl->m_stz2.empty())
        {
            DE_ERROR("No stsz or stsz")
        }
        else
        {
            auto v = stbl->m_stz2.front().m_sample_sizes;
            DE_OK("Got stz2 ",v.size())
            stsz.insert(stsz.end(), v.begin(), v.end());
        }
    }
    else
    {
        auto v = stbl->m_stsz.front().m_sample_sizes;
        DE_OK("Got stsz ",v.size())
        stsz.insert(stsz.end(), v.begin(), v.end());
    }

    // =========================================
    // Collect MP4_SampleToChunk:
    // =========================================

    file::mp4::MP4_SampleToChunk stsc;

    if (stbl->m_stsc.empty())
    {
        DE_ERROR("No stsc")
    }
    else
    {
        auto v = stbl->m_stsc.front().m_entries;
        DE_OK("Got stsc ",v.size())
        stsc.insert(stsc.end(), v.begin(), v.end());
    }

    // =========================================
    // Collect MP4_ChunkOffsets:
    // =========================================

    file::mp4::MP4_ChunkOffsets stco;

    if (stbl->m_stco.empty())
    {
        if (stbl->m_co64.empty())
        {
            DE_ERROR("No stco or co64")
        }
        else
        {
            for (const auto & a : stbl->m_co64)
            {
                const auto & v = a.m_chunk_offsets;
                DE_OK("Got co64 ",v.size())
                stco.insert(stco.end(), v.begin(), v.end());
            }
        }
    }
    else
    {
        for (const auto & a : stbl->m_stco)
        {
            const auto & v = a.m_chunk_offsets;
            DE_OK("Got stco ",v.size())
            stco.insert(stco.end(), v.begin(), v.end());
        }
    }

    // =========================================
    // Collect MP4_DecodingTimeToSample:
    // =========================================

    file::mp4::MP4_DecodingTimeToSample stts;

    if (stbl->m_stts.empty())
    {
        DE_ERROR("No stts")
    }
    else
    {
        auto v = stbl->m_stts.front().m_entries;
        DE_OK("Got stts ",v.size())
        stts.insert(stts.end(), v.begin(), v.end());
    }

    // =========================================
    // Collect MP4_CompositionTimeToSample:
    // =========================================

    file::mp4::MP4_CompositionTimeToSample ctts;

    if (stbl->m_ctts.empty())
    {
        DE_ERROR("No ctts")
    }
    else
    {
        auto v = stbl->m_ctts.front().m_entries;
        DE_OK("Got ctts ",v.size())
        ctts.insert(ctts.end(), v.begin(), v.end());
    }

    DE_OK("Build MP4_SampleInfoTable:")
    DE_OK("stsz = ",stsz.size())
    DE_OK("stsc = ",stsc.size())
    DE_OK("stco = ",stco.size())
    DE_OK("stts = ",stts.size())
    DE_OK("ctts = ",ctts.size())

    // =========================================
    // build_sample_table
    // =========================================
    de::file::mp4::MP4_SampleInfoTable table =
        de::file::mp4::build_sample_table(
            stsz,
            stsc,
            stco,
            stts,
            ctts);

    DE_OK("Got MP4_SampleInfoTable ",table.size())

    if (!libSupport.esds)
    {
        DE_ERROR("File no esds")
        return false;
    }

    const file::mp4::aac::AscInfo & asc = libSupport.esds->m_esd.decConfig.decSpecific.m_asc;

    File file(uri, eFileMode::Read);
    if (!file.is_open())
    {
        DE_ERROR("File not open")
        return false;
    }

    sound.m_uri = uri;
    AAC_Decoder aacDecoder;
    aacDecoder.readFileInfo(sound, asc, table.size());

    DE_BENNI("Sound ", sound.str())

    u32 callbacks = 0;
    aacDecoder.decodeAAC(file,asc,table,
        [&](const float* pcm, size_t samples)
        {
            const uint8_t* __restrict__ beg = reinterpret_cast<const uint8_t*>(pcm);
            const uint8_t* __restrict__ end = beg + (samples * sizeof(float));
            sound.m_samples.insert(sound.m_samples.end(), beg, end);
        }

        // [&](const std::vector<float>& pcm)
        // {
        //     sound.m_samples.insert(sound.m_samples.end(),pcm.begin(),pcm.end());
        //     //DE_OK("Got ",pcm.size()," pcm samples")
        //     callbacks++;
        // }
    );

    DE_BENNI("Got ", callbacks, " pcm callbacks with ", sound.m_samples.size(), " samples")

/*
    u32 ch = sound.m_fileInfo.channelCount;
    u64 fc = samples.size() / ch;
    if (ch > 0)
    {
        sound.m_samples.resize(fc);
        DE_OK("Got L ",fc)
        for (u64 i = 0; i < fc; ++i)
        {
            sound.m_L[i] = samples[i*ch];
        }
    }

    if (ch > 1)
    {
        sound.m_R.resize(fc);
        DE_OK("Got R ",fc)
        for (u64 i = 0; i < fc; ++i)
        {
            sound.m_R[i] = samples[i*ch + 1u];
        }
    }

    AP4_ByteStream* input;
    AP4_Result hr;
    hr = AP4_FileByteStream::Create(uri.c_str(),
                            AP4_FileByteStream::STREAM_MODE_READ,
                            input);

    if (AP4_FAILED(hr))
    {
        DE_ERROR("Cannot open input file ",uri," e = ",hr)
        return false;
    }

    AP4_File file(*input, true);
    //ShowFileInfo(*file);

    AP4_Movie* movie = file.GetMovie();
    AP4_FtypAtom* ftyp = file.GetFileType();
    if (!movie)
    {
        DE_ERROR("No movie ",uri)
        input->Release();
        //delete input;
        return false;
    }

    AP4_List<AP4_Track>& tracks = movie->GetTracks();
    DE_OK("TrackCount = ",tracks.ItemCount())

    int index=1;
    for (AP4_List<AP4_Track>::Item* track_item = tracks.FirstItem();
         track_item;
         track_item = track_item->GetNext(), ++index)
    {
        auto track = *track_item->GetData();
        switch (track.GetType())
        {
        case AP4_Track::TYPE_AUDIO: DE_OK("Track[",index,"] Audio") break;
        case AP4_Track::TYPE_VIDEO: DE_OK("Track[",index,"] Video") break;
        case AP4_Track::TYPE_HINT: DE_OK("Track[",index,"] Hint") break;
        case AP4_Track::TYPE_SYSTEM: DE_OK("Track[",index,"] System") break;
        case AP4_Track::TYPE_TEXT: DE_OK("Track[",index,"] Text") break;
        case AP4_Track::TYPE_JPEG: DE_OK("Track[",index,"] JPEG") break;
        case AP4_Track::TYPE_SUBTITLES: DE_OK("Track[",index,"] Subtitles") break;
        default: DE_WARN("Track[",index,"] Unknown") break;
        }
    }

    DE_OK("Passed loop")

    AP4_Track* audio_track = movie->GetTrack(AP4_Track::TYPE_AUDIO);
    if (!audio_track)
    {
        DE_ERROR("No audio track ",uri)
        input->Release();
        //delete input;
        return false;
    }

    DE_OK("Got audio track")

    const AP4_Ordinal sampleCount = audio_track->GetSampleCount();
    AP4_SampleDescription* sd = audio_track->GetSampleDescription(0);
    auto* audio_sd = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);
    if (!audio_sd)
    {
        DE_ERROR("No audio sample description ",uri)
        //delete file;
        input->Release();
        //delete input;
        return false;
    }

    DE_OK("Got audio description")

    // ------------------------------------------------------------
    // FILL YOUR EXACT FileInfo
    // ------------------------------------------------------------
    sound.m_uri = uri;
    sound.m_fileInfo.sampleRate   = audio_sd->GetSampleRate();
    sound.m_fileInfo.channelCount = audio_sd->GetChannelCount();
    sound.m_fileInfo.frameCount   = sampleCount;
    sound.m_fileInfo.sampleType   = FileInfo::ST_F32;
    if (sound.m_fileInfo.sampleRate < 1)
    {
        sound.m_fileInfo.duration = 0;
    }
    else
    {
        sound.m_fileInfo.duration = (u64)((double)sampleCount / sound.m_fileInfo.sampleRate * 1e9);
    }

    DE_OK("Got header")
    DE_OK(sound.m_fileInfo.str())

    DE_OK("Cleanup")
    //delete file;
    //delete input;
    input->Release();

*/
    DE_OK("End function")
    return true;
}


} // end namespace sound.
} // end namespace de.

#if 0

bool load_sound_mp4_f32_opus(Sound& sound, const std::string& uri)
{

    sound.m_uri.clear();
    sound.m_samples.clear();
    sound.m_fileInfo = {};

    // ------------------------------------------------------------
    // OPEN FILE + BASIC INFO
    // ------------------------------------------------------------
    AP4_ByteStream* input = nullptr;
    if (AP4_FileByteStream::Create(uri.c_str(),
                                   AP4_FileByteStream::STREAM_MODE_READ,
                                   input) != AP4_SUCCESS)
        return false;

    AP4_File file(*input, true);
    AP4_Movie* movie = file.GetMovie();
    if (!movie) return false;

    AP4_Track* track = movie->GetTrack(AP4_Track::TYPE_AUDIO);
    if (!track) return false;

    const AP4_Ordinal sampleCount = track->GetSampleCount();
    AP4_SampleDescription* sd = track->GetSampleDescription(0);
    auto* asd = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);
    if (!asd) return false;

    const u32 sampleRate = asd->GetSampleRate();
    const u32 channels   = asd->GetChannelCount();

    sound.m_uri = uri;
    sound.m_fileInfo.sampleRate   = sampleRate;
    sound.m_fileInfo.channelCount = channels;
    sound.m_fileInfo.sampleType   = FileInfo::ST_F32;

    // ------------------------------------------------------------
    // OPUS ONLY: detect + get OpusHead
    // ------------------------------------------------------------
    if (sd->GetFormat() != AP4_SAMPLE_FORMAT_OPUS)
        return false; // here we only handle Opus

    AP4_Mp4sSampleEntry* mp4s = AP4_DYNAMIC_CAST(AP4_Mp4sSampleEntry, sd);
    if (!mp4s)
        return false;

    // Look for dOps atom (OpusSpecificBox)
    AP4_Atom* atom = mp4s->GetChild(AP4_ATOM_TYPE_DOPS);
    if (!atom)
        return false; // not Opus

    AP4_DOpsAtom* dOps = AP4_DYNAMIC_CAST(AP4_DOpsAtom, atom);
    if (!dOps)
        return false;


    const AP4_DataBuffer& opusHead = opusAtom->GetOpusHead();

    const AP4_DataBuffer& head = opusEntry->GetOpusHead();
    // (You can parse OpusHead if you need pre-skip, etc.)

    // ------------------------------------------------------------
    // CREATE OPUS DECODER
    // ------------------------------------------------------------
    int err = 0;
    OpusDecoder* opus = opus_decoder_create(sampleRate, channels, &err);
    if (err != OPUS_OK || !opus)
        return false;

    const int maxFrames = 5760; // 120 ms @ 48k
    std::vector<float> tmp(maxFrames * channels);

    // ------------------------------------------------------------
    // PASS 1: scan all packets, count total decoded frames
    // ------------------------------------------------------------
    u64 totalFrames = 0;

    for (AP4_Ordinal i = 0; i < sampleCount; ++i)
    {
        AP4_Sample sample;
        AP4_DataBuffer buffer;

        if (track->ReadSample(i, sample, buffer) != AP4_SUCCESS)
        {
            opus_decoder_destroy(opus);
            return false;
        }

        const unsigned char* data = buffer.GetData();
        const opus_int32 size     = (opus_int32)buffer.GetDataSize();

        int f = opus_decode_float(opus,
                                  data,
                                  size,
                                  tmp.data(),
                                  maxFrames,
                                  0);
        if (f < 0)
        {
            opus_decoder_destroy(opus);
            return false;
        }

        totalFrames += (u64)f;
    }

    // fill FileInfo now that we know frameCount/duration
    sound.m_fileInfo.frameCount = totalFrames;
    sound.m_fileInfo.duration   =
        (u64)((double)totalFrames / (double)sampleRate * 1e9);

    // ------------------------------------------------------------
    // PREALLOCATE m_samples
    // ------------------------------------------------------------
    sound.m_samples.resize(totalFrames * channels);

    // ------------------------------------------------------------
    // RESET DECODER FOR PASS 2
    // ------------------------------------------------------------
    opus_decoder_destroy(opus);
    err  = 0;
    opus = opus_decoder_create(sampleRate, channels, &err);
    if (err != OPUS_OK || !opus)
        return false;

    // ------------------------------------------------------------
    // PASS 2: decode again into preallocated m_samples
    // ------------------------------------------------------------
    u64 writePos = 0;

    for (AP4_Ordinal i = 0; i < sampleCount; ++i)
    {
        AP4_Sample sample;
        AP4_DataBuffer buffer;

        if (track->ReadSample(i, sample, buffer) != AP4_SUCCESS)
        {
            opus_decoder_destroy(opus);
            return false;
        }

        const unsigned char* data = buffer.GetData();
        const opus_int32 size     = (opus_int32)buffer.GetDataSize();

        int f = opus_decode_float(opus,
                                  data,
                                  size,
                                  tmp.data(),
                                  maxFrames,
                                  0);
        if (f < 0)
        {
            opus_decoder_destroy(opus);
            return false;
        }

        const u32 totalSamples = (u32)f * channels;

        std::memcpy(&sound.m_samples[writePos],
                    tmp.data(),
                    totalSamples * sizeof(float));

        writePos += totalSamples;
    }

    opus_decoder_destroy(opus);
    input->Release();

    return true;
}


bool load_sound_mp4_f32(Sound& sound, const std::string& uri)
{
    sound.m_uri.clear();
    sound.m_samples.clear();
    sound.m_fileInfo = {};

    // ------------------------------------------------------------
    // OPEN FILE
    // ------------------------------------------------------------
    AP4_ByteStream* input = nullptr;
    if (AP4_FileByteStream::Create(uri.c_str(),
                                   AP4_FileByteStream::STREAM_MODE_READ,
                                   input) != AP4_SUCCESS)
        return false;

    AP4_File file(*input, true);
    AP4_Movie* movie = file.GetMovie();
    if (!movie) return false;

    AP4_Track* track = movie->GetTrack(AP4_Track::TYPE_AUDIO);
    if (!track) return false;

    const AP4_Ordinal sampleCount = track->GetSampleCount();
    AP4_SampleDescription* sd = track->GetSampleDescription(0);
    auto* asd = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);
    if (!asd) return false;

    const u32 sampleRate = asd->GetSampleRate();
    const u32 channels   = asd->GetChannelCount();

    // ------------------------------------------------------------
    // FILL YOUR EXACT FileInfo
    // ------------------------------------------------------------
    sound.m_uri = uri;
    sound.m_fileInfo.sampleRate   = sampleRate;
    sound.m_fileInfo.channelCount = channels;
    sound.m_fileInfo.frameCount   = sampleCount;
    sound.m_fileInfo.sampleType   = FileInfo::ST_F32;
    sound.m_fileInfo.duration     = (u64)((double)sampleCount / sampleRate * 1e9);

    // ------------------------------------------------------------
    // PREALLOCATE PCM BUFFER
    // ------------------------------------------------------------
    sound.m_samples.resize((u64)sampleCount * channels);

    // ------------------------------------------------------------
    // DETECT CODEC
    // ------------------------------------------------------------
    AP4_UI32 fmt = sd->GetFormat();

    bool isAAC  = (fmt == AP4_SAMPLE_FORMAT_MP4A);
    bool isALAC = (fmt == AP4_SAMPLE_FORMAT_ALAC);
    bool isOPUS = (fmt == AP4_SAMPLE_FORMAT_OPUS);

    // ------------------------------------------------------------
    // EXTRACT CODEC CONFIG
    // ------------------------------------------------------------
    std::vector<u8> asc;
    std::vector<u8> alacCookie;
    std::vector<u8> opusHead;
    u32 alacSpf = 0;
    u32 alacBitDepth = 0;

    if (isAAC)
    {
        AP4_EsdsAtom* esds = nullptr;
        sd->GetChild(esds);
        auto* dcd = esds->GetDecoderConfigDescriptor();
        auto* dsi = dcd->GetDecoderSpecificInfo();
        const AP4_DataBuffer& buf = dsi->GetData();
        asc.assign(buf.GetData(), buf.GetData() + buf.GetDataSize());
    }
    else if (isALAC)
    {
        auto* alac = AP4_DYNAMIC_CAST(AP4_AlacAudioSampleEntry, sd);
        const AP4_DataBuffer& cookie = alac->GetCodecSpecificData();
        alacCookie.assign(cookie.GetData(),
                          cookie.GetData() + cookie.GetDataSize());
        alacSpf      = alac->GetSamplesPerFrame();
        alacBitDepth = alac->GetSampleSize();
    }
    else if (isOPUS)
    {
        auto* opus = AP4_DYNAMIC_CAST(AP4_OpusSampleEntry, sd);
        const AP4_DataBuffer& head = opus->GetOpusHead();
        opusHead.assign(head.GetData(), head.GetData() + head.GetDataSize());
    }

    // ------------------------------------------------------------
    // INITIALIZE DECODERS
    // ------------------------------------------------------------
    NeAACDecHandle aac = nullptr;
    ALACDecoder alac;
    OpusDecoder* opus = nullptr;

    std::vector<int32_t> alacInt;
    std::vector<float> opusTmp;

    if (isAAC)
    {
        aac = NeAACDecOpen();
        NeAACDecConfigurationPtr c = NeAACDecGetCurrentConfiguration(aac);
        c->outputFormat = FAAD_FMT_FLOAT;
        NeAACDecSetConfiguration(aac, c);

        long sr = 0;
        unsigned char ch = 0;

        NeAACDecInit2(aac,
                      asc.data(),
                      asc.size(),
                      (unsigned long*)&sr,
                      &ch);
    }

    if (isALAC)
    {
        alac.Init(alacCookie.data(), alacCookie.size());
        alacInt.resize(alacSpf * channels);
    }

    if (isOPUS)
    {
        int err = 0;
        opus = opus_decoder_create(sampleRate, channels, &err);
        opusTmp.resize(5760 * channels);
    }

    // ------------------------------------------------------------
    // DECODE PACKETS INTO m_samples
    // ------------------------------------------------------------
    u64 writePos = 0;

    for (AP4_Ordinal i = 0; i < sampleCount; ++i)
    {
        AP4_Sample sample;
        AP4_DataBuffer buffer;

        if (track->ReadSample(i, sample, buffer) != AP4_SUCCESS)
            return false;

        const u8* data = buffer.GetData();
        const u32 size = buffer.GetDataSize();

        // ---------------- AAC ----------------
        if (isAAC)
        {
            NeAACDecFrameInfo fi;
            void* sampleBuf = nullptr;

            NeAACDecDecode2(aac,
                            &fi,
                            const_cast<u8*>(data),
                            size,
                            &sampleBuf,
                            0);

            float* pcm = (float*)sampleBuf;
            u32 samples = fi.samples;

            std::memcpy(&sound.m_samples[writePos],
                        pcm,
                        samples * sizeof(float));

            writePos += samples;
        }

        // ---------------- ALAC ----------------
        else if (isALAC)
        {
            uint32_t outFrames = alacSpf;

            alac.Decode((void*)data,
                        size,
                        alacInt.data(),
                        &outFrames,
                        channels);

            const float scale = 1.0f / float(1u << (alacBitDepth - 1));
            u32 totalSamples = outFrames * channels;

            for (u32 s = 0; s < totalSamples; ++s)
                sound.m_samples[writePos + s] = alacInt[s] * scale;

            writePos += totalSamples;
        }

        // ---------------- OPUS ----------------
        else if (isOPUS)
        {
            int f = opus_decode_float(opus,
                                      data,
                                      size,
                                      opusTmp.data(),
                                      5760,
                                      0);

            u32 totalSamples = f * channels;

            std::memcpy(&sound.m_samples[writePos],
                        opusTmp.data(),
                        totalSamples * sizeof(float));

            writePos += totalSamples;
        }
    }

    // cleanup
    if (aac)  NeAACDecClose(aac);
    if (opus) opus_decoder_destroy(opus);
    input->Release();

    return true;
}


bool load_sound_mp4_f32(Sound& sound, const std::string& uri)
{
    AP4_ByteStream* stream = nullptr;
    AP4_Result hr = AP4_FileByteStream::Create(uri.c_str(), AP4_FileByteStream::STREAM_MODE_READ, stream);
    if (AP4_FAILED(hr))
    {
        DE_ERROR("No bytestream ", uri)
        return false;
    }

    //AP4_AutoPtr<AP4_ByteStream> streamHolder(stream);
    std::unique_ptr<AP4_ByteStream> streamHolder(stream);
    AP4_File file(*stream);

    AP4_Movie* movie = file.GetMovie();
    if (!movie)
    {
        DE_ERROR("No movie ", uri)
        return false;
    }

    // Audio-Track finden
    AP4_Track* track = movie->GetTrack(AP4_Track::TYPE_AUDIO);
    if (!track)
    {
        DE_ERROR("No track ", uri)
        return false;
    }

    // Sample Description holen
    AP4_SampleDescription* sd = track->GetSampleDescription(0);
    if (!sd)
    {
        DE_ERROR("No SampleDescription ", uri)
        return false;
    }

    AP4_AudioSampleDescription* audio_desc = AP4_DYNAMIC_CAST(AP4_AudioSampleDescription, sd);
    if (audio_desc == NULL)
    {
        DE_ERROR("No Audio SampleDescription ", uri)
        return false;
    }

    // Sound-Objekt füllen
    sound.m_uri = uri;
    sound.m_fileInfo.sampleRate = audio_desc->GetSampleRate();
    sound.m_fileInfo.frameCount = track->GetSampleCount();
    sound.m_fileInfo.channelCount = audio_desc->GetChannelCount();
    sound.m_fileInfo.duration = (uint64_t)track->GetDurationMs() * 1000000ull;

    // Decoder erzeugen (Bento4 macht das automatisch)
    // AP4_Decoder* decoder = nullptr;
    // if (AP4_DecoderFactory::CreateDecoder(sd, decoder) != AP4_SUCCESS || !decoder)
    //     return false;

    // AP4_AutoPtr<AP4_Decoder> decoderHolder(decoder);

    // PCM sammeln
    TAlignedVector<float> pcm;
    pcm.reserve(1024 * 1024);

    AP4_Sample sample;
    AP4_DataBuffer sampleData;
    AP4_DataBuffer pcmData;

    AP4_Ordinal index = 0;
    while (track->ReadSample(index++, sample, sampleData) == AP4_SUCCESS)
    {
        if (decoder->DecodeFrame(sampleData, pcmData) != AP4_SUCCESS)
            continue;

        const float* f = reinterpret_cast<const float*>(pcmData.GetData());
        size_t count = pcmData.GetDataSize() / sizeof(float);

        pcm.insert(pcm.end(), f, f + count);
    }

    // Sound-Objekt füllen
    sound.m_samples    = std::move(pcm);

    return true;
}

#endif

