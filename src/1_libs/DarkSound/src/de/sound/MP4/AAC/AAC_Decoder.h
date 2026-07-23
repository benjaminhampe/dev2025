#pragma once
#include <DarkMP4.h>
#include <de/sound/Sound.h>

// #include <vector>
// #include <cmath>

// ---------- FAAD2 ----------
extern "C"
{
    #include <neaacdec.h>
    #include <faad.h>
}

namespace de {
namespace sound {


class AAC_Decoder
{
public:
    AAC_Decoder()
        : m_handle(nullptr)
    {
        open();
    }

    ~AAC_Decoder()
    {
        close();
    }

    bool readFileInfo(Sound& sound,
                      const file::mp4::aac::AscInfo& asc,
                      uint64_t sample_count)
    {
        if (!reinit(asc))
        {
            DE_ERROR("")
            return false;
        }

        unsigned long sampleRate = 0;
        unsigned char channels   = 0;

        // NeAACDecInit2 wurde in reinit() schon aufgerufen,
        // sampleRate/channels sind dort gesetzt worden.
        sampleRate = m_sampleRate;
        channels   = m_channels;

        if (sampleRate == 0 || channels == 0)
        {
            DE_ERROR("Invalid sampleRate/channels from FAAD2: ",
                     sampleRate, " / ", int(channels));
            return false;
        }

        // sample_count = Anzahl AAC-Frames
        // 1 AAC-Frame = 1024 PCM-Samples pro Kanal (AAC-LC)
        uint64_t totalPcmSamples = sample_count * 1024ull;

        sound.m_sampleRate   = static_cast<uint32_t>(sampleRate);
        sound.m_channelCount = static_cast<uint32_t>(channels);
        sound.m_sampleType   = Sound::ST_F32;
        sound.m_frameCount   = totalPcmSamples; // PCM-Samples (interleaved)
        sound.m_duration     = double(totalPcmSamples) *
                                1'000'000'000.0 /
                                double(sampleRate);

        return true;
    }

    bool decodeAAC(
        File& file,
        const file::mp4::aac::AscInfo& asc,
        const file::mp4::MP4_SampleInfoTable& table,
        const std::function<void(const float*, size_t, uint32_t /*channels*/)> & pcmCallback
    )
    {
        if (!reinit(asc))
        {
            DE_ERROR("")
            return false;
        }

        std::vector<uint8_t> tmp;

        for (size_t i = 0; i < table.size(); ++i)
        {
            const auto& s = table[i];

            if (s.size == 0)
                continue;

            if (!file.seek(static_cast<int64_t>(s.offset)))
            {
                DE_ERROR("seek failed at sample ", i, " offset=", s.offset);
                return false;
            }

            tmp.resize(s.size);
            size_t bytesRead = file.read(tmp.data(), s.size);
            if (bytesRead != s.size)
            {
                DE_ERROR("read failed at sample ", i,
                         " expected=", s.size, " got=", bytesRead);
                return false;
            }

            NeAACDecFrameInfo frameInfo{};
            void* pcm = NeAACDecDecode(
                m_handle,
                &frameInfo,
                tmp.data(),
                static_cast<unsigned long>(tmp.size())
            );

            if (frameInfo.error != 0)
            {
                DE_ERROR("FAAD2 decode error at sample ", i,
                         " err=", frameInfo.error);
                continue; // weiter versuchen
            }

            if (!pcm || frameInfo.samples == 0)
                continue;

            // Wir haben FAAD2 auf float-Ausgabe konfiguriert.
            float* pcmF = reinterpret_cast<float*>(pcm);
            size_t samplesOut = frameInfo.samples; // total (interleaved)
            uint32_t ch = frameInfo.channels;

            if (ch == 0)
                ch = m_channels; // Fallback

            pcmCallback(pcmF, samplesOut, ch);
        }

        return true;
    }

private:
    NeAACDecHandle m_handle;
    unsigned long  m_sampleRate = 0;
    unsigned char  m_channels   = 0;

    void open()
    {
        if (!m_handle)
        {
            m_handle = NeAACDecOpen();
            if (!m_handle)
            {
                DE_ERROR("NeAACDecOpen failed");
                return;
            }

            // Auf float-Ausgabe konfigurieren
            NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(m_handle);
            if (conf)
            {
                conf->outputFormat = FAAD_FMT_FLOAT;
                NeAACDecSetConfiguration(m_handle, conf);
            }
        }
    }

    void close()
    {
        if (m_handle)
        {
            NeAACDecClose(m_handle);
            m_handle = nullptr;
        }
        m_sampleRate = 0;
        m_channels   = 0;
    }

    bool reinit(const file::mp4::aac::AscInfo& asc)
    {
        // Für jede Datei/ASC neu initialisieren
        if (!m_handle)
            open();

        if (!m_handle)
            return false;

        m_sampleRate = 0;
        m_channels   = 0;

        long init = NeAACDecInit2(
            m_handle,
            (unsigned char*)asc.raw.data(),
            (unsigned long)asc.raw.size(),
            &m_sampleRate,
            &m_channels
        );

        if (init < 0 || m_sampleRate == 0 || m_channels == 0)
        {
            DE_ERROR("NeAACDecInit2 failed: ret=", init,
                     " sr=", m_sampleRate,
                     " ch=", int(m_channels));
            return false;
        }

        return true;
    }
};


#if 0
class AAC_Decoder
{
public:
    NeAACDecHandle m_handle;

