#pragma once
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

// ===========================================================================
struct MathRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.MathRenderer")

   MathRenderer( VideoDriver* driver );
   ~MathRenderer() {}
   void render();

   VideoDriver* m_driver;
   SMeshBuffer m_Test1; // BKS - xyz quaternion test

};

} // end namespace gpu.
} // end namespace de.
