#pragma once
#include <de/gpu/RenderTarget.hpp>

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
struct WaterRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.WaterRenderer")

   // Stupid as hell
   static void
   addWave( std::vector< glm::vec3 > & points, float amp, float freq, float phase, float t )
   {
      for ( size_t i = 0; i < points.size(); ++i )
      {
         auto & pos = points[ i ];
         //auto t = float(i) / float( points.size() );
         auto phi = fmod( float(2.0*M_PI) * freq * (t+float(i)) + phase, float(2.0*M_PI) );
         pos.y += amp * sin( phi );
      }
   }

   WaterRenderer();
   ~WaterRenderer();
   void render();

   void setDriver( VideoDriver* driver ) { m_driver = driver; }

   VideoDriver* m_driver;

   float m_WaterHeight;

   RenderTargetData m_RT;

   double m_TimeStart;
   double m_Time;

};

} // end namespace gpu.
} // end namespace de.


