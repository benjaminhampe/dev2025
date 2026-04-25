#pragma once
#include "SMesh.hpp"

namespace de {
namespace gpu {

class VideoDriver;

// FileReader:    .mtl 
//     - TnL standard material description file
//     - only makes sense for .obj model files.
struct MTL
{
   DE_CREATE_LOGGER("de.gpu.MTL")

   static bool save( std::vector< Material > const & liste, std::string uri );
   static bool load( std::vector< Material > & liste, std::string uri );
};

} // end namespace
} // end namespace
