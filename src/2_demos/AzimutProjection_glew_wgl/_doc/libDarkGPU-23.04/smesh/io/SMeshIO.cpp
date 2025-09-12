#include "VideoDriver.hpp"
#include "SMeshIO.hpp"
#include "SMeshIO_3DS.hpp"
#include "SMeshIO_XML.hpp"
#include "SMeshIO_OFF.hpp"
#include "SMeshIO_OBJ.hpp"
#include "SMeshIO_STL.hpp"
#include "SMeshIO_ASSIMP.hpp"

namespace de {
namespace gpu {


// ===========================================================================
bool SMeshIO::load( SMesh & mesh,
                    std::string uri,
                    VideoDriver* driver,
                    SMeshOptions const & options )
// ===========================================================================
{
   PerformanceTimer timer;
   timer.start();
   // BEGIN
   bool ok = false;
   std::string ext = dbGetFileSuffix( uri );
   if ( ext == "xml" )
   {
      ok = SMeshXML::load( mesh, uri, driver );
   }
   else if ( ext == "3ds" )
   {
      ok = SMeshLoad3DS::load( mesh, uri, driver );
   }
   else
   {
      ok = SMeshLoadASSIMP::load( mesh, uri, driver, options );
   }

   if ( !ok )
   {
      DE_ERROR("Unsupported 3d model file! ext(", ext, "), uri(", uri,")")
      return false;
   }

   if ( options.flipYZ )
   {
      SMeshTool::flipYZ( mesh );
   }

   if ( options.centerVertices )
   {
      SMeshTool::centerVertices( mesh );
   }

   if ( options.scaleToMaxSize )
   {
      auto size = mesh.getBoundingBox().getSize();
      auto max_size = std::max( std::max( size.x, size.y ), size.z );
      auto scale = options.maxSize / max_size;
      SMeshTool::scaleVertices( mesh, scale, scale, scale );
   }

   if ( options.computeNormals )
   {
      SMeshTool::recalculateNormals( mesh );
   }

   // DE_DEBUG("Loading mesh ", uri )
   //   if ( options.debug )
   //   {
   //      mesh.recalculateBoundingBox();
   //      DE_DEBUG("Needed ms(",timer.ms(),"), ", mesh.toString() )
   //   }

   // END
   timer.stop();
   DE_DEBUG("Needed ms(",timer.ms(),"), ", mesh.toString() )
   return ok;
}


// ===========================================================================
bool SMeshIO::load( SMeshBuffer & mesh,
                    std::string uri,
                    VideoDriver* driver,
                    SMeshOptions const & options )
// ===========================================================================
{
   //DE_DEBUG("Loading meshbuffer ", uri )
   std::string ext = dbGetFileSuffix( uri );
   bool ok = false;
   if ( ext == "off" )
   {
      ok = SMeshBufferOFF::load( mesh, uri );
   }
   else if ( ext == "stl" )
   {
      ok = SMeshBufferSTL::load( mesh, uri );
   }
//   else if ( ext == "obj" )
//   {
//      return SMeshBufferLoadOBJ::load( mesh, uri );
//   }
//   else if ( ext == "off" )
//   {
//      return SMeshBufferLoadOFF::load( mesh, uri );
//   }
   if ( !ok )
   {
      DE_ERROR("Unsupported extension ", ext, ", uri = ", uri)
   }
   else
   {
      //mesh.recalculateBoundingBox();
   }
   return ok;
}


// ===========================================================================
bool SMeshIO::save( SMesh const & mesh,
                    std::string uri,
                    VideoDriver* driver,
                    SMeshOptions const & options )
// ===========================================================================
{
   // DE_DEBUG("Save SMesh ", uri )

   PerformanceTimer timer;
   timer.start();

   std::string ext = dbGetFileSuffix( uri );
   bool ok = false;
   if ( ext == "obj" )
   {
      ok = SMeshOBJ::save( mesh, uri, driver );
   }
   else if ( ext == "xml" )
   {
      ok = SMeshXML::save( mesh, uri, driver );
   }

//   else if ( ext == "off" )
//   {
//      return SMeshOFF::save( mesh, uri );
//   }
//   else if ( ext == "stl" )
//   {
//      return SMeshSTL::save( mesh, uri );
//   }

   if ( !ok )
   {
      DE_ERROR("Unsupported 3d model file save ext(", ext, "), uri(", uri,")")
      return false;
   }

   timer.stop();
   DE_DEBUG("Needed ms(",timer.ms(),"), mesh(", mesh.toString(),")" )
   return true;
}

// ===========================================================================
bool SMeshIO::save( SMeshBuffer const & mesh,
                    std::string uri,
                    VideoDriver* driver,
                    SMeshOptions const & options )
// ===========================================================================
{
   //DE_DEBUG("Saving meshbuffer ", uri )

   std::string ext = dbGetFileSuffix( uri );

   if ( ext == "off" )
   {
      return SMeshBufferOFF::save( mesh, uri );
   }
   else if ( ext == "stl" )
   {
      return SMeshBufferSTL::save( mesh, uri );
   }
   else if ( ext == "xml" )
   {
      return SMeshXML::save( mesh, uri, driver );
   }
   else if ( ext == "obj" )
   {
      return SMeshOBJ::save( mesh, uri, driver );
   }
   else
   {
      DE_ERROR("Unsupported extension ", ext, ", uri = ", uri)
      return false;
   }
}




} // end namespace
} // end namespace

