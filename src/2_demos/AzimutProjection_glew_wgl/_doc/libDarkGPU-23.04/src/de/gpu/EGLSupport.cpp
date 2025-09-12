#include "EGLSupport.hpp"

namespace de {
namespace gpu {

//===========================================================
struct EGL
//===========================================================
{
   DE_CREATE_LOGGER("de.gpu.EGL")

   static std::vector< EGLConfig >
   getConfigVector( EGLDisplay display )
   {
      if ( !display )
      {
   #ifdef USE_DARKEGL_DEBUGGING
         DE_ERROR("Invalid pointer to EGL display")
   #endif
         return {};
      }

      EGLint n = 0;
      EGLBoolean ok = ::eglGetConfigs( display, nullptr, 0, &n );
      EGL_VALIDATE
      if ( ok == EGL_FALSE )
      {
         n = 0;
      }
      if ( n < 1 )
      {
         return {};
      }

      std::vector< EGLConfig > configs;
      configs.reserve( n );
      for ( EGLint i = 0; i < n; ++i )
      {
         configs.push_back( nullptr );
      }

      // fill configs
   #ifdef USE_DARKEGL_DEBUGGING
      EGLint m = n;
   #endif
      ok = ::eglGetConfigs( display, configs.data(), configs.size(), &n );
      EGL_VALIDATE
   #ifdef USE_DARKEGL_DEBUGGING
      // check outcome container sizes
      if ( m != n )
      {
         DE_WARN("Returned config count(",m,") differs from container sized(",n,")")
      }
   #endif
      // return good stuff
      return configs;
   }

   // ===========================================================================
   // Configs
   // ===========================================================================
   static std::vector< EGLConfig >
   getConfigVector( EGLDisplay display, SurfaceFormat const & fmt )
   {
      if ( !display ) { DE_ERROR("No display.") return {}; }

      std::vector< EGLint > const eformat = EGL::toSurfaceFormatAttribVector( fmt );

      EGLint supportCount = 0;
      EGLBoolean ok = ::eglChooseConfig( display, eformat.data(), nullptr, 0, &supportCount );
      EGL_VALIDATE
      if ( supportCount < 1 || ok != EGL_TRUE )
      {
         DE_ERROR("Unsupported surface format ", fmt.toString())
         return {};
      }

      std::vector< EGLConfig > supported( supportCount, nullptr );
      EGLint receiveCount = 0;
      ok = ::eglChooseConfig( display, eformat.data(), supported.data(), supported.size(), &receiveCount );
      EGL_VALIDATE
      if ( ok != EGL_TRUE )
      {
         DE_ERROR("No configs")
         return {};
      }

      if ( supportCount != receiveCount )
      {
         DE_WARN("Returned sizes differ")
      }

   //      std::cout << "[Info] " << __func__ << " :: EGL.Supported.SurfaceFormats " << supported.size() << "\n";
   //      for ( size_t i = 0; i < supported.size(); ++i )
   //      {
   //         std::string t = getConfigString( display, supported[ i ] );
   //         std::cout << "[Info] " << __func__ << " :: EGL.Supported[" << i << "] " << t << "\n";
   //      }
      return supported;
   }

   // ===========================================================================
   // ConfigValue
   // ===========================================================================
   static EGLint
   getConfigValue( EGLDisplay display, EGLConfig config, EGLint attr )
   {
      EGLint value{ 0 };
      EGLBoolean ok = ::eglGetConfigAttrib( display, config, attr, &value );
      EGL_VALIDATE
      if ( ok == EGL_FALSE )
      {
         value = 0;
      }
      return value;
   }


   // ===========================================================================
   // ConfigId
   // ===========================================================================
   static EGLint
   getConfigId( EGLDisplay display, EGLConfig config )
   {
      EGLint value{ 0 };
      EGLBoolean ok = ::eglGetConfigAttrib( display, config, EGL_CONFIG_ID, &value );
      EGL_VALIDATE
      if ( ok == EGL_FALSE )
      {
         value = 0;
      }
      return value;
   }


   // ===========================================================================
   // ConfigString
   // ===========================================================================
   static std::string
   getConfigString( EGLDisplay display, EGLConfig config )
   {
      std::stringstream txt;
      GLint const id = getConfigValue( display, config, EGL_CONFIG_ID );
      //txt << "ID";
      //if ( id < 100 ) txt << "0";
      //if ( id < 10 ) txt << "0";
      txt << id;
   //   GLint const cbt = getConfigValue( display, config, EGL_COLOR_BUFFER_TYPE );
   //   if ( cbt == EGL_RGB_BUFFER )
   //   {
   //      txt << "Color";
   //   }
   //   else if ( cbt == EGL_LUMINANCE_BUFFER )
   //   {
   //      txt << "Luminance";
   //   }
      GLint const colorBits = getConfigValue( display, config, EGL_BUFFER_SIZE );
      if ( colorBits > 0 )
      {
         txt << "_" << colorBits;
         GLint const a = getConfigValue( display, config, EGL_ALPHA_SIZE );
         GLint const r = getConfigValue( display, config, EGL_RED_SIZE );
         GLint const g = getConfigValue( display, config, EGL_GREEN_SIZE );
         GLint const b = getConfigValue( display, config, EGL_BLUE_SIZE );
         txt << "_";
         txt << "RGB";
         if ( a > 0 ) { txt << "A"; }
         txt << r << g << b;
         if ( a > 0 ) { txt << a; }
      }
      GLint const d = getConfigValue( display, config, EGL_DEPTH_SIZE );
      GLint const s = getConfigValue( display, config, EGL_STENCIL_SIZE );
      if ( d > 0 || s > 0 )
      {
         txt << "_";
         if ( d > 0 ) { txt << "D" << d; }
         if ( s > 0 ) { txt << "S" << s; }
      }
      GLint const swa = getConfigValue( display, config, EGL_MIN_SWAP_INTERVAL );
      GLint const swb = getConfigValue( display, config, EGL_MAX_SWAP_INTERVAL );
      txt << "_Swap(" << swa << "," << swb << ")";
      GLint const pbw = getConfigValue( display, config, EGL_MAX_PBUFFER_WIDTH );
      GLint const pbh = getConfigValue( display, config, EGL_MAX_PBUFFER_HEIGHT );
      txt << "_MaxTexSize(" << pbw << "," << pbh << ")_";
      GLint const st = getConfigValue( display, config, EGL_SURFACE_TYPE );
      if ( st > 0 )
      {
         if ( EGL_WINDOW_BIT == ( st & EGL_WINDOW_BIT ) ) txt << "_WindowBit";
         if ( EGL_PBUFFER_BIT == ( st & EGL_PBUFFER_BIT ) ) txt << "|PBufferBit";
         if ( EGL_PIXMAP_BIT == ( st & EGL_PIXMAP_BIT ) ) txt << "|PixmapBit";
      }
      return txt.str();
   }

   static EGLDisplay
   createDisplay()
   {
      EGLDisplay display = ::eglGetDisplay( nullptr );  EGL_VALIDATE
      if ( !display )
      {
         DE_ERROR("No eglGetDisplay()")
         return nullptr; // EGL_NO_DISPLAY - Unable to open connection to local windowing system
      }

      // Initialize
      EGLint major, minor;
      EGLBoolean ok = ::eglInitialize( display, &major, &minor );
      EGL_VALIDATE

      if ( ok != EGL_TRUE ) // Unable to initialize EGL; handle and recover
      {
         return nullptr;
      }

      std::vector< EGLConfig > configs = getConfigVector( display );
      DE_DEBUG("EglVersion(",major,".",minor,"), Display(",display,"), EglConfigCount(", configs.size(), ")")
      
      for ( size_t i = 0; i < configs.size(); ++i )
      {
         std::string t = getConfigString( display, configs[ i ] );
         DE_DEBUG("EglConfig[",i,"] ",t)
      }
      return display;
   }

   static SurfaceFormat
   getSurfaceFormat( EGLDisplay display, EGLConfig config )
   {
      SurfaceFormat fmt;
      GLint const colorBits = EGL::getConfigValue( display, config, EGL_BUFFER_SIZE );
      fmt.r = EGL::getConfigValue( display, config, EGL_RED_SIZE );
      fmt.g = EGL::getConfigValue( display, config, EGL_GREEN_SIZE );
      fmt.b = EGL::getConfigValue( display, config, EGL_BLUE_SIZE );
      fmt.a = EGL::getConfigValue( display, config, EGL_ALPHA_SIZE );
      fmt.d = EGL::getConfigValue( display, config, EGL_DEPTH_SIZE );
      fmt.s = EGL::getConfigValue( display, config, EGL_STENCIL_SIZE );

      if ( colorBits != fmt.getColorBits() )
      {
         DE_WARN("BitSize not match ",colorBits," != ",fmt.getColorBits())
      }
      return fmt;
   }

   // ===========================================================================
   static std::vector< EGLint >
   toSurfaceFormatAttribVector( SurfaceFormat const & fmt )
   {
      // Color RGBA_DS surface format list.
      std::vector< EGLint > attribs {
         EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
         EGL_BUFFER_SIZE, fmt.getColorBits(),
         EGL_RED_SIZE, fmt.r,
         EGL_GREEN_SIZE, fmt.g,
         EGL_BLUE_SIZE, fmt.b,
         EGL_ALPHA_SIZE, fmt.a,
         EGL_DEPTH_SIZE, fmt.d,
         EGL_STENCIL_SIZE, fmt.s,
         EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR
      };

      // push SurfaceType: (Window|PBuffer)
      attribs.push_back( EGL_SURFACE_TYPE );
      if ( fmt.offscreen )
      {
         attribs.push_back( EGL_PBUFFER_BIT );
      }
      else
      {
         attribs.push_back( EGL_WINDOW_BIT );
      }

      // push End
      attribs.push_back( EGL_NONE );
      return attribs;
   }
   
   static std::vector< EGLConfig >
   setSurfaceFormat( EGLDisplay display, SurfaceFormat fmt )
   {
      if ( !display )
      {
         DE_ERROR("Invalid display.")
         return {};
      }

      EGLint supportCount = 0;
      EGLBoolean ok = EGL_TRUE;

      std::vector< EGLint > attribs = EGL::toSurfaceFormatAttribVector( fmt );
      ok = ::eglChooseConfig( display,
                              attribs.data(),
                              nullptr,
                              0,
                              &supportCount );
      EGL_VALIDATE
      if ( supportCount < 1 || ok != EGL_TRUE )
      {
         DE_ERROR("Unsupported surface format ", fmt.toString())
         return {};
      }

      std::vector< EGLConfig > supported( supportCount, nullptr );
      EGLint receiveCount = 0;
      ok = ::eglChooseConfig( display,
                              attribs.data(),
                              supported.data(),
                              supported.size(),
                              &receiveCount );
      EGL_VALIDATE
      if ( ok != EGL_TRUE )
      {
         DE_ERROR("No configs")
         return {};
      }

      if ( supportCount != receiveCount )
      {
         DE_WARN("Returned sizes differ")
      }
      return supported;
   }


   // ===========================================================================
   // Surface
   // ===========================================================================
   static EGLSurface
   createWindowSurface( EGLDisplay & display, int32_t w, int32_t h, 
                        uint64_t nativeHandle, SurfaceFormat fmt, EGLConfig * outSelectedConfig )
   {
      if ( !display ) { DE_ERROR("No display.") return nullptr; }
      if ( w < 1 ) { DE_ERROR("No window width.") return nullptr; }
      if ( h < 1 ) { DE_ERROR("No window height.") return nullptr; }
      if ( nativeHandle < 1 ) { DE_ERROR("No native window handle.") return nullptr; }

      fmt.offscreen = false;
      std::vector< EGLConfig > supported = setSurfaceFormat( display, fmt );
      if ( supported.size() < 1 )
      {
         DE_ERROR( "Not supported surface format(", fmt.toString(), ")" )
         return nullptr;
      }
      EGLConfig selectedConfig = supported[ 0 ];
      std::vector< EGLint > windowAttr{
            // EGL_RENDER_BUFFER, EGL_BACK_BUFFER, // NoPBuffer
            EGL_NONE
         };

      EGLSurface surface = ::eglCreateWindowSurface( display, selectedConfig,
         EGLNativeWindowType( nativeHandle ), windowAttr.data() ); EGL_VALIDATE
      if ( !surface )
      {
         DE_ERROR( "Cant create surface") return nullptr;
      }

      if ( outSelectedConfig )
      {
         *outSelectedConfig = selectedConfig;
      }

      DE_DEBUG( "SupportedConfigCount = ", supported.size() )
      for ( size_t i = 0; i < supported.size(); ++i )
      {
         DE_DEBUG( "SupportedConfig[",i,"] ",getConfigString(display,supported[i]) )
      }

      DE_DEBUG("Selected[",getConfigId( display, selectedConfig ),"] = ", getConfigString( display, selectedConfig ) )
      DE_DEBUG("Window surface w(", w, "), h(", h, "), fmt(", fmt.toString(), ")" )

      return surface;
   }

   // ===========================================================================
   // Surface
   // ===========================================================================
   static EGLSurface
   createPBufferSurface( EGLDisplay & display, int32_t w, int32_t h,
                                  SurfaceFormat fmt, EGLConfig * outSelectedConfig, bool debug )
   {
      if ( !display ) { DE_ERROR("No display.") return nullptr; }
      fmt.offscreen = true;
      std::vector< EGLConfig > supported = setSurfaceFormat( display, fmt );
      if ( supported.size() < 1 )
      {
         DE_ERROR( "Not supported surface format(", fmt.toString(), ")" )
         return nullptr;
      }
      EGLConfig selectedConfig = supported[ 0 ];

      std::vector< EGLint > windowAttr{
            EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
            EGL_NONE
         };

      EGLSurface surface = ::eglCreatePbufferSurface( display,
                                                     selectedConfig,
                                                     windowAttr.data() );
      EGL_VALIDATE
      if ( !surface ) { DE_ERROR( "Cant create PBuffer surface") return nullptr; }

      if ( outSelectedConfig )
         *outSelectedConfig = selectedConfig;

   #ifdef USE_DARKEGL_DEBUGGING
      DE_DEBUG( "CompatConfigs = ", supported.size() )
      for ( size_t i = 0; i < supported.size(); ++i )
      {
         DE_DEBUG( "CompatConfig[",i,"] ",getConfigString(display,supported[i]) )
      }

      DE_DEBUG("SelectedConfig = ", selectedConfig, ", "
         "id(", getConfigId( display, selectedConfig ), "), "
         "", getConfigString( display, selectedConfig ) )

      DE_DEBUG("PBuffer width ", w )
      DE_DEBUG("PBuffer height ", h )
      DE_DEBUG("PBuffer format ", fmt.toString() )
   #endif

      return surface;
   }

   // ===========================================================================
   // createContext
   // ===========================================================================

   static EGLContext
   createContext( EGLDisplay display, EGLSurface surface, EGLConfig selected )
   {
      if ( !display ) { DE_ERROR("No display.") return nullptr; }
      if ( !surface ) { DE_ERROR("No surface.") return nullptr; }
      if ( !selected ) { DE_ERROR("No selected config.") return nullptr; }

      ::eglBindAPI( EGL_OPENGL_ES_API ); EGL_VALIDATE

      std::vector< EGLint > const contextAttr { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };// requires 'EGL_KHR_create_context'.
      
      EGLContext ctx = ::eglCreateContext( display, selected, nullptr, contextAttr.data() ); EGL_VALIDATE
      if ( !ctx ) { DE_ERROR("Cant create context") return nullptr; }
      //EGLint contextVersion = 0;
      //::eglQueryContext( display, ctx, EGL_CONTEXT_CLIENT_VERSION, &contextVersion );
      //EGL_VALIDATE
      //DE_DEBUG("Created OpenGL ES context with version (",contextVersion,")")
      return ctx;
   }

};

//===========================================================
EGLSupport::EGLSupport()
//===========================================================
   : m_useEGL( false )
   , m_debugLevel( 0 )
   , m_currentVSync( -1 )
   , m_desiredVSync( 0 )
   , m_eglDisplay( nullptr )
   , m_eglConfig( nullptr )
   , m_eglSurface( nullptr )
   , m_eglContext( nullptr )
{}


EGLSupport::~EGLSupport()
{
   destroy();
}

void 
EGLSupport::setVSync( int vsync )
{
   if ( vsync < 0 || vsync > 3 )
   {
      return;
   }
   m_desiredVSync = vsync;
}

void 
EGLSupport::makeCurrent()
{
   if ( !m_useEGL ) { return; }

   if ( !m_eglDisplay ){ DE_WARN("Not display") return; }
   if ( !m_eglSurface ){ DE_WARN("Not surface") return; }
   if ( !m_eglContext ){ DE_WARN("Not context") return; }

   ::eglMakeCurrent( m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext );
   EGL_VALIDATE
}

void 
EGLSupport::swapBuffers()
{
   if ( !m_useEGL ) { return; }

   if ( m_eglSurface )
   {
      EGLBoolean ok;
      if ( m_currentVSync != m_desiredVSync )
      {
         ok = ::eglSwapInterval( m_eglDisplay, m_desiredVSync ); EGL_VALIDATE
         if ( ok != EGL_TRUE ) { DE_ERROR("Swap interval error") }
         m_currentVSync = m_desiredVSync;
      }

      ok = ::eglSwapBuffers( m_eglDisplay, m_eglSurface ); EGL_VALIDATE
   }
}

void 
EGLSupport::destroy()
{
   if ( !m_useEGL ) { return; }

   if ( m_eglDisplay && m_eglContext )
   {
      ::eglDestroyContext( m_eglDisplay, m_eglContext ); EGL_VALIDATE
      m_eglContext = nullptr;
   }

   if ( m_eglDisplay && m_eglSurface )
   {
      ::eglDestroySurface( m_eglDisplay, m_eglSurface ); EGL_VALIDATE
      m_eglSurface = nullptr;
   }

   if ( m_eglDisplay )
   {
      ::eglTerminate( m_eglDisplay ); EGL_VALIDATE
      m_eglDisplay = nullptr;
   }
}

bool 
EGLSupport::create( bool useEGL, int w, int h, uint64_t winHandle, SurfaceFormat const & fmt, int vsync, int logLevel )
{
   m_useEGL = useEGL;
   m_desiredVSync = vsync;
   m_debugLevel = logLevel;

   if ( !m_useEGL ) { return false; }

   m_eglDisplay = EGL::createDisplay();
   if ( !m_eglDisplay ) 
   { 
      DE_ERROR("No display") 
      return false; 
   }
   
   // [EGL] Window surface0 = onscreen
   m_eglSurface = EGL::createWindowSurface( m_eglDisplay, w, h, winHandle, fmt, &m_eglConfig );
   if ( !m_eglSurface ) 
   { 
      DE_ERROR("No window surface") 
      return false;    
   }

   // [EGL] Window surface Context
   m_eglContext = EGL::createContext( m_eglDisplay, m_eglSurface, m_eglConfig );
   if ( !m_eglContext )
   { 
      DE_ERROR("No context") 
      return false;    
   }

   makeCurrent();
   return true;
}


} // end namespace gpu.
} // end namespace de.
