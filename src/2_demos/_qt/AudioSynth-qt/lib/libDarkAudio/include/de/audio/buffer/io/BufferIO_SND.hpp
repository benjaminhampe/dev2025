#ifndef DE_AUDIO_SAMPLEBUFFER_SNDFILE_HPP
#define DE_AUDIO_SAMPLEBUFFER_SNDFILE_HPP

#include <de/audio/buffer/Buffer.hpp>

#ifdef USE_LIBSNDFILE

namespace de {
namespace audio {
namespace io {

// Converts any src buffer to signed 16 bit integer ( WAV PCM ) in range [-32768,32767]
struct BufferSND
{
   DE_CREATE_LOGGER("de.audio.BufferSND")

   static bool
   load( Buffer & dst, std::string uri );

   static bool
   save( Buffer const & src, std::string uri );
};

} // end namespace io
} // end namespace audio
} // end namespace de

#endif // USE_LIBSNDFILE

#endif










