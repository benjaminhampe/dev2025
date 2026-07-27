#include <de/sound/OGG/SoundWriter_OGG_Vorbis.h>

#include <de/sound/SoundUtil.h>

#include <ogg/ogg.h>
#include <vorbis/vorbisenc.h>

namespace de {
namespace sound {

// ------------------------------------------------------------
// PCM-Konvertierung: uint8_t → float[-1..1]
// ------------------------------------------------------------
void convert_to_float(const Sound& snd, std::vector<float>& out)
{
    size_t frames = snd.m_frames;
    int ch = snd.m_channels;

    out.resize(frames * ch);

    if (snd.m_sampleType == SampleType::S16)
    {
        const int16_t* src = reinterpret_cast<const int16_t*>(snd.m_samples.data());
        for (size_t i = 0; i < frames * ch; ++i)
            out[i] = float(src[i]) / 32768.0f;
    }
    else if (snd.m_sampleType == SampleType::F32)
    {
        const float* src = reinterpret_cast<const float*>(snd.m_samples.data());
        std::memcpy(out.data(), src, frames * ch * sizeof(float));
    }
}

SampleType selectDstType( SampleType srcType )
{
    switch ( srcType )
    {
        case SampleType::U8: return SampleType::S16;
        case SampleType::S8: return SampleType::S16;
        case SampleType::S16: return SampleType::S16;
        case SampleType::S24: return SampleType::F32;
        case SampleType::S32: return SampleType::F32;
        case SampleType::F32: return SampleType::F32;
        case SampleType::F64: return SampleType::F32;
        default: return SampleType::Unknown;
    }
}

/*
    🎯 Vorbis VBR Qualitätsstufen (Q‑Werte)
        (offizielle Werte aus libvorbis, realistische Bitraten aus Messungen)
        Q‑Wert	Typische Bitrate (Stereo)	Beschreibung
        0.0	~64 kbps	niedrigste Qualität, aber brauchbar
        0.1	~80 kbps	besseres Low‑Quality
        0.2	~96 kbps	akzeptabel
        0.3	~112 kbps	mittlere Qualität
        0.4	~128 kbps	Standard MP3‑128kbps Niveau
        0.5	~160 kbps	gute Qualität
        0.6	~192 kbps	sehr gute Qualität
        0.7	~224 kbps	transparent für viele Quellen
        0.8	~256 kbps	sehr hohe Qualität
        0.9	~320 kbps	extrem hohe Qualität
        1.0	~500+ kbps	nahezu lossless, aber ineffizient

        Diese Werte sind die einzigen sinnvollen.
        Alles außerhalb 0.0–1.0 wird von libvorbis ignoriert oder führt zu Fehlern.

    🎧 Was du praktisch verwenden solltest
        Q = 0.4 → ~128 kbps (Standard, klein, gut)
        Q = 0.5 → ~160 kbps (empfohlen)
        Q = 0.6 → ~192 kbps (sehr gut)
        Q = 0.7 → ~224 kbps (fast transparent)
        Q = 0.8–1.0 → unnötig groß, aber möglich
*/
bool
save_sound_ogg_vorbis(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    auto srcType = sound.m_sampleType;
    auto dstType = SampleType::F32;
    auto converter = SampleTypeConverter::getConverter(srcType,dstType);
    if (!converter)
    {
        DE_ERROR("No converter to F32, ", sound.str(), ", uri = ", uri)
        return false;
    }

    File file(uri, eFileMode::Write);
    if (!file.is_open())
    {
        DE_ERROR("Cannot open output file, ",uri)
        return false;
    }

    options.onProgress(1);

    // Vorbis Info
    vorbis_info vi;
    vorbis_info_init(&vi);

    options.onProgress(2);

    if (vorbis_encode_init_vbr(&vi, sound.m_channels, sound.m_sampleRate, 0.4f))
    {
        vorbis_info_clear(&vi);
        DE_ERROR("vorbis_encode_init_vbr failed")
        return false;
    }

    vorbis_comment vc;
    vorbis_comment_init(&vc);
    vorbis_comment_add_tag(&vc, "ENCODER", "SoundWriter_OGG_Vorbis");

    vorbis_dsp_state vd;
    vorbis_block vb;

    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);

    ogg_stream_state os;
    ogg_stream_init(&os, 12345); // random serial

    // Header erzeugen
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);

    ogg_stream_packetin(&os, &header);
    ogg_stream_packetin(&os, &header_comm);
    ogg_stream_packetin(&os, &header_code);

    ogg_page og;
    while (ogg_stream_flush(&os, &og))
    {
        file.write(og.header, og.header_len);
        file.write(og.body, og.body_len);
    }

    options.onProgress(5);

    // PCM → Vorbis
    int64_t cFrames = 4096;
    int64_t cSamples = cFrames * sound.m_channels;
    TAlignedVector<float> chunkBuf(cSamples);

