#pragma once
#include "Material.hpp"

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
struct ScreenRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.ScreenRenderer")

   ScreenRenderer( VideoDriver* driver );
   ~ScreenRenderer() {}
   bool setMaterial( Material const & material );
   void unsetMaterial( Material const & material );
   void animate( double pts ) {}

protected:
   static std::string createShaderName( Material const & material );
   static std::string createVS( Material const & material );
   static std::string createFS( Material const & material );
   VideoDriver* m_driver;
   Shader* m_shader[ 2 ];  // color or texture shader
   float m_zIndex;
   bool m_isDirty;
   AnimTimer m_animTime;
   Material m_material;
};

} // end namespace gpu.
} // end namespace de.