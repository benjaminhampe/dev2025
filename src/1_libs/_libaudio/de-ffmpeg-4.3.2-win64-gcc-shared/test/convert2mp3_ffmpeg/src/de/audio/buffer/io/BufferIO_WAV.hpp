#pragma once
#include <de/audio/buffer/Buffer.hpp>

namespace de {
namespace audio {
namespace io {

struct BufferWAV
{
   DE_CREATE_LOGGER("de.audio.BufferWAV")
   static bool
   load( Buffer & dst, std::string uri );

   static bool
   save( Buffer const & src, std::string uri );
};

} // end namespace io
} // end namespace audio
} // end namespace de


