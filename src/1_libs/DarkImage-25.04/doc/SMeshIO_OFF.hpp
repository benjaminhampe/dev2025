#pragma once
#include "SMesh.hpp"

namespace de {
namespace gpu {

class VideoDriver;

struct SMeshBufferOFF
{
   DE_CREATE_LOGGER("de.gpu.SMeshBufferOFF")

   static bool
   load( SMeshBuffer & dst, std::string uri, int flags = 0 );

   static bool
   save( SMeshBuffer const & mesh, std::string uri );

   //EType 
   //getType() const {	return EMWT_OBJ; }

   static std::string
   toString( glm::vec3 const & pos );
   static std::string
   toString( glm::vec2 const & pos );
   static std::string
   toString( uint32_t color, std::string prefix = "" );

};

} // end namespace
} // end namespace
