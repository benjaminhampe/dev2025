#pragma once
#include <de/gpu/Material.hpp>
#include <de/EGL_Validate.hpp>

namespace de {
namespace gpu {

//===========================================================
struct EGLSupport
//===========================================================
{
   DE_CREATE_LOGGER("de.gpu.EGLSupport")

   bool m_useEGL;
   int m_debugLevel;
   int m_currentVSync;
   int m_desiredVSync;

   EGLDisplay m_eglDisplay;   ///< Selected display - Currently used EGL display
   EGLConfig m_eglConfig;   ///< Selected config - Currently used EGL config
   EGLSurface m_eglSurface;   ///< Selected surface - Currently used EGL surface
   EGLContext m_eglContext;   ///< Selected context - Currently used EGL context

   EGLSupport();
   ~EGLSupport();

   bool isUsed() const {return m_useEGL; }
   int getVSync() const {return m_desiredVSync; }

   void setVSync( int vsync );

   void makeCurrent();
   void swapBuffers();

   void destroy();
   bool create( bool useEGL, int w, int h, uint64_t winHandle, SurfaceFormat const & fmt, int vsync, int logLevel );

};

} // end namespace gpu.
} // end namespace de.
