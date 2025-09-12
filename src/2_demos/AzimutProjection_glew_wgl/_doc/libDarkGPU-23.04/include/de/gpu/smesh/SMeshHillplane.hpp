#pragma once
#include <de/gpu/smesh/SMesh.hpp>

namespace de {
namespace gpu {

// =======================================================================
struct SMeshHillplane
// =======================================================================
{
   DE_CREATE_LOGGER("de.gpu.smesh.Hillplane")

   static void
   add( SMeshBuffer & o, Image const & heightMap, glm::vec3 siz = glm::vec3(1000.0, 200.0, 1000.0 ) );

   static void
   add( SMeshBuffer & o, std::string uri, glm::vec3 siz = glm::vec3(1000.0, 200.0, 1000.0 ) )
   {
      Image heightMap;
      dbLoadImage( heightMap, uri );
      add( o, heightMap, siz );
   }
};

} // end namespace gpu.
} // end namespace de.