    int64_t pos = 0;
    while (pos < sound.m_frames)
    {
        int64_t desired = std::min<int64_t>(cFrames, sound.m_frames - pos);
        // DE_DEBUG("chunk = ",chunk, ", pos = ",pos)
        int64_t chunk = sound.read_frames(converter,chunkBuf.data(),desired,pos);

        const float* __restrict__ pSrc = chunkBuf.data();
        float** __restrict__ pAnalysis = vorbis_analysis_buffer(&vd, chunk * sound.m_channels); //

        for (int32_t c = 0; c < sound.m_channels; ++c)
        {
            float* __restrict__ pDst = pAnalysis[c];
            for (int64_t i = 0; i < chunk; ++i)
            {
                pDst[i] = pSrc[i * sound.m_channels + c];
            }
        }

        pos += chunk;

        vorbis_analysis_wrote(&vd, (int)chunk);

        // Encoding
        while (vorbis_analysis_blockout(&vd, &vb) == 1)
        {
            vorbis_analysis(&vb, nullptr);
            vorbis_bitrate_addblock(&vb);

            ogg_packet op;
            while (vorbis_bitrate_flushpacket(&vd, &op))
            {
                op.granulepos = vd.granulepos;      // WICHTIG: granulepos setzen
                ogg_stream_packetin(&os, &op);

                while (ogg_stream_pageout(&os, &og))
                {
                    file.write(og.header, og.header_len);
                    file.write(og.body, og.body_len);
                }
            }
        }

        // // *** FEHLTE VORHER: letzte unvollständige Page schreiben ***
        // while (ogg_stream_flush(&os, &og))
        // {
        //     file.write(og.header, og.header_len);
        //     file.write(og.body, og.body_len);
        // }

        options.onProgress(85.0 * double(pos) / double(sound.m_frames));
    }

    options.onProgress(90);

    // Ende: keine weiteren Samples
    vorbis_analysis_wrote(&vd, 0);

    while (vorbis_analysis_blockout(&vd, &vb) == 1)
    {
        // DE_DEBUG("endchunk")
        vorbis_analysis(&vb, nullptr);
        vorbis_bitrate_addblock(&vb);

        ogg_packet op;
        while (vorbis_bitrate_flushpacket(&vd, &op))
        {
            // DE_DEBUG("endflush")
            op.granulepos = vd.granulepos;          // auch hier granulepos setzen
            ogg_stream_packetin(&os, &op);

            while (ogg_stream_pageout(&os, &og))
            {
                // DE_DEBUG("endwrite")
                file.write(og.header, og.header_len);
                file.write(og.body, og.body_len);
            }
        }
    }

    options.onProgress(95);

    // FINALER FLUSH: letzte unvollständige Page schreiben
    while (ogg_stream_flush(&os, &og))
    {
        file.write(og.header, og.header_len);
        file.write(og.body, og.body_len);
    }

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);

    options.onProgress(100);

    return true;
}

} // end namespace sound.
} // end namespace de.

#if 0

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

#include <ogg/ogg.h>
#include <vorbis/vorbisenc.h>

enum class SampleType {
    Int16,
    Float32
};

struct Sound {
    std::vector<uint8_t> m_samples;
    int m_channels = 2;
    int m_sampleRate = 48000;
    size_t m_frames = 0;
    SampleType m_sampleType = SampleType::Int16;
};

// ------------------------------------------------------------
// PCM-Konvertierung: uint8_t → float[-1..1]
// ------------------------------------------------------------
static void convert_to_float(const Sound& snd, std::vector<float>& out)
{
    size_t frames = snd.m_frames;
    int ch = snd.m_channels;

    out.resize(frames * ch);

    if (snd.m_sampleType == SampleType::Int16)
    {
        const int16_t* src = reinterpret_cast<const int16_t*>(snd.m_samples.data());
        for (size_t i = 0; i < frames * ch; ++i)
            out[i] = float(src[i]) / 32768.0f;
    }
    else if (snd.m_sampleType == SampleType::Float32)
    {
        const float* src = reinterpret_cast<const float*>(snd.m_samples.data());
        std::memcpy(out.data(), src, frames * ch * sizeof(float));
    }
}