    AAC_Decoder()
        : m_handle(nullptr)
    {
        m_handle = NeAACDecOpen();
    }

    ~AAC_Decoder()
    {
        if (m_handle)
        NeAACDecClose(m_handle);
        m_handle = nullptr;
    }

    bool readFileInfo(FileInfo& fileInfo,
            const file::mp4::aac::AscInfo & asc,
                       u64 sample_count)
    {
        DE_BENNI("sample_count = ",sample_count)
        DE_BENNI("asc.raw.data() = ",(void*)asc.raw.data())
        DE_BENNI("asc.raw.size() = ",asc.raw.size())
        DE_BENNI("m_handle = ",m_handle)

        if (!m_handle)
        {
            DE_ERROR("No FAAD2 handle")
            return false;
        }

        unsigned long sampleRate;
        unsigned char channels;
        NeAACDecInit2(m_handle,
                      (unsigned char*)asc.raw.data(),
                      (unsigned long)asc.raw.size(),
                      &sampleRate,
                      &channels);

        DE_BENNI("sampleRate = ",sampleRate)
        DE_BENNI("channels = ",int(channels))

        // AAC frame count = sample_count * 1024 (usually)
        uint64_t totalFrames = uint64_t(sample_count) * 1024;

        DE_BENNI("totalFrames = ",totalFrames)

        fileInfo.sampleRate   = u32(sampleRate);
        fileInfo.channelCount = u32(channels);
        fileInfo.sampleType   = FileInfo::ST_F32;
        fileInfo.frameCount   = totalFrames;
        fileInfo.duration     = double(totalFrames) *
                                double(1'000'000'000ull) / double(sampleRate);

        DE_BENNI("fileInfo = ", fileInfo.str())
        return true;
    }

    bool decodeAAC(
        File & file,
        const file::mp4::aac::AscInfo & asc,
        const file::mp4::MP4_SampleInfoTable& table,
        std::function<void(const std::vector<float>&)> pcmCallback
    )
    {
        if (!m_handle)
        {
            DE_ERROR("No FAAD2 handle")
            return false;
        }

        std::vector<uint8_t> tmp;

        std::vector<float> pcmDecoded;

        for (size_t i = 0; i < table.size(); ++i)
        {
            const file::mp4::MP4_SampleInfo& s = table[i];

            int64_t fileOffset = s.offset;
            int64_t frameSize = s.size;

            tmp.resize(frameSize);
            file.seek(fileOffset);
            file.read(tmp.data(),frameSize);

            NeAACDecFrameInfo frameInfo;
            void* pcm = NeAACDecDecode(m_handle,
                            &frameInfo,
                            tmp.data(),
                            tmp.size());

            if (frameInfo.error != 0)
            {
                DE_ERROR("[",i,"] info.error = ",frameInfo.error)
                continue;
            }

            // PCM ist float* oder int16*, abhängig von FAAD2-Config
            float* pcmF = reinterpret_cast<float*>(pcm);
            size_t samplesOut = frameInfo.samples;

            pcmDecoded.resize(samplesOut);
            for (size_t i = 0; i < pcmDecoded.size(); ++i)
            {
                pcmDecoded[i] = pcmF[i];
            }

            pcmCallback(pcmDecoded);
        }

        return true;
    }

/*
    void decodeInto(File & file,
                    const file::mp4::aac::AscInfo & asc,
                    const file::mp4::MP4_SampleInfoTable& table)
    {
        m_handle = NeAACDecOpen();
        NeAACDecConfigurationPtr cfg = NeAACDecGetCurrentConfiguration(m_handle);
        cfg->outputFormat = FAAD_FMT_FLOAT;
        NeAACDecSetConfiguration(m_handle, cfg);

        unsigned long sampleRate;
        unsigned char channels;
        NeAACDecInit2(m_handle,
                      (unsigned char*)asc.raw.data(),
                      (unsigned long)asc.raw.size(),
                      &sampleRate,
                      &channels);

        std::vector<uint8_t> frameBuf;
        uint64_t pos = 0;

        for (unsigned i = 0; i < tr->sample_count; ++i)
        {
            unsigned fb = 0;
            unsigned ts = 0;
            unsigned dur = 0;
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
*/
    static bool isAac(unsigned oti)
    {
        return oti == 0x40 || oti == 0x66 || oti == 0x67 || oti == 0x68;
    }

    static int read_cb(int64_t o, void* b, size_t s, void* t)
    {
        File* file = (File*)t;
        file->seek(o);
        return file->read(b, s);
    }
};

#endif

} // end namespace sound.
} // end namespace de.






/*

class AAC_Decoder
{
public:
    unsigned long sampleRate = 0;
    unsigned char channels = 0;
    uint64_t totalFrames = 0;

    AAC_Decoder()
    {}

    void readFileInfo(FileInfo& info)
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

*/
