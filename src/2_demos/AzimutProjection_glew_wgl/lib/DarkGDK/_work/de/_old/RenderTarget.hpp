#pragma once

/// ==========================================================================
/// @file RenderTarget.hpp
/// @author 2018-2023 Benjamin Hampe <benjaminhampe@gmx.de>
/// @copyright Free open source software
/// ==========================================================================

#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
struct RenderTargetData
// ===========================================================================
{
   int w;        // screen width
   int h;       // screen height
   uint32_t fbo;        // GL fbo id
   uint32_t colorTex;   // GL tex id RGBA
   uint32_t depthTex;   // GL tex id DepthStencilCombi D24S8 probably.
   TexInternalFormat colorFormat; // Defined in <Material.hpp> and contains 3 uint32_t values that OpenGL needs.
   TexInternalFormat depthFormat; // Defined in <Material.hpp> and contains 3 uint32_t values that OpenGL needs.
   int getWidth() const { return w; }
   int getHeight() const { return h; }

};

// Factory implementation depends on OpenGL and is in .cpp file, header does not depend on OpenGL
// ===========================================================================
struct RenderTargetFactory
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.RenderTargetFactory")

   static bool
   create( RenderTargetData & rt, int w, int h, VideoDriver* driver );

   static void
   destroy( RenderTargetData & rt, VideoDriver* driver );
};

// Stateful RenderTarget that RAII auto destroys itself when leaves out of scope/block {}.
// ===========================================================================
struct RenderTarget
// ===========================================================================
{
   VideoDriver* m_driver;
   RenderTargetData m_state;

   RenderTarget() : m_driver( nullptr ) {}
   RenderTarget( int w, int h, VideoDriver* driver ) { create( w, h, driver ); }
   ~RenderTarget() { destroy(); }

   bool create( int w, int h, VideoDriver* driver )
   {
      m_driver = driver;
      return RenderTargetFactory::create( m_state, w, h, m_driver );
   }

   void destroy()
   {
      RenderTargetFactory::destroy( m_state, m_driver );
   }
};

} // end namespace gpu.
} // end namespace de.

