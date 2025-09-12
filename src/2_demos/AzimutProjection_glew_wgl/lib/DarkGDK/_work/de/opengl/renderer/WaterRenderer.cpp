#include <de/gpu/renderer/WaterRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

WaterRenderer::WaterRenderer()
   : m_driver(nullptr)
   , m_WaterHeight( -1.0f ) // -1m
   , m_TimeStart(dbSeconds())
   , m_Time(0.0)
{}

WaterRenderer::~WaterRenderer()
{

}

void
WaterRenderer::render()
{
   if ( !m_driver ) { DE_ERROR("No driver") return; }

   if ( !m_RT.fbo )
   {
      RenderTargetFactory::create( m_RT, 1024, 1024, m_driver );
   }

   // Set RestorePoint
   GLint old_fbo = 0;
   ::glGetIntegerv( GL_FRAMEBUFFER_BINDING, &old_fbo ); GL_VALIDATE;

   // FBO:
   ::glBindFramebuffer( GL_FRAMEBUFFER, m_RT.fbo ); GL_VALIDATE

   // I. ReflectionPass
   auto camera = m_driver->getCamera();
   if (!camera) { DE_ERROR("No camera") return; }

   /*
   // Reflect camera at water surface ( m_WaterHeight )
   //          Cam.pos.y
   //         /
   //        /
   //   ----+-------------- m_WaterHeight
   //        \\
   //         \\
   //          NewCam.pos.y = m_WaterHeight - ( Cam.pos.y - m_WaterHeight )
   //
   */
   auto old_pos = camera->getPos();
   auto old_dir = camera->getDir();
   auto old_up = camera->getUp();
   auto new_pos = old_pos;
   auto new_dir = old_dir;
   new_pos.y = m_WaterHeight - ( old_pos.y - m_WaterHeight );
   new_dir.y *= -1.f; // reflect direction
   camera->lookAt( new_pos, new_dir, old_up );

   // Render scene that will be reflected ( optimize here by using a reduced scene )
   // on water surface

   SMeshBuffer lines ( PrimitiveType::Lines );
   lines.setLighting( false );
   lines.setBlend( Blend::alphaBlend() );

   m_driver->draw3D( lines );

   // FBO:
   ::glBindFramebuffer( GL_FRAMEBUFFER, old_fbo ); GL_VALIDATE

}


} // end namespace gpu.
} // end namespace de.









