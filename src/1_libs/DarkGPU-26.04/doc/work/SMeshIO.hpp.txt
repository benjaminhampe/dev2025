#pragma once
#include "SMesh.hpp"

namespace de {
namespace gpu {

struct VideoDriver;

// =======================================================================
struct SMeshOptions
// =======================================================================
{
   SMeshOptions()
      : debug(false)
      , computeNormals(false)
      , flipYZ( false )
      , centerVertices( false )
      , scaleToMaxSize( false )
      , maxSize( 100.0f )
   {}

   bool debug;
   bool computeNormals;
   bool flipYZ;
   bool centerVertices;
   bool scaleToMaxSize;
   float maxSize;
};

// =======================================================================
struct SMeshIO
// =======================================================================
{
   DE_CREATE_LOGGER("de.gpu.SMeshIO")

   static bool
   load( SMeshBuffer & mesh,
         std::string uri,
         VideoDriver* driver,
         SMeshOptions const & options = SMeshOptions() );

   static bool
   load( SMesh & mesh,
         std::string uri,
         VideoDriver* driver,
         SMeshOptions const & options = SMeshOptions() );

   static bool
   save( SMeshBuffer const & mesh,
         std::string uri,
         VideoDriver* driver,
         SMeshOptions const & options = SMeshOptions() );

   static bool
   save( SMesh const & mesh,
         std::string uri,
         VideoDriver* driver,
         SMeshOptions const & options = SMeshOptions() );


};

} // end namespace gpu
} // end namespace de