// ------------------------------------------------------------
// OGG/Vorbis Export
// ------------------------------------------------------------
bool export_ogg(const std::string& path, const Sound& snd)
{
    // PCM float vorbereiten
    std::vector<float> pcm;
    convert_to_float(snd, pcm);

    FILE* fp = std::fopen(path.c_str(), "wb");
    if (!fp) {
        std::fprintf(stderr, "Cannot open output file\n");
        return false;
    }

    // Vorbis Info
    vorbis_info vi;
    vorbis_info_init(&vi);

    // Qualitätsmodus: 0.4 = ~128 kbps stereo
    if (vorbis_encode_init_vbr(&vi, snd.m_channels, snd.m_sampleRate, 0.4f)) {
        std::fprintf(stderr, "vorbis_encode_init_vbr failed\n");
        return false;
    }

    vorbis_comment vc;
    vorbis_comment_init(&vc);
    vorbis_comment_add_tag(&vc, "ENCODER", "Benjamin Vorbis Export");

    vorbis_dsp_state vd;
    vorbis_block vb;

    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);

    ogg_stream_state os;
    ogg_stream_init(&os, 12345); // random serial

    // Header erzeugen
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);

    ogg_stream_packetin(&os, &header);
    ogg_stream_packetin(&os, &header_comm);
    ogg_stream_packetin(&os, &header_code);

    ogg_page og;
    while (ogg_stream_flush(&os, &og)) {
        fwrite(og.header, 1, og.header_len, fp);
        fwrite(og.body, 1, og.body_len, fp);
    }

    // PCM → Vorbis
    size_t frames = snd.m_frames;
    int ch = snd.m_channels;

    size_t pos = 0;
    while (pos < frames)
    {
        size_t chunk = std::min<size_t>(1024, frames - pos);

        float** buffer = vorbis_analysis_buffer(&vd, (int)chunk);

        for (size_t i = 0; i < chunk; ++i)
        {
            for (int c = 0; c < ch; ++c)
                buffer[c][i] = pcm[(pos + i) * ch + c];
        }

        pos += chunk;

        vorbis_analysis_wrote(&vd, (int)chunk);

        // Encoding
        while (vorbis_analysis_blockout(&vd, &vb) == 1)
        {
            vorbis_analysis(&vb, nullptr);
            vorbis_bitrate_addblock(&vb);

            ogg_packet op;
            while (vorbis_bitrate_flushpacket(&vd, &op))
            {
                ogg_stream_packetin(&os, &op);

                while (ogg_stream_pageout(&os, &og))
                {
                    fwrite(og.header, 1, og.header_len, fp);
                    fwrite(og.body, 1, og.body_len, fp);
                }
            }
        }
    }

    // Ende
    vorbis_analysis_wrote(&vd, 0);

    while (vorbis_analysis_blockout(&vd, &vb) == 1)
    {
        vorbis_analysis(&vb, nullptr);
        vorbis_bitrate_addblock(&vb);

        ogg_packet op;
        while (vorbis_bitrate_flushpacket(&vd, &op))
        {
            ogg_stream_packetin(&os, &op);

            while (ogg_stream_pageout(&os, &og))
            {
                fwrite(og.header, 1, og.header_len, fp);
                fwrite(og.body, 1, og.body_len, fp);
            }
        }
    }

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);

    fclose(fp);
    return true;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main()
{
    Sound snd;

    // Beispiel: 1 Sekunde 440 Hz Sinus
    snd.m_channels = 2;
    snd.m_sampleRate = 48000;
    snd.m_frames = 48000;
    snd.m_sampleType = SampleType::Float32;

    snd.m_samples.resize(snd.m_frames * snd.m_channels * sizeof(float));
    float* f = reinterpret_cast<float*>(snd.m_samples.data());

    for (size_t i = 0; i < snd.m_frames; ++i)
    {
        float s = sinf(2.0f * 3.14159265f * 440.0f * (float)i / snd.m_sampleRate);
        f[i*2+0] = s;
        f[i*2+1] = s;
    }

    export_ogg("test.ogg", snd);
    return 0;
}


namespace {

struct AutoCloser
{
    SNDFILE* m_file;

    explicit AutoCloser(SNDFILE* file)
    {
        m_file = file;
    }

    ~AutoCloser()
    {
        if (m_file)
        {
            sf_close(m_file);
            m_file = nullptr;
        }
    }
};

} // end namespace.

bool
save_sound_ogg_vorbis(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    auto ext = dbFileSuffix(uri);
    if (ext != "ogg")
    {
        DE_ERROR("This exporter only supports ogg files, ",uri)
        return false;
    }

    // if (sound.m_sampleType != SampleType::F32)
    // {
    //     DE_WARN("Only native support for ST_F32, ",uri)
    //     DE_WARN("Need converter, ",sound.str())
    //     return false;
    // }

    SF_INFO header;
    ::memset( &header, 0, sizeof(SF_INFO) );
    //header.frames = sf_count_t( sound.m_frames );
    header.channels = int( sound.m_channels );
    header.samplerate = int( sound.m_sampleRate );
    header.format = SF_FORMAT_OGG | SF_FORMAT_VORBIS; // SF_ENDIAN_CPU | | SF_FORMAT_FLOAT;
    //header.sections = 0;
    //header.seekable = 1;

    SNDFILE* file = sf_open( uri.c_str(), SFM_WRITE, &header );
    if ( !file )
    {
        DE_ERROR("Cannot sf_open, ",uri)
        DE_ERROR(sf_strerror(nullptr));
        return false;
    }

    AutoCloser autoCloser(file);

    options.onProgress(1);

    if (sound.m_sampleType == SampleType::F32)
    {
        auto p = reinterpret_cast<const float*>(sound.m_samples.data());
        int64_t frameWritten = sf_write_float(file, p, sound.m_frames);
    }
    else
    {
        Sound s2;
        if (!SoundUtil::convert(sound,s2,SampleType::F32))
        {
            DE_ERROR("Conversion failed. ", uri)
            return false;
        }
        options.onProgress(20);
        auto p = reinterpret_cast<const float*>(s2.m_samples.data());
        int64_t frameWritten = sf_write_float(file, p, s2.m_frames);
    }

    options.onProgress(100);

    return true;
}

#endif
