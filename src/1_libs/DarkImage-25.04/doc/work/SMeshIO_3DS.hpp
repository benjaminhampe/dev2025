#pragma once
#include "SMesh.hpp"

namespace de {
namespace gpu {

struct VideoDriver;

struct SMeshLoad3DS
{
   DE_CREATE_LOGGER("de.gpu.SMeshLoad3DS")

   // inline
   static bool isSupportedFileExtension( std::string const & ext )
   { return ext == "3ds"; }

   // inline
   static bool load( SMesh & mesh, std::string uri, VideoDriver* driver )
   {
      Binary file( uri );
      if ( !file.is_open() )
      {
         DE_ERROR("Cant open ",uri)
         return false;
      }
      return load( mesh, file, driver );
   }

   // impl in .cpp
   static bool load( SMesh & mesh, Binary & file, VideoDriver* driver );

};

} // end namespace gpu
} // end namespace de
