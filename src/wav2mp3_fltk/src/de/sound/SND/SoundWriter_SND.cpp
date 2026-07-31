#include <de/sound/SND/SoundWriter_SND.h>

#include <de/sound/SoundUtil.h>

#include <de/sound/SND/de_libsndfile.h>

namespace de {
namespace sound {

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

/*
+----------------+------+------+------+------+------+------+------+------+
| Format         | U8   | S8   | S16  | S24  | S32  | F32  | F64  | Notes |
+----------------+------+------+------+------+------+------+------+------+
| WAV            | yes  | yes  | yes  | yes  | yes  | yes  | yes  | PCM + float
| AIFF           | yes  | yes  | yes  | yes  | yes  | yes  | yes  | PCM + float
| AU (Sun/NeXT)  | yes  | yes  | yes  | yes  | yes  | yes  | yes  | PCM + float
| RAW            | yes  | yes  | yes  | yes  | yes  | yes  | yes  | depends on subtype
| PAF            | no   | no   | yes  | yes  | yes  | no   | no   | integer PCM only
| SVX (IFF 8SVX) | no   | yes  | no   | no   | no   | no   | no   | S8 only
| NIST/Sphere    | no   | no   | yes  | no   | no   | no   | no   | S16 only
| VOC            | yes  | yes  | yes  | no   | no   | no   | no   | U8/S8/S16
| IRCAM          | no   | no   | yes  | no   | no   | yes  | yes  | S16/F32/F64
| W64            | yes  | yes  | yes  | yes  | yes  | yes  | yes  | PCM + float
| MAT4           | no   | no   | no   | no   | no   | yes  | no   | F32 only
| MAT5           | no   | no   | no   | no   | no   | yes  | yes  | F32/F64
| PVF            | no   | no   | yes  | yes  | yes  | no   | no   | integer PCM
| XI             | no   | no   | yes  | no   | no   | no   | no   | S16 only
| HTK            | no   | no   | yes  | no   | no   | no   | no   | S16 only
| SDS            | no   | no   | yes  | no   | no   | no   | no   | S16 only
| AVR            | no   | no   | yes  | no   | no   | no   | no   | S16 only
| CAF            | yes  | yes  | yes  | yes  | yes  | yes  | yes  | PCM + float
| FLAC           | no   | no   | yes  | yes  | yes  | no   | no   | integer PCM only
| OGG/Vorbis     | no   | no   | no   | no   | no   | yes  | no   | F32 only
| OPUS           | no   | no   | no   | no   | no   | yes  | no   | F32 only (libsndfile encodes Opus)
+----------------+------+------+------+------+------+------+------+------+
*/

bool
save_sound_sndfile_ogg_vorbis(
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

bool
save_sound_sndfile_wav(
    const Sound& sound,
    const std::string& uri,
    const SoundSaveOptions& options)
{
    auto ext = dbFileSuffix(uri);
    if (ext != "wav")
    {
        DE_ERROR("This exporter only supports wav files, ",uri)
        return false;
    }

    // if (sound.m_sampleType != SampleType::S16)
    // {
    //     DE_ERROR("This exporter only supports SampleType_S16, ",uri)
    //     return false;
    // }

    //int sfType = Utils::getSndTypeFromFileExt(ext);
    int sfSampleType = Utils::getSndSampleType(sound.m_sampleType);
    //int sfEndian = SF_ENDIAN_CPU;

    SF_INFO header;
    ::memset( &header, 0, sizeof(SF_INFO) );
    header.frames = sf_count_t( sound.m_frames );
    header.channels = int( sound.m_channels );
    header.samplerate = int( sound.m_sampleRate );
    //header.format = SF_ENDIAN_CPU | SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    header.format = SF_ENDIAN_CPU | SF_FORMAT_WAV | sfSampleType;
    header.sections = 0;
    header.seekable = 1;

    SNDFILE* file = sf_open( uri.c_str(), SFM_WRITE, &header );
    if ( !file )
    {
        DE_ERROR("Cant open wav 16bit stereo")
        return false;
    }

    if (sound.m_sampleType == SampleType::S8)
    {
        Sound sound2;
        SoundUtil::convert(sound,sound2,SampleType::U8);
        auto p = reinterpret_cast<const uint8_t*>(sound2.m_samples.data());
        sf_write_raw(file, p, sound2.sampleCount());
    }
    else if (sound.m_sampleType == SampleType::U8)
    {
        auto p = reinterpret_cast<const uint8_t*>(sound.m_samples.data());
        sf_write_raw(file, p, sound.sampleCount());
    }
    else if (sound.m_sampleType == SampleType::S16)
    {
        auto p = reinterpret_cast<const int16_t*>(sound.m_samples.data());
        sf_write_short(file, p, sound.sampleCount());
    }
    else if (sound.m_sampleType == SampleType::S24)
    {
        Sound sound2;
        SoundUtil::convert(sound,sound2,SampleType::F32);
        auto p = reinterpret_cast<const float*>(sound2.m_samples.data());
        sf_write_float(file, p, sound2.sampleCount());
    }
    // Close
    sf_close( file );
    return true;
}

} // end namespace sound.
} // end namespace de.

#if 0


bool
BufferSND::save( Buffer const & src, std::string uri )
{
   SF_INFO header;
   ::memset( &header, 0, sizeof(SF_INFO) );
   header.frames = sf_count_t( src.getFrameCount() );
   header.channels = int( src.getChannelCount() );
   header.samplerate = int( src.getSampleRate() );
   header.format = SF_ENDIAN_CPU | SF_FORMAT_WAV | SF_FORMAT_PCM_16;
   header.sections = 0;
   header.seekable = 1;

   SNDFILE* file = sf_open( uri.c_str(), SFM_WRITE, &header );
   if ( !file )
   {
      DE_ERROR("Cant open wav 16bit stereo")
      return false;
   }

   // Write samples (frames x 2 channels)
   for ( uint64_t i = 0; i < uint64_t( header.frames ); ++i )
   {
      //sf_seek( file, i, SEEK_CUR );
      int16_t L,R;
      src.getSample( i, 0, &L, ST_S16 ); // interleaved or planar not vip using getSample() getter.
      src.getSample( i, 1, &R, ST_S16 ); // interleaved or planar not vip using getSample() getter.
      sf_write_short( file, &L, sf_count_t( 1 ) );
      sf_write_short( file, &R, sf_count_t( 1 ) );
   }

   sf_close( file );
   return true;
}

#endif
