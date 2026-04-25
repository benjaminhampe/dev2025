#include "SMeshIO_OFF.hpp"
namespace de {
namespace gpu {

// OFF header
// - First line (optional): OFF to mark the file type.
// - Second line: the number of vertices, number of faces, and number of edges, in order or 0.
// - List of vertices: X, Y and Z coordinates.
// - List of faces: # vertices, followed by the face indices, in order (indexed from zero).
// - Optionally, the RGB values for the face color can follow the elements of the faces.
// OFF
// # cube.off
// # A cube
// 8 6 12
// 1.0 0.0 1.4142
// 0.0 1.0 1.4142
// -1.0 0.0 1.4142
// 0.0 -1.0 1.4142
// 1.0 0.0 0.0
// 0.0 1.0 0.0
// -1.0 0.0 0.0
// 0.0 -1.0 0.0

// 4 0 1 2 3 255 0 0 #red
// 4 7 4 0 3 0 255 0 #green
// 4 4 5 1 0 0 0 255 #blue
// 4 5 6 2 1 0 255 0
// 4 3 2 6 7 0 0 255
// 4 6 5 4 7 255 0 0

//EType getType() const {	return EMWT_OBJ; }

//   OFF
//   1900 3796 0
//   -0.25829 -4.29578 4.02524
//   -0.3358 -4.53536 -3.84291
//   -7.57239 -3.38175 4.92635
//   -8.24296 -3.47912 -4.28423
//   -10.9128 0.101619 0.0623689
//   -3.95774 -0.0574358 6.92651
//   2.9146 0.0533714 0.24182
//   3 0 1025 74
//   3 0 74 1193
//   3 0 418 994
//   3 0 1193 418
//   3 0 994 540
//   3 0 540 1092
//   3 0 1092 1025
//   3 1 18 779


// static
std::string
SMeshBufferOFF::toString( glm::vec3 const & pos )
{
   std::ostringstream s;
   s << -pos.x << " " << pos.y << " " << pos.z << "\n";
   return s.str();
}

// static
std::string
SMeshBufferOFF::toString( glm::vec2 const & pos )
{
   std::ostringstream s;
   s << pos.x << " " << -pos.y << "\n";
   return s.str();
}

// static
std::string
SMeshBufferOFF::toString( uint32_t color, std::string prefix )
{
   std::ostringstream s; s << prefix << " "
   << float( RGBA_R(color) )/255.f << " "
   << float( RGBA_G(color) )/255.f << " "
   << float( RGBA_B(color) )/255.f << "\n";
   return s.str();
}

// static
bool
SMeshBufferOFF::load( SMeshBuffer & dst, std::string uri, int flags )
{
   std::vector< std::string > lines = dbLoadTextLn( uri );

   if ( lines.size() < 4 )
   {
      DE_ERROR("Malformed 'off' model file ",uri)
      return false;
   }

   if ( lines[ 0 ] == "OFF" || lines[ 0 ] == "off" )
   {
      DE_ERROR("Invalid header 'off' model file ",uri)
      return false;
   }

   std::vector< std::string > header = dbStrSplit( lines[ 1 ], ' ' );
   if ( header.size() < 3 )
   {
      DE_ERROR("Invalid header size (",header.size(),")'off' model file ",uri)
      return false;
   }

   uint64_t vertexCount = atoll( header[ 0 ].c_str() );
   uint64_t faceCount = atoll( header[ 1 ].c_str() );
   uint64_t edgeCount = atoll( header[ 2 ].c_str() );

   DE_DEBUG("Loading OFF mesh = ", uri )
   DE_DEBUG("vertexCount = ", vertexCount )
   DE_DEBUG("faceCount = ", faceCount )
   DE_DEBUG("edgeCount = ", edgeCount )

   dst.Vertices.clear();
   dst.Vertices.reserve( vertexCount );

   dst.Indices.clear();
   dst.Indices.reserve( vertexCount );

   std::vector< std::string > splitted;

   // Read vertices
   for ( size_t i = 0; i < vertexCount; ++i )
   {
      size_t k = 2 + i;
      if ( k > lines.size() )
      {
         DE_ERROR("Invalid line ",k, " of ",lines.size(), ", vertexCount(",vertexCount,")" )
         continue;
      }

      auto const & line = lines[ k ];
      splitted = dbStrSplit( line, ' ' );
      if ( splitted.size() < 3 )
      {
         DE_ERROR("Not a vertex at line ", k )
         continue;
      }

      float x = atof( splitted[ 0 ].c_str() );
      float y = atof( splitted[ 1 ].c_str() );
      float z = atof( splitted[ 2 ].c_str() );
      dst.addVertex( S3DVertex( x, y, z, 0,0,-1, 0xFFFFFFFF, 0,0 ) );
   }

   // Read faces
   for ( size_t i = 0; i < faceCount; ++i )
   {
      size_t k = 2 + vertexCount + i;
      if ( k > lines.size() )
      {
         DE_ERROR("Invalid line ",k, " of ",lines.size(), ", vertexCount(",vertexCount,")" )
         continue;
      }

      splitted = dbStrSplit( lines[ k ], ' ' );
      if ( splitted.size() < 2 )
      {
         DE_ERROR("Not a face at line ", k )
         continue;
      }

      int faceType = atol( splitted[0].c_str() );
      if ( faceType == 1 )
      {
         uint32_t a = atoll( splitted[1].c_str() );
         dst.addIndex( a );
      }
      else if ( faceType == 2 )
      {
         if ( splitted.size() < 3 )
         {
            DE_ERROR("Not a line face at line ", k )
         }
         else
         {
            uint32_t a = atoll( splitted[1].c_str() );
            uint32_t b = atoll( splitted[2].c_str() );
            dst.addIndexedLine( a,b );
         }
      }
      else if ( faceType == 3 )
      {
         if ( splitted.size() < 4 )
         {
            DE_ERROR("Not a triangle face at line ", k )
         }
         else
         {
            uint32_t a = atoll( splitted[1].c_str() );
            uint32_t b = atoll( splitted[2].c_str() );
            uint32_t c = atoll( splitted[3].c_str() );
            dst.addIndexedTriangle( a,b,c );
         }
      }
      else // if ( faceType ==  )
      {
         DE_ERROR("Not supported face type(",faceType,") at line ", k )

      }
   }

   dst.recalculateBoundingBox();
   return true;
}



// static
bool
SMeshBufferOFF::save( SMeshBuffer const & mesh, std::string uri )
{
   uint32_t faceCount = mesh.getFaceCount();
   uint32_t edgeCount = mesh.getEdgeCount();
   uint32_t mbv = mesh.getVertexCount();
   uint32_t mbi = mesh.getIndexCount();

   std::ostringstream s; s << "OFF\n"
//      "# SMeshBuffer.Name = " << mesh.getName() << "\n"
//      "# SMeshBuffer.BBox.Min = " << mesh.getBoundingBox().getMin() << "\n"
//      "# SMeshBuffer.BBox.Max = " << mesh.getBoundingBox().getMax() << "\n"
//      "# SMeshBuffer.PrimitiveType = " << mesh.getPrimitiveTypeStr() << "\n"
//      "# SMeshBuffer.VertexCount = " << mesh.getVertexCount() << "\n"
//      "# SMeshBuffer.IndexCount = " << mesh.getIndexCount() << "\n"
//      "# SMeshBuffer.IndexType = " << mesh.getIndexTypeStr() << "\n"
   << mbv << " " << faceCount << " " << edgeCount << "\n";

   //s << "# Vertices(" << mbv << ")\n";
   for ( uint32_t v = 0; v < mbv; ++v )
   {
      s << mesh.getVertex( v ).getPos().x << " "
        << mesh.getVertex( v ).getPos().y << " "
        << mesh.getVertex( v ).getPos().z << "\n";
   }

   //s << "# Faces(" << faceCount << ")\n";
   if ( mesh.getPrimitiveType() == PrimitiveType::Points )
   {
      for ( uint32_t i = 0; i < mbi; ++i )
      {
         s << "1 " << mesh.getIndex( i ) << "\n";
      }
   }
   else if ( mesh.getPrimitiveType() == PrimitiveType::Lines )
   {
      for ( uint32_t i = 0; i < mbi; i += 2 )
      {
         s << "2 " << mesh.getIndex( i ) << " " << mesh.getIndex( i+1 ) << "\n";
      }
   }
   else if ( mesh.getPrimitiveType() == PrimitiveType::Triangles )
   {
      for ( uint32_t i = 0; i < mbi; i += 3 )
      {
         s << "3 " << mesh.getIndex( i ) << " " << mesh.getIndex( i+1 )
           << " " << mesh.getIndex( i+2 ) << "\n";
      }
   }
//      else if ( mesh.getPrimitiveType() == PrimitiveType::Quads )
//      {
//         for ( uint32_t i = 0; i < mbi; i += 4 )
//         {
//            s << "4 " << mesh.getIndex( i ) << " " << mesh.getIndex( i+1 )
//              << " " << mesh.getIndex( i+2 ) << " " << mesh.getIndex( i+3 ) << "\n";
//         }
//      }
   else
   {
      s << "# Unsupported face type\n";
   }

   if ( dbSaveText( uri, s.str() ) )
   {
      DE_DEBUG("Saved OFF mesh file to disk, uri = ", uri )
      DE_DEBUG("Saved OFF mesh name = ", mesh.getName() )
      return true;
   }
   else
   {
      DE_DEBUG("Cant save OFF mesh file to disk, uri = ", uri )
      return false;
   }

   return true;
}

/*
struct SMeshBufferLoadOFF
{
   DE_CREATE_LOGGER("de.gpu.SMeshBufferLoadOFF")
   //EType getType() const {	return EMWT_OBJ; }

   static std::string
   toString( glm::vec3 const & pos )
   {
      std::ostringstream s;
      s << -pos.x << " " << pos.y << " " << pos.z << "\n";
      return s.str();
   }

   static std::string
   toString( glm::vec2 const & pos )
   {
      std::ostringstream s;
      s << pos.x << " " << -pos.y << "\n";
      return s.str();
   }

   static std::string
   toString( uint32_t color, std::string prefix = "" )
   {
      std::ostringstream s; s << prefix << " "
      << float( RGBA_R(color) )/255.f << " "
      << float( RGBA_G(color) )/255.f << " "
      << float( RGBA_B(color) )/255.f << "\n";
      return s.str();
   }

};
*/


} // end namespace
} // end namespace
