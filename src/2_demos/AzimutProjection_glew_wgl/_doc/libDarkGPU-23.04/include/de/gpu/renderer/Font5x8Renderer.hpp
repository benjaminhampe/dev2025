#pragma once
#include <de/Font5x8.hpp>
#include <de/gpu/smesh/SMesh.hpp>

namespace de {
namespace gpu {

// Declare existence of mighty VideoDriver class.
struct VideoDriver;

// ===========================================================================
struct Font5x8Renderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.Font5x8Renderer")

   Font5x8Renderer( VideoDriver* driver = nullptr );
   ~Font5x8Renderer();

   static void
   add5x8ColorQuadsToMeshBuffer(
      SMeshBuffer & o, int x, int y,
      std::string const & msg, uint32_t color, Align align, Font5x8 font = Font5x8() );

   void setDriver( VideoDriver* driver ) { m_Driver = driver; }

   glm::ivec2
   getTextSize(
      std::string const & msg,
      Font5x8 const & font = Font5x8() ) const;

   void
   add2DText( SMeshBuffer & o, int x, int y,
            std::string const & msg,
            uint32_t color,
            Align align,
            Font5x8 const & font );

   void
   draw2DText( int x, int y,
               std::string const & msg,
               uint32_t color = 0xFFFFFFFF,
               Align align = Align::Default,
               Font5x8 const & font = Font5x8() );

   VideoDriver* m_Driver;

};

} // end namespace gpu.
} // end namespace de.
