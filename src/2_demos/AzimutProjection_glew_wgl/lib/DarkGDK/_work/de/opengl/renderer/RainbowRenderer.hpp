#ifndef DE_GPU_RAINBOW_RENDERER_HPP
#define DE_GPU_RAINBOW_RENDERER_HPP

#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

// ===========================================================================
struct SRainbow
// ===========================================================================
{
   std::vector< glm::vec3 > points;
   SMeshBuffer mesh;
   glm::dmat4 modelMat; // TRS
};

// ===========================================================================
struct RainbowRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.RainbowRenderer")
   float m_ScreenGamma;
   SMeshBuffer m_Mesh;
   TexRef m_rainbowTexture;

   std::vector< SRainbow > m_Fibers;
   RainbowRenderer();
   ~RainbowRenderer();

   void
   clear();

   void
   addRainbow( float dx, float dy,
      glm::dmat4 const & modelMat = glm::dmat4(1.0),
      float lineWidth = 100.f,
      int nSegments = 100 );

   void
   addTriangleStrip( std::vector< glm::vec3 > points, float lineWidth = 100.0f  );

   void
   render( VideoDriver* driver );

   void
   draw3D( IMeshBuffer & vao, VideoDriver* driver );

   void
   draw3D( SMesh & mesh, VideoDriver* driver )
   {
      for ( SMeshBuffer & p : mesh.Buffers )
      {
         draw3D( p, driver );
      }
   }

   float getGamma() const { return m_ScreenGamma; }
   void setGamma( float gamma ) { m_ScreenGamma = gamma; }
};

} // end namespace gpu.
} // end namespace de.

#endif

