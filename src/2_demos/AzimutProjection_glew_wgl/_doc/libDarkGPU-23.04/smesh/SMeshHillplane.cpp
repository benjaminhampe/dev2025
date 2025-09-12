#include <de/gpu/smesh/SMeshHillplane.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

void
SMeshHillplane::add( SMeshBuffer & o, Image const & heightMap, glm::vec3 siz )
{
   o.setPrimitiveType( PrimitiveType::Triangles );
   o.setLighting( 2 );
   //o.setCulling( false );
   //o.setDepth( Depth( Depth::Always, true ) );
   //o.setTexture( 0, m_Texture0 );

   int32_t w = heightMap.getWidth();
   int32_t h = heightMap.getHeight();
   if ( w < 2 ) { DE_ERROR("Not enough heightmap width") return; }
   if ( h < 2 ) { DE_ERROR("Not enough heightmap height") return; }

   if ( heightMap.getFormat() == ColorFormat::R32F )
   {
      DE_ERROR("[Benni] Got R32F GeoTIFF heightmap")
   }

   // createTerrainVertices();
   o.Vertices.clear();
   o.Vertices.reserve( size_t( w ) * size_t( h ) );

   float cell_x = siz.x / float( w - 1 );
   float cell_y = siz.y / 255.0f;
   float cell_z = siz.z / float( h - 1 );
   for ( int32_t j = 0; j < h; ++j )
   {
      for ( int32_t i = 0; i < w; ++i )
      {
         uint8_t r = RGBA_R( heightMap.getPixel( i, j ) );
         float x = cell_x * i;
         float y = cell_y * float( r );
         float z = cell_z * j;
         float u = float( i ) / float( w - 1 );
         float v = float( j ) / float( h - 1 );
         uint32_t color = 0xFFFFFFFF; // RGBA( r,r,r );
         o.addVertex( S3DVertex( x,y,z, 0,1,0, color, u,v ));
      }
   }

   o.recalculateBoundingBox();

   SMeshBufferTool::computeNormals( o );

   //createTerrainIndices();
   o.Indices.clear();
   o.Indices.reserve( 6 * size_t( w-1 ) * size_t( h-1 ) );

   for ( int32_t j = 0; j < h-1; ++j )
   {
      for ( int32_t i = 0; i < w-1; ++i )
      {
         size_t iA = size_t( i ) + size_t( w ) * size_t( j );
         size_t iB = iA + size_t( w );
         size_t iC = iB + 1;
         size_t iD = iA + 1;
         o.addIndexedQuad( iA, iB, iC, iD );
      }
   }

}

} // end namespace gpu.
} // end namespace de.


