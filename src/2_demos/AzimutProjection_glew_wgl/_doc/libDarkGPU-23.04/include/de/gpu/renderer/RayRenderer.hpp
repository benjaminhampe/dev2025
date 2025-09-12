#ifndef DE_GPU_RAY_RENDERER_HPP
#define DE_GPU_RAY_RENDERER_HPP

#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

// ===========================================================================
struct RayRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.RayRenderer")

   struct Item
   {
      glm::vec3 dir; // normalized direction vector ( abs of dir is in v_dir )
      glm::vec3 pos; // pos
      glm::vec3 p_start; // start pos
      float t = 0.0f; // time0;
      float phi_start = 0.0f; // phase0;
      float f = 33.0f; // frequency of EM field ( elongation along dir )
      float amp = 10.0f; // amplitude A
      float v_dir = 1.0f; // velocity abs along direction ( c for light, but that would be invisible )
      float a_dir = 0.0f; // acceleration abs along direction ( for visual effects ) can inc or dec velocity over time

   };

   VideoDriver* m_Driver;
   double m_TimeStart;
   double m_Time;
   std::vector< Item > m_Items;

   RayRenderer();
   //~RayRenderer();
   void setDriver( VideoDriver* driver );
   void clear();
   void add( glm::vec3 const & pos, glm::vec3 const & dir );

   void
   start();

   void
   render();
};

} // end namespace gpu.
} // end namespace de.

#endif

