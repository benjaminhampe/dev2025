#ifndef DE_AUDIO_SAMPLEBUFFER_IO_COLLECTION_HPP
#define DE_AUDIO_SAMPLEBUFFER_IO_COLLECTION_HPP

#include <de/audio/buffer/Buffer.hpp>

namespace de {
namespace audio {

struct BufferIO
{
   DE_CREATE_LOGGER("de.audio.BufferIO")

   static bool
   load(
      Buffer & dst,
      std::string uri,
      int streamIndex = -1, // -1 = AutoSelect best audio stream, 0..N-1 select specific stream ( could be video stream ? )
      ESampleType convertType = ST_Unknown,
      int sampleRate = 0 );

   static bool
   save( Buffer const & src, std::string uri );
};

} // end namespace audio
} // end namespace de

#endif










