#pragma once
#include "SMesh.hpp"
#include "SMeshIO_MTL.hpp"

namespace de {
namespace gpu {

class VideoDriver;

struct SMeshOBJ
{
   DE_CREATE_LOGGER("de.gpu.SMeshOBJ")

   static bool
   save( SMeshBuffer const & buf, std::string const & objUri, VideoDriver* driver );
   static bool
   save( SMesh const & mesh, std::string const & objUri, VideoDriver* driver );

   static std::string
   toString( glm::vec3 const & pos );
   static std::string
   toString( glm::vec2 const & pos );
   static std::string
   toString( uint32_t color, std::string prefix = "" );

};

} // end namespace
} // end namespace
