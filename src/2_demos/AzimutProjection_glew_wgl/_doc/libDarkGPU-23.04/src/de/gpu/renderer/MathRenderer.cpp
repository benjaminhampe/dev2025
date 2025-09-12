#include <de/gpu/renderer/MathRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

MathRenderer::MathRenderer( VideoDriver* driver )
   : m_driver( driver )
{
   //mesh::Cylinder::addXY( m_Test1, 5, 100,100,100 );
   SMeshBufferTool::translateVertices( m_Test1, 0,50,0 );
}

void
MathRenderer::render()
{
   if (!m_driver) return;
//   int w = m_driver->getScreenWidth();
//   int h = m_driver->getScreenHeight();
//   de::gpu::ICamera* camera = m_driver->getActiveCamera();

//   int mx = m_driver->getMouseX();
//   int my = m_driver->getMouseY();

//   //m_driver->draw2DLine( mx, 0, mx, h, 0x2FFFFFFF );
//   if ( camera )
//   {

//   }

   //m_Test1.setTexture( 0, m_driver->getTexture( "irr" ) );
   //m_driver->draw3D( m_Test1 );
}


} // end namespace gpu.
} // end namespace de.
