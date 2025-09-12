#pragma once
#include "SMesh.hpp"

namespace de {
namespace gpu {

struct VideoDriver;

struct SMeshXML
{
   DE_CREATE_LOGGER("de.gpu.SMeshXML")
   static bool
   writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* bufNode, SMeshBuffer const & buf, VideoDriver* driver );
   static bool
   save( SMesh const & mesh, std::string uri, VideoDriver* driver );
   static bool
   save( SMeshBuffer const & buf, std::string uri, VideoDriver* driver );
   static bool
   load( SMesh & mesh, std::string uri, VideoDriver* driver );
};

} // end namespace
} // end namespace
