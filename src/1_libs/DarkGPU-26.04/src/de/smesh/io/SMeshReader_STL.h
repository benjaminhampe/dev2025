#pragma once
#include <de/smesh/SMeshIO.h>

namespace de {
namespace smesh {

struct SMeshBufferSTL
{
   static bool
   load( SMeshBuffer const & mesh, std::string uri, int flags = 0 );
   
   static std::string
   writeVec3f( glm::vec3 const & pos );
   static std::string
   writeTriangle3f( glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c );
   static bool
   writeMeshASCII( SMeshBuffer const & mesh, std::string uri, int flags );
   static bool
   save( SMeshBuffer const & mesh, std::string uri, int flags = 0 );
};

} // end namespace smesh.
} // end namespace de.


