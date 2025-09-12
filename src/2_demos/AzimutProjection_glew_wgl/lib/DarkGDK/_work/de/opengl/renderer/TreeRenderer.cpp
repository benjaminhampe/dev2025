#include <de/gpu/renderer/TreeRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

void addStumpf( SMeshBuffer & ast )
{
   SMeshCylinder::addXZ( ast, 10, 10, 9, 9, 1, 0xFFFFFFFF, 13, 1, glm::vec3(0,0,0) );
   SMeshCylinder::addXZ( ast, 9, 9, 7, 7, 1, 0xFFFFFFFF, 13, 2, glm::vec3(0,1,0) );
   SMeshCylinder::addXZ( ast, 7, 7, 6.5, 6.5, 10, 0xFFFFFFFF, 13, 3, glm::vec3(0,2,0) );
   SMeshCylinder::addXZ( ast, 6.5, 6.5, 6.7, 6.8, 10, 0xFFFFFFFF, 13, 5, glm::vec3(0,12,0) );
   SMeshCylinder::addXZ( ast, 6.7, 6.8, 6.1, 6.0, 20, 0xFFFFFFFF, 13, 10, glm::vec3(0,22,0) );
   SMeshCylinder::addXZ( ast, 6.1, 6.0, 4.8, 4.6, 20, 0xFFFFFFFF, 13, 10, glm::vec3(0,42,0) );
   SMeshCylinder::addXZ( ast, 4.8, 4.6, 4.9, 4.7, 20, 0xFFFFFFFF, 13, 10, glm::vec3(0,62,0) );
   SMeshCylinder::addXZ( ast, 4.9, 4.7, 4.1, 4.1, 5, 0xFFFFFFFF, 13, 5, glm::vec3(0,82,0) );
   SMeshCylinder::addXZ( ast, 4.1, 4.1, 3.2, 3.1, 20, 0xFFFFFFFF, 13, 10, glm::vec3(0,87,0) );
}

TreeRenderer::TreeRenderer()
   : m_driver(nullptr)
{
   // Add dir lines
   m_baum0 = SMeshBuffer( PrimitiveType::Triangles );


   m_baum0.setLighting( 1 );
   //m_baum0.setBlend( Blend::alphaBlend() );

//   m_baum0.Vertices.reserve( 200 );
//   m_baum0.Indices.reserve( 200 );
//   m_baum0.addVertex( S3DVertex( s.x,s.y,s.z, 0,0,-1, 0xFFFFFFFF,0,0 ) );
//   m_baum0.addVertex( S3DVertex( e.x,e.y,e.z, 0,0,-1, 0xFFFFFFFF,0,0 ) );
//   m_baum0.addIndex( 0 );
//   m_baum0.addIndex( 1 );

//   for ( size_t i = 1; i < points.size(); ++i )
//   {
//      int k1 = i - 1;
//      int k2 = i;
//      auto const & a = points[ k1 ];
//      auto const & b = points[ k2 ];
//      S3DVertex A( a.x,a.y,a.z, 0,0,-1, 0xFF00FFFF,0,0 );
//      S3DVertex B( b.x,b.y,b.z, 0,0,-1, 0xFFA0FFFF,0,0 );
//      int k = m_baum0.getVertexCount();
//      m_baum0.addVertex( A );
//      m_baum0.addVertex( B );
//      m_baum0.addIndex( k );
//      m_baum0.addIndex( k+1 );
//   }

   addStumpf( m_baum0 );
   for ( int i = 0; i < 6; ++i )
   {
      SMeshBuffer ast( PrimitiveType::Triangles );
      addStumpf( ast );
      SMeshBufferTool::scaleVertices( ast, 0.5, 0.5, 0.5 );
      //SMeshBufferTool::translateVertices( ast, 0, 50, 0 );
      SMeshBufferTool::rotateVertices( ast, 90, 0, 60*i );

//      if ( i == 0 ) SMeshBufferTool::rotateVertices( ast, 90, 0, 0 );
//      else if ( i == 1 ) SMeshBufferTool::rotateVertices( ast, 90, 180, 0 );
//      else if ( i == 2 ) SMeshBufferTool::rotateVertices( ast, 0, 0, 0 );
//      else if ( i == 3 ) SMeshBufferTool::rotateVertices( ast, 0, 0, 0 );
//      else if ( i == 4 ) SMeshBufferTool::rotateVertices( ast, 0, 0, 0 );
//      else if ( i == 5 ) SMeshBufferTool::rotateVertices( ast, 0, 0, 0 );

//      for ( int k = 0; k < 6; ++k )
//      {
//            SMeshBuffer tmp( PrimitiveType::Triangles );
//            addStumpf( tmp );
//         SMeshBufferTool::scaleVertices( tmp, 0.5, 0.5, 0.5 );
//         SMeshBufferTool::rotateVertices( tmp, 60, 60*k, 0 );
//         SMeshBufferTool::translateVertices( tmp, 0, 50, 0 );
//         SMeshBufferTool::fuse( ast, tmp );
//      }

      SMeshBufferTool::fuse( m_baum0, ast );
   }

   SMeshBufferTool::translateVertices( m_baum0, -100, 0, 100 );
   m_baum0.recalculateBoundingBox();
   SMeshBufferTool::computeNormals( m_baum0 );
}

//TreeRenderer::~TreeRenderer() {}
void
TreeRenderer::setDriver( VideoDriver* driver ) { m_driver = driver; }
void
TreeRenderer::clear() {}
void
TreeRenderer::render()
{
   if ( !m_driver ) { DE_ERROR("No driver") return; }

   de::gpu::TexRef tex = m_driver->getTexture( "../../media/TreeRenderer/baumstamm.jpg");
   m_baum0.setTexture( 0,tex );

//   ITexture* tex = m_driver->getTexture( "rayr" );
//   if ( !tex ) { DE_ERROR("No tex") return; }
//   {
//      tex = m_driver->addTexture2D( "rayr", createRayImage() );
//   }

   m_driver->draw3D( m_baum0 );

}


} // end namespace gpu.
} // end namespace de.









