#pragma once
#include <de/audio/buffer/Buffer.hpp>

namespace de {
namespace audio {

struct BufferIO
{
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
