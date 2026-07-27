#include <de/sound/SND/SoundReader_SND.h>
#include <de/sound/SND/de_libsndfile.h>

namespace de {
namespace sound {

bool load_sound_snd_f32(Sound & sound, const std::string & uri )
{
    auto ext = dbFileSuffix( uri );
    int fmt = Utils::getSndTypeFromFileExt( ext );
    if (fmt < 0)
    {
        DE_ERROR("Unsupported SNDFILE type, uri = ", uri)
        return false;
    }

    SF_INFO info{};
    info.format = 0;

    SNDFILE* file = sf_open(uri.c_str(), SFM_READ, &info);
    if ( !file )
    {
        DE_ERROR("Cant open uri ", uri )
        return false;
    }

    DE_DEBUG("SNDFILE "
             "Seekable(",info.seekable,"), "
             "Sections(",info.sections,"), "
             "Uri(", uri,")")
    DE_DEBUG("SNDFILE "
             "Fmt(",info.format,"), "
             "Type(",Utils::getSndTypeStr(info.format),"), "
             "SampleType(",Utils::getSndSampleTypeStr(info.format),"), "
             "Endian(",Utils::getSndEndianStr(info.format),")")
    DE_DEBUG("SNDFILE "
             "SampleRate(",info.samplerate,"), "
             "Channels(",info.channels,"), "
             "Frames(",info.frames,")")

    sound.m_sampleType = SampleType::F32;
    sound.m_sampleRate = info.samplerate;
    sound.m_frames = info.frames;
    sound.m_channels = info.channels;
    sound.m_samples.resize(sound.m_frames * sound.m_channels * sizeof(float));

    float* __restrict__ dst = reinterpret_cast<float*>(sound.m_samples.data());

    sf_count_t gotten = sf_readf_float(file, dst, sound.m_frames);
    sf_close(file);

    if (gotten != sound.m_frames)
    {
        DE_ERROR("Gotten(",gotten,") != sound.m_frameCount(",sound.m_frames,")")
    }

    return true;
}

} // end namespace sound.
} // end namespace de.


#if 0


bool load_sound_snd_f32(Sound & sound, const std::string & uri )
{
    // Check file-extension
    auto ext = dbFileSuffix( uri );
    int sndFmt = Utils::getSndFormatFromFileExt( ext );
    if (sndFmt == -1)
    {
        DE_ERROR("Unsupported audio type for uri ", uri )
        return false;
    }

    SF_INFO sndInfo = {0};
    sndInfo.format = 0;

    SNDFILE* file = sf_open( uri.c_str(), SFM_READ, &sndInfo );
    if ( !file )
    {
        DE_ERROR("Cant open uri ", uri )
        return false;
    }

    DE_DEBUG("Loading uri ", uri )
    DE_DEBUG("Format = ",sndInfo.format );
    DE_DEBUG("Channels = ",sndInfo.channels );
    DE_DEBUG("Frames = ",sndInfo.frames );
    DE_DEBUG("SampleRate = ",sndInfo.samplerate );
    DE_DEBUG("Sections = ",sndInfo.sections );
    DE_DEBUG("Seekable = ",sndInfo.seekable );

    // Prepare: DestinationBuffer
    dst.setFormat( ST_F32I, sndInfo.channels, sndInfo.samplerate );
    dst.resize( uint64_t(sndInfo.frames) );
    auto dstPtr = reinterpret_cast< float* >( dst.data() );

    // Prepare: OneFrameBuffer ( intermediate between File and Dst )
    std::vector< uint8_t > frameBuffer( sizeof( float ) * sndInfo.channels, 0x00 );
    float* src = reinterpret_cast< float* >( frameBuffer.data() );

    // Read frame per frame ( a frame can have multiple channels )
    for ( size_t i = 0; i < sndInfo.frames; ++i )
    {
        // Write File -> FrameBuffer
        sf_seek( file, i, SEEK_CUR );
        sf_read_float( file, src, sf_count_t(1) );

        // Write FrameBuffer -> DestinationBuffer ( multiple channels )
        auto srcPtr = src;
        for ( size_t c = 0; c < sndInfo.channels; ++c )
        {
            *dstPtr = *srcPtr;
            srcPtr++;
            dstPtr++;
        }
    }

    sf_close( file );
    return true;
}

#endif
