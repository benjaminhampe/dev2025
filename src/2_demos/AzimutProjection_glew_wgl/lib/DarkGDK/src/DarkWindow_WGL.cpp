#include "DarkWindow_WGL.h"

#include <vector> // for iAttributes in CreateContext

#ifdef BENNI_USE_COUT

inline std::ostream &
operator<< ( std::ostream & o, RECT const & rect )
{
   o << rect.left << "," << rect.top << ","
     << (rect.right - rect.left) << "," << (rect.bottom - rect.top);
   return o;
}

#endif

namespace DarkGDK {

uint32_t
convertLocaleIdToCodepage( uint32_t localeId );

LRESULT CALLBACK
OpenGLWindow_WGL_WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

DarkWindow_WGL::DarkWindow_WGL()
   : m_hInstance( nullptr )
   , m_hWnd( nullptr )
   , m_hDC( nullptr )
   , m_hRC( nullptr )
   , m_eventReceiver( nullptr )
   , m_shouldRun( true )
   , m_dummyWnd( nullptr )
   , m_dummyDC( nullptr )
   , m_dummyRC( nullptr )
   //, m_opengl32( nullptr )
   , m_KEYBOARD_INPUT_HKL( nullptr )
   , m_KEYBOARD_INPUT_CODEPAGE( 1252 )
   , m_screenWidth( 640 )
   , m_screenHeight( 480 )
{
//   if ( !create() )
//   {
//      std::cout << "[Error] Cant create dummy window" << std::endl;
//   }

//   if ( !loadOpenGL() )
//   {
//      std::cout << "[Error] Cant loadOpenGL()" << std::endl;
//   }
}

DarkWindow_WGL::~DarkWindow_WGL()
{
   destroy();
}

void DarkWindow_WGL::yield( int ms )
{
   if ( ms < 1 ) ms = 1;
   if ( ms > 10000 ) ms = 10000;
   Sleep( DWORD(ms) );
}

void
DarkWindow_WGL::setEventReceiver( IEventReceiver* receiver )
{
   m_eventReceiver = receiver;
}

IEventReceiver*
DarkWindow_WGL::getEventReceiver()
{
   return m_eventReceiver;
}

void
DarkWindow_WGL::requestClose()
{
   if ( m_hWnd )
   {
      PostMessage( m_hWnd, WM_QUIT, 0, 0 );
   }
}

void
DarkWindow_WGL::swapBuffers()
{
   // TODO: rework this. Seems like enabled vsync = 1 leads to 30 fps, not 60
   //       which should mean we swapped one time too often.
   //       Do we need SwapBuffers at all in combi with DwmFlush ?
   //       Should it rather be only one of them, not both at once?

   // TODO: Do we need glFlush() at all for modern GL ( non fixed pipeline )???
   // glFlush();

   //if (IsWindowsVistaOrGreater())
   //{
      // DWM Composition is always enabled on Win8+
      BOOL win8plus = IsWindows8OrGreater();
      if ( win8plus )
      {
         BOOL enabled;
         SUCCEEDED( DwmIsCompositionEnabled(&enabled) );

         // HACK: Use DwmFlush when desktop composition is enabled
         if (enabled)
         {
            int count = abs(m_params.vsync);
            while (count--)
            {
               DwmFlush();
            }
         }
      }
   //}

   bool ok = SwapBuffers( m_hDC ) == TRUE;
}



void
DarkWindow_WGL::destroy()
{
//   if ( m_dummyRC )
//   {
//      wglDeleteContext( m_dummyRC );
//      m_dummyRC = nullptr;
//   }

//   if ( m_opengl32 )
//   {
//      FreeLibrary( m_opengl32 );
//      m_opengl32 = nullptr;
//   }

//   wglMakeCurrent(hDC, nullptr);
//   wglDeleteContext(hRC);
//   ReleaseDC(hwnd, hDC);
//   DestroyWindow(hwnd);
//   UnregisterClass(className.c_str(), hInstance);

}

bool
DarkWindow_WGL::create( CreateParams params )
{
   m_params = params;
   m_hInstance = GetModuleHandle( nullptr );
   std::string className = "de_OpenGLWindow_WGL";
   std::string dummyName = "de_OpenGLWindow_WGL_dummy2";
//   #ifdef BENNI_USE_COUT
//   std::cout << "DesiredCreateParams(" << m_params.toString() << ")" << std::endl;
//   #endif

   // =============================================================
   // DEVMODE
   // =============================================================
   memset(&m_desktopMode, 0, sizeof(DEVMODE));
   m_desktopMode.dmSize = sizeof(DEVMODE);
   EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &m_desktopMode);

   // =============================================================
   // Create first Window
   // =============================================================
   WNDCLASSEXA wcex;
   wcex.cbSize			= sizeof(WNDCLASSEXA);
   wcex.style        = CS_HREDRAW | CS_VREDRAW; //  | CS_OWNDC
   wcex.cbClsExtra   = 0;
   wcex.cbWndExtra   = 0;
   wcex.hInstance    = m_hInstance;
   wcex.hIcon        = LoadIcon( nullptr, IDI_WINLOGO );
   wcex.hIconSm		= LoadIcon( nullptr, IDI_WINLOGO );
   wcex.hCursor      = LoadCursor( nullptr, IDC_ARROW );   // IDC_ARROW,_UPARROW,_WAIT,_APPSTARTING,_CROSS,_HAND,_HELP,_IBEAM,_NO,_SIZEALL,_SIZENESW,_SIZENS,_SIZENWSE,_SIZEWE
   wcex.hbrBackground= reinterpret_cast<HBRUSH>(COLOR_WINDOW+1); //CreateSolidBrush( RGB( 0, 150, 255 ) );
   wcex.lpszMenuName = nullptr;
   wcex.lpszClassName= className.c_str();
   wcex.lpfnWndProc  = OpenGLWindow_WGL_WndProc;

   if ( !RegisterClassExA( &wcex ) )
   {
      #ifdef BENNI_USE_COUT
      std::cout << "RegisterClass(" << className << ") failed" << std::endl;
      #endif
      return false;
   }

   // Figure out the WindowStyle flags:

   //DWORD dwWndStyle = WS_OVERLAPPEDWINDOW;
   //DWORD dwExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

   m_windowedStyle = WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
                   // | WS_THICKFRAME;
   m_fullscreenStyle = WS_POPUP;

   if ( m_params.isResizable )
   {
      m_windowedStyle |= DWORD(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
   }

   if ( m_params.isFullscreen )
   {
      m_windowStyle = m_fullscreenStyle;
   }
   else
   {
      m_windowStyle = m_windowedStyle;
   }

   // Use WindowStyle and desired FrameBuffer size to figure out the WindowSize:
   RECT r_window;
   r_window.left = 0;
   r_window.top = 0;
   r_window.right = params.width; // Desired framebuffer width
   r_window.bottom = params.height; // Desired framebuffer height
   AdjustWindowRect( &r_window, m_windowStyle, FALSE );
   #ifdef BENNI_USE_COUT
   std::cout << "AdjustWindowRect(" << r_window << ")" << std::endl;
   #endif

   int winW = r_window.right - r_window.left;
   int winH = r_window.bottom - r_window.top;
   int winX = 0;
   int winY = 0;

   if (m_params.isFullscreen)
   {
      winX = 0;
      winY = 0;
   }
   else
   {
      if ( winX < 0 ) winX = 0;
      if ( winY < 0 ) winY = 0;	// make sure window menus are in screen on creation
   }

   m_hWnd = CreateWindowA( className.c_str(),
                           "DarkWindow_WGL",
                           m_windowStyle,
                           winX, winY, winW, winH,
                           nullptr,
                           nullptr,
                           m_hInstance,
                           this );

   #ifdef BENNI_USE_COUT
   std::cout << "create window(" << m_hWnd << ")" << std::endl;
   #endif

   ShowWindow( m_hWnd, SW_SHOWNORMAL );
   UpdateWindow( m_hWnd );
   ShowCursor( TRUE );

   // fix ugly ATI driver bugs. Thanks to ariaci
   MoveWindow( m_hWnd, winX, winY, winW, winH, TRUE );

   GetClientRect( m_hWnd, &r_window );

   #ifdef BENNI_USE_COUT
   std::cout << "FinalWindowRect(" << r_window << ")" << std::endl;
   #endif

   // =============================================================
   // Create dummy Window
   // =============================================================

   WNDCLASSEXA dummy_wcex;
   dummy_wcex.cbSize			= sizeof(WNDCLASSEXA);
   dummy_wcex.style        = CS_HREDRAW | CS_VREDRAW; //  | CS_OWNDC
   dummy_wcex.cbClsExtra   = 0;
   dummy_wcex.cbWndExtra   = 0;
   dummy_wcex.hInstance    = m_hInstance;
   dummy_wcex.hIcon        = nullptr; // LoadIcon( nullptr, IDI_WINLOGO );
   dummy_wcex.hIconSm		= nullptr; // LoadIcon( nullptr, IDI_WINLOGO );
   dummy_wcex.hCursor      = nullptr; // LoadCursor( nullptr, IDC_ARROW );   // IDC_ARROW,_UPARROW,_WAIT,_APPSTARTING,_CROSS,_HAND,_HELP,_IBEAM,_NO,_SIZEALL,_SIZENESW,_SIZENS,_SIZENWSE,_SIZEWE
   dummy_wcex.hbrBackground= nullptr; //reinterpret_cast<HBRUSH>(COLOR_WINDOW+1); //CreateSolidBrush( RGB( 0, 150, 255 ) );
   dummy_wcex.lpszMenuName = nullptr;
   dummy_wcex.lpszClassName= dummyName.c_str();
   dummy_wcex.lpfnWndProc  = DefWindowProc;

   if ( !RegisterClassExA( &dummy_wcex ) )
   {
      #ifdef BENNI_USE_COUT
      std::cout << "RegisterClass(" << dummyName << ") failed" << std::endl;
      #endif
      return false;
   }

   m_dummyWnd = CreateWindowA( dummyName.c_str(), "GLTestAAsupportWindow2",
            m_windowStyle, winX, winY, winW, winH,
            nullptr, nullptr, m_hInstance, nullptr );

   if (!m_dummyWnd)
   {
      #ifdef BENNI_USE_COUT
      std::cout << "Cannot create a temporary window." << std::endl;
      #endif
      UnregisterClassA(dummyName.c_str(), m_hInstance);
      return false;
   }

   #ifdef BENNI_USE_COUT
   std::cout << "Created dummy window(" << m_dummyWnd << ")" << std::endl;
   #endif

   m_dummyDC = GetDC( m_dummyWnd );

   ZeroMemory(&m_pfd, sizeof(PIXELFORMATDESCRIPTOR));
   // Set up pixel format descriptor with desired parameters
   m_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Size Of This Pixel Format Descriptor
   m_pfd.nVersion = 1;
   m_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL; //  | PFD_DOUBLEBUFFER
   m_pfd.iPixelType = PFD_TYPE_RGBA;        // Request An RGBA Format
   m_pfd.cColorBits = 32;                   // Select Our Color Depth
   m_pfd.cDepthBits = 24;                   // Z-Buffer (Depth Buffer)
   m_pfd.cStencilBits = 8;                  // Stencil Buffer Depth
   m_pfd.iLayerType = PFD_MAIN_PLANE;       // Main Drawing Layer

   int pixelFormat = ChoosePixelFormat(m_dummyDC, &m_pfd);
   if (!pixelFormat)
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] No such pixelFormat available" << std::endl;
      #endif
      ReleaseDC( m_dummyWnd, m_dummyDC );
      DestroyWindow( m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), m_hInstance );
      return false;
   }

   if (!SetPixelFormat(m_dummyDC, pixelFormat, &m_pfd))
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] Cant set pixelFormat for dummy context" << std::endl;
      #endif
      ReleaseDC( m_dummyWnd, m_dummyDC );
      DestroyWindow( m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), m_hInstance );
      return false;
   }

   // Create WGL context
   m_dummyRC = wglCreateContext( m_dummyDC );
   if (!m_dummyRC)
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] No WGL context created" << std::endl;
      #endif
      ReleaseDC( m_dummyWnd, m_dummyDC );
      DestroyWindow( m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), m_hInstance );
      return false;
   }

   HDC old_dc = wglGetCurrentDC();
   HGLRC old_rc = wglGetCurrentContext();

   if (!wglMakeCurrent( m_dummyDC, m_dummyRC ))
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] WGL: Failed to make dummy context current" << std::endl;
      #endif
      wglMakeCurrent( old_dc, old_rc );
      wglDeleteContext( m_dummyRC );
      ReleaseDC( m_dummyWnd, m_dummyDC );
      DestroyWindow( m_dummyWnd );
      UnregisterClassA( dummyName.c_str(), m_hInstance );
      m_dummyRC = nullptr;
      return false;
   }

   std::string m_wglExtensionString;

#ifdef WGL_ARB_extensions_string
   PFNWGLGETEXTENSIONSSTRINGARBPROC my_wglGetExtensionsStringARB =
      reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(
         wglGetProcAddress("wglGetExtensionsStringARB"));

   if (my_wglGetExtensionsStringARB)
   {
      auto t = my_wglGetExtensionsStringARB( m_dummyDC );
      if (t)
      {
         m_wglExtensionString = t;
      }
      else
      {
         #ifdef BENNI_USE_COUT
         std::cout << "[Error] wglGetExtensionsStringARB returned nullptr" << std::endl;
         #endif
      }
   }
   else
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] No wglGetExtensionsStringARB" << std::endl;
      #endif
   }
#elif defined(WGL_EXT_extensions_string)
   PFNWGLGETEXTENSIONSSTRINGEXTPROC my_wglGetExtensionsStringEXT =
   reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGEXTPROC>(wglGetProcAddress("wglGetExtensionsStringEXT"));
   if (my_wglGetExtensionsStringEXT)
   {
      auto t = my_wglGetExtensionsStringEXT();
      if (t)
      {
         m_wglExtensionString = t;
      }
      else
      {
         m_wglExtensionString = "";
         std::cout << "[Error] wglGetExtensionsStringEXT returned nullptr" << std::endl;
      }
   }
   else
   {
      std::cout << "[Error] No wglGetExtensionsStringEXT" << std::endl;
   }
#endif

   printf( "WGL_EXTENSIONS = %s\n", m_wglExtensionString.c_str() );

   bool const hasWGL_ARB_pixel_format = m_wglExtensionString.find("WGL_ARB_pixel_format") != std::string::npos;


   #ifdef BENNI_USE_COUT
   bool const hasWGL_ARB_multisample = m_wglExtensionString.find("WGL_ARB_multisample") != std::string::npos;
   bool const hasWGL_EXT_multisample = m_wglExtensionString.find("WGL_EXT_multisample") != std::string::npos;
   bool const hasWGL_3DFX_multisample = m_wglExtensionString.find("WGL_3DFX_multisample") != std::string::npos;

   bool const hasWGL_multisample = hasWGL_ARB_multisample
                                || hasWGL_EXT_multisample
                                || hasWGL_3DFX_multisample;

   bool const hasWGL_ARB_framebuffer_sRGB = m_wglExtensionString.find("WGL_ARB_framebuffer_sRGB") != std::string::npos;
   bool const hasWGL_EXT_framebuffer_sRGB = m_wglExtensionString.find("WGL_EXT_framebuffer_sRGB") != std::string::npos;
   bool const hasWGL_framebuffer_sRGB = hasWGL_ARB_framebuffer_sRGB
                                     || hasWGL_EXT_framebuffer_sRGB;

   std::cout << "WGL extensions: "<< m_wglExtensionString << std::endl;

//   std::vector<std::string> wglExtensions = splitText(m_wglExtensionString, ' ');

//   std::cout << "WGL extensions count = "<< wglExtensions.size() << std::endl;
//   for ( size_t i = 0; i < wglExtensions.size(); ++i )
//   {
//      std::cout << "WGL extension[" << i <<"] "<< wglExtensions[i] << std::endl;
//   }

   std::cout << "hasWGL_ARB_pixel_format = " << hasWGL_ARB_pixel_format << std::endl;
   std::cout << "hasWGL_ARB_multisample = " << hasWGL_ARB_multisample << std::endl;
   std::cout << "hasWGL_EXT_multisample = " << hasWGL_EXT_multisample << std::endl;
   std::cout << "hasWGL_3DFX_multisample = " << hasWGL_3DFX_multisample << std::endl;
   std::cout << "hasWGL_multisample = " << hasWGL_multisample << std::endl;
   std::cout << "hasWGL_ARB_framebuffer_sRGB = " << hasWGL_ARB_framebuffer_sRGB << std::endl;
   std::cout << "hasWGL_EXT_framebuffer_sRGB = " << hasWGL_EXT_framebuffer_sRGB << std::endl;
   std::cout << "hasWGL_framebuffer_sRGB = " << hasWGL_framebuffer_sRGB << std::endl;
   #endif

// #ifdef WGL_ARB_pixel_format
   PFNWGLCHOOSEPIXELFORMATARBPROC my_wglChoosePixelFormatARB =
    reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));

   int pixelFormatWGL = 0;

   if (hasWGL_ARB_pixel_format && my_wglChoosePixelFormatARB)
   {
      // This value determines the number of samples used for antialiasing
      // My experience is that 8 does not show a big
      // improvement over 4, but 4 shows a big improvement
      // over 2.

      if(m_params.aa > 32)
         m_params.aa = 32;

      std::vector< int32_t > iAttributes
      {
         WGL_DRAW_TO_WINDOW_ARB, 1,
         WGL_SUPPORT_OPENGL_ARB, 1,
         WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
         WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
         WGL_RED_BITS_ARB, m_params.r,
         WGL_GREEN_BITS_ARB, m_params.g,
         WGL_BLUE_BITS_ARB, m_params.b,
         WGL_ALPHA_BITS_ARB, m_params.a,
         WGL_DEPTH_BITS_ARB, m_params.d,
         WGL_STENCIL_BITS_ARB, m_params.s,
         WGL_DOUBLE_BUFFER_ARB, m_params.isDoubleBuffered ? 1 : 0,
         WGL_STEREO_ARB, 0,
         //WGL_SWAP_EXCHANGE_ARB, 1
      };

#if 0 // !!! Multisampling not working on my PC
      if ( hasWGL_multisample )
      {
         std::cout << "Use WGL_multisample" << std::endl;
      #ifdef WGL_ARB_multisample
         iAttributes.emplace_back( WGL_SAMPLES_ARB );
         iAttributes.emplace_back( m_params.aa );
         iAttributes.emplace_back( WGL_SAMPLE_BUFFERS_ARB );
         iAttributes.emplace_back( 1 );
      #elif defined(WGL_EXT_multisample)
         iAttributes.emplace_back( WGL_SAMPLES_EXT );
         iAttributes.emplace_back( m_params.aa );
         iAttributes.emplace_back( WGL_SAMPLE_BUFFERS_EXT );
         iAttributes.emplace_back( 1 );
         #elif defined(WGL_3DFX_multisample)
         iAttributes.emplace_back( WGL_SAMPLES_3DFX );
         iAttributes.emplace_back( m_params.aa );
         iAttributes.emplace_back( WGL_SAMPLE_BUFFERS_3DFX );
         iAttributes.emplace_back( 1 );
      #endif
         }

      if (hasWGL_framebuffer_sRGB)
      {
         std::cout << "Use WGL_framebuffer_sRGB" << std::endl;
      #ifdef WGL_ARB_framebuffer_sRGB
         iAttributes.emplace_back( WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB );
         iAttributes.emplace_back( m_params.handleSRGB ? 1 : 0 );
      #elif defined(WGL_EXT_framebuffer_sRGB)
         iAttributes.emplace_back( WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT );
         iAttributes.emplace_back( m_params.handleSRGB ? 1 : 0 );
      #endif
      }
      //iAttributes.emplace_back( WGL_DEPTH_FLOAT_EXT );
      //iAttributes.emplace_back( 1 );
#endif

      iAttributes.emplace_back( 0 );
      iAttributes.emplace_back( 0 );

      f32 fAttributes[] = {0.0f, 0.0f};

      UINT numFormats = 0;
      #ifdef BENNI_USE_COUT
      BOOL const valid =
      #endif
      my_wglChoosePixelFormatARB( m_dummyDC, iAttributes.data(), fAttributes, 1,
         &pixelFormatWGL, &numFormats);

      #ifdef BENNI_USE_COUT
      std::cout << "Got num formats = " << numFormats << std::endl;
      std::cout << "Got valid = " << valid << std::endl;
      #endif
   }
   else
   {
      m_params.aa = 0;
      m_params.handleSRGB = false;
   }

   wglMakeCurrent( m_dummyDC, nullptr );
   wglDeleteContext( m_dummyRC );
   ReleaseDC( m_dummyWnd, m_dummyDC );
   DestroyWindow( m_dummyWnd );
   UnregisterClassA( dummyName.c_str(), m_hInstance );

   if ( !pixelFormatWGL )
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] pixelFormatWGL == 0." << std::endl;
      #endif
   }
   else
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Ok] pixelFormat = " << pixelFormat << "." << std::endl;
      std::cout << "[Ok] pixelFormatWGL = " << pixelFormatWGL << "." << std::endl;
      #endif
   }

   m_hDC = GetDC( m_hWnd );
   if (!m_hDC)
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] Cannot create a GL DC device context." << std::endl;
      #endif
      return false;
   }

   if ( !SetPixelFormat( m_hDC, pixelFormatWGL, &m_pfd ) )
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] Cannot set the GL pixel format." << std::endl;
      #endif
      return false;
   }

#if 0 // !!! wglCreateContextAttribsARB not working on my PC !!!
   bool const hasWGL_ARB_create_context = m_wglExtensionString.find("WGL_ARB_create_context") != std::string::npos;
   bool const hasWGL_ARB_create_context_profile = m_wglExtensionString.find("hasWGL_ARB_create_context_profile") != std::string::npos;
   std::cout << "hasWGL_ARB_create_context = " << hasWGL_ARB_create_context << std::endl;
   std::cout << "hasWGL_ARB_create_context_profile = " << hasWGL_ARB_create_context_profile << std::endl;

   // create rendering context
   //HGLRC WINAPI
   //wglCreateContextAttribsARB (HDC hDC, HGLRC hShareContext, const int *attribList);

   //#ifdef WGL_ARB_create_context
   //#endif

   PFNWGLCREATECONTEXTATTRIBSARBPROC my_wglCreateContextAttribsARB =
      reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));

   if ( my_wglCreateContextAttribsARB )
   {
      std::cout << "[Ok] Got wglCreateContextAttribsARB()" << std::endl;
   }
   else
   {
      std::cout << "[Error] No wglCreateContextAttribsARB()" << std::endl;
   }

   if ( my_wglCreateContextAttribsARB )
   {
      int iContextAttribs[] =
      {
         WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
         WGL_CONTEXT_MINOR_VERSION_ARB, 3,
         0
      };

      m_hRC = my_wglCreateContextAttribsARB( m_hDC, nullptr, iContextAttribs );

      if ( m_hRC )
      {
         std::cout << "[Ok] wglCreateContextAttribsARB(3,3)" << std::endl;
      }
      else
      {
         std::cout << "[Error] wglCreateContextAttribsARB(3,3)" << std::endl;
      }
   }
   else
#endif
   {
      m_hRC = wglCreateContext( m_hDC );
      if ( m_hRC )
      {
         #ifdef BENNI_USE_COUT
         std::cout << "[Warn] Only wglCreateContext()" << std::endl;
         #endif
      }
      else
      {
         #ifdef BENNI_USE_COUT
         std::cout << "[Error] Failed wglCreateContext()" << std::endl;
         #endif
         return false;
      }
   }

   wglMakeCurrent( m_hDC, m_hRC );

   // !!! wglSwapIntervalEXT not working on my PC, maybe i need to use all EXT, not ARB? !!!
#ifdef WGL_EXT_swap_control
   PFNWGLSWAPINTERVALEXTPROC my_wglSwapIntervalEXT =
      reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>( wglGetProcAddress("wglSwapIntervalEXT") );

   if (my_wglSwapIntervalEXT)
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Ok] wglSwapIntervalEXT." << std::endl;
      #endif
   }
   else
   {
      #ifdef BENNI_USE_COUT
      std::cout << "[Error] wglSwapIntervalEXT." << std::endl;
      #endif
   }

   if (my_wglSwapIntervalEXT)
   {
      my_wglSwapIntervalEXT( m_params.vsync );

      #ifdef BENNI_USE_COUT
      std::cout << "[Ok] my_wglSwapIntervalEXT." << std::endl;
      #endif
   }

   //wglSwapIntervalEXT( m_params.vsync );
#endif

   // OpenGLDriver.init():

   glewInit(); // VIP call

   //beginScene();
   //endScene();
   //swapBuffers();
   /// !!! Using GLAD not working on my PC !!!
#ifdef BENNI_USE_COUT
   auto t1 = glGetString(GL_VERSION);
   auto t2 = glGetString(GL_VENDOR);
   auto t3 = glGetString(GL_RENDERER);
   auto t4 = glGetString(GL_SHADING_LANGUAGE_VERSION);
   auto t5 = glGetString(GL_EXTENSIONS);

   if ( t1 )
   {
      std::cout << "[Ok] GL_VERSION = " << (char const*)t1 << std::endl;
   }
   else
   {
      std::cout << "[Error] GL_VERSION" << std::endl;
   }

   if ( t2 )
   {
      std::cout << "[Ok] GL_VENDOR = " << (char const*)t2 << std::endl;
   }
   else
   {
      std::cout << "[Error] GL_VENDOR" << std::endl;
   }

   if ( t3 )
   {
      std::cout << "[Ok] GL_RENDERER = " << (char const*)t3 << std::endl;
   }
   else
   {
      std::cout << "[Error] GL_RENDERER" << std::endl;
   }

   if ( t4 )
   {
      std::cout << "[Ok] GL_SHADING_LANGUAGE_VERSION = " << (char const*)t4 << std::endl;
   }
   else
   {
      std::cout << "[Error] GL_SHADING_LANGUAGE_VERSION" << std::endl;
   }

   std::string glExtensions;

   if ( t5 )
   {
      glExtensions = (char const*)t5;
      //std::cout << "GL extensions = "<< glExtensions << std::endl;
   }
   else
   {
      std::cout << "[Error] GL_EXTENSIONS" << std::endl;
   }

   s32 m_shaderVersionMajor = 0;
   s32 m_shaderVersionMinor = 0;
   glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor );
   glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor );

   //GL_EXT_texture_filter_anisotropic
   GLfloat maxAF = 0.0f;
   glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );

   GLint maxTexSize = 0;
   glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxTexSize );

   GLint numTexUnits = 0;
   glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &numTexUnits );

   // PointSize
   f32 pointSize[2] { 0,0 };
   f32 pointSizeRange[4] { 0,0,0,0 };
   glGetFloatv( GL_POINT_SIZE, pointSize );
   glGetFloatv( GL_POINT_SIZE_RANGE, pointSizeRange );

   f32 lineWidth[4] { 0,0,0,0 };
   f32 lineWidthRange[4] { 0,0,0,0 };
   f32 lineWidthGranularity[4] { 0,0,0,0 };
   glGetFloatv( GL_LINE_WIDTH, lineWidth );
   glGetFloatv( GL_LINE_WIDTH_RANGE, lineWidthRange );
   glGetFloatv( GL_LINE_WIDTH_GRANULARITY, lineWidthGranularity );


   // DEBUG BEGIN:
#if 1
   std::cout << "GL extensions: "<< glExtensions << std::endl;
#else
   std::vector< std::string > glExtensionsVec = splitText( glExtensions, ' ' );

   std::cout << "GL extensions count = "<< glExtensionsVec.size() << std::endl;
   for ( size_t i = 0; i < glExtensionsVec.size(); ++i )
   {
      std::cout << "GL extension[" << i <<"] "<< glExtensionsVec[i] << std::endl;
   }
#endif

   std::cout << "[GL] GL_MAJOR_VERSION = " << m_shaderVersionMajor << std::endl;
   std::cout << "[GL] GL_MINOR_VERSION = " << m_shaderVersionMinor << std::endl;

   std::cout << "[GL] GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = " << maxAF << std::endl;
   std::cout << "[GL] GL_MAX_TEXTURE_SIZE = " << maxTexSize << std::endl;
   std::cout << "[GL] GL_MAX_TEXTURE_IMAGE_UNITS = " << numTexUnits << std::endl;

   std::cout << "[GL] GL_POINT_SIZE = " << pointSize[0] << std::endl;
   std::cout << "[GL] GL_POINT_SIZE_RANGE = " << pointSizeRange[0] << "," << pointSizeRange[1] << std::endl;

   std::cout << "[GL] GL_LINE_WIDTH = " << lineWidth[0] << std::endl;
   std::cout << "[GL] GL_LINE_WIDTH_RANGE = " << lineWidthRange[0] << "," << lineWidthRange[1] << std::endl;
   std::cout << "[GL] GL_LINE_WIDTH_GRANULARITY = " << lineWidthGranularity[0] << std::endl;

   /*
   if ( params.m_msaa > 0 )
   {
      glEnable( GL_MULTISAMPLE );
      s32 samples[4] { 0,0,0,0 };
      glGetIntegerv( GL_MULTISAMPLE, samples );
      f32 mlwRange[4] { 0,0,0,0 };
      glGetFloatv( GL_MULTISAMPLE_LINE_WIDTH_RANGE, mlwRange );
      f32 mlwGranularity[4] { 0,0,0,0 };
      glGetFloatv( GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY, mlwGranularity );
      DE_DEBUG("GL_MULTISAMPLE")
      DE_DEBUG("GL_MULTISAMPLE = ", samples[0])
      DE_DEBUG("GL_MULTISAMPLE_LINE_WIDTH_RANGE = ",mlwRange[0],",",mlwRange[1])
      DE_DEBUG("GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY = ",mlwGranularity[0])
   }

   DE_DEBUG("GL_gpu_shader4 = ",glHasExtension( "GL_gpu_shader4" ))
   DE_DEBUG("GL_gpu_shader5 = ",glHasExtension( "GL_gpu_shader5" ))
   DE_DEBUG("GL_EXT_gpu_shader4 = ",glHasExtension( "GL_EXT_gpu_shader4" ))
   DE_DEBUG("GL_EXT_gpu_shader5 = ",glHasExtension( "GL_EXT_gpu_shader5" ))
   */

   #endif


   printf( "GL_EXTENSIONS = %s\n", glGetString(GL_EXTENSIONS) );
   printf( "GL_VERSION = %s\n", glGetString(GL_VERSION) );
   printf( "GL_VENDOR = %s\n", glGetString(GL_VENDOR) );
   printf( "GL_RENDERER = %s\n", glGetString(GL_RENDERER) );
   printf( "GL_SHADING_LANGUAGE_VERSION = %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );
   fflush( stdout );

   return true;
}


// [Keyboard] Windows VK_Key enum -> Benni's EKEY enum
EKEY
translateWinKey( UINT winKey )
{
   switch ( winKey )
   {
      case 48: return ::DarkGDK::KEY_0; // No VK enums for these? Are they all language dependent aswell?
      case 49: return ::DarkGDK::KEY_1;
      case 50: return ::DarkGDK::KEY_2;
      case 51: return ::DarkGDK::KEY_3;
      case 52: return ::DarkGDK::KEY_4;
      case 53: return ::DarkGDK::KEY_5;
      case 54: return ::DarkGDK::KEY_6;
      case 55: return ::DarkGDK::KEY_7;
      case 56: return ::DarkGDK::KEY_8;
      case 57: return ::DarkGDK::KEY_9;

      case 65: return ::DarkGDK::KEY_A; // No VK enums for these? Are they all language dependent aswell?
      case 66: return ::DarkGDK::KEY_B;
      case 67: return ::DarkGDK::KEY_C;
      case 68: return ::DarkGDK::KEY_D;
      case 69: return ::DarkGDK::KEY_E;
      case 70: return ::DarkGDK::KEY_F;
      case 71: return ::DarkGDK::KEY_G;
      case 72: return ::DarkGDK::KEY_H;
      case 73: return ::DarkGDK::KEY_I;
      case 74: return ::DarkGDK::KEY_J;
      case 75: return ::DarkGDK::KEY_K;
      case 76: return ::DarkGDK::KEY_L;
      case 77: return ::DarkGDK::KEY_M;
      case 78: return ::DarkGDK::KEY_N;
      case 79: return ::DarkGDK::KEY_O;
      case 80: return ::DarkGDK::KEY_P;
      case 81: return ::DarkGDK::KEY_Q;
      case 82: return ::DarkGDK::KEY_R;
      case 83: return ::DarkGDK::KEY_S;
      case 84: return ::DarkGDK::KEY_T;
      case 85: return ::DarkGDK::KEY_U;
      case 86: return ::DarkGDK::KEY_V;
      case 87: return ::DarkGDK::KEY_W;
      case 88: return ::DarkGDK::KEY_X;
      case 89: return ::DarkGDK::KEY_Y;
      case 90: return ::DarkGDK::KEY_Z;

      case VK_F1: return ::DarkGDK::KEY_F1;
      case VK_F2: return ::DarkGDK::KEY_F2;
      case VK_F3: return ::DarkGDK::KEY_F3;
      case VK_F4: return ::DarkGDK::KEY_F4;
      case VK_F5: return ::DarkGDK::KEY_F5;
      case VK_F6: return ::DarkGDK::KEY_F6;
      case VK_F7: return ::DarkGDK::KEY_F7;
      case VK_F8: return ::DarkGDK::KEY_F8;
      case VK_F9: return ::DarkGDK::KEY_F9;
      case VK_F10: return ::DarkGDK::KEY_F10; // F10 not working on Logitech K280e. d.k. the fuck why.
      case VK_F11: return ::DarkGDK::KEY_F11;
      case VK_F12: return ::DarkGDK::KEY_F12;
      case VK_F13: return ::DarkGDK::KEY_F13;
      case VK_F14: return ::DarkGDK::KEY_F14;
      case VK_F15: return ::DarkGDK::KEY_F15;
      case VK_F16: return ::DarkGDK::KEY_F16;
      case VK_F17: return ::DarkGDK::KEY_F17;
      case VK_F18: return ::DarkGDK::KEY_F18;
      case VK_F19: return ::DarkGDK::KEY_F19;
      case VK_F20: return ::DarkGDK::KEY_F20;
      case VK_F21: return ::DarkGDK::KEY_F21;
      case VK_F22: return ::DarkGDK::KEY_F22;
      case VK_F23: return ::DarkGDK::KEY_F23;
      case VK_F24: return ::DarkGDK::KEY_F24;

      case VK_ESCAPE: return ::DarkGDK::KEY_ESCAPE;
      case VK_RETURN: return ::DarkGDK::KEY_ENTER;
      case VK_BACK: return ::DarkGDK::KEY_BACKSPACE;
      case VK_SPACE: return ::DarkGDK::KEY_SPACE;
      case VK_TAB: return ::DarkGDK::KEY_TAB;
      case VK_LSHIFT: return ::DarkGDK::KEY_LEFT_SHIFT;
      case VK_RSHIFT: return ::DarkGDK::KEY_RIGHT_SHIFT;

      //case VK_MENU: return ::DarkGDK::KEY_LEFT_SUPER;
      //case VK_LMENU: return ::DarkGDK::KEY_LEFT_SUPER;
      //case VK_RMENU: return ::DarkGDK::KEY_RIGHT_SUPER;

      case VK_LWIN: return ::DarkGDK::KEY_LEFT_SUPER;
      case VK_RWIN: return ::DarkGDK::KEY_RIGHT_SUPER;
      case VK_LCONTROL: return ::DarkGDK::KEY_LEFT_CTRL;
      case VK_RCONTROL: return ::DarkGDK::KEY_RIGHT_CTRL;

//      else if (key == GLFW_KEY_LEFT_ALT ) { key_key = KEY_LEFT_ALT; }
//      else if (key == GLFW_KEY_RIGHT_ALT ) { key_key = KEY_RIGHT_ALT; }
//      else if (key == GLFW_KEY_LEFT_CONTROL ) { key_key = KEY_LEFT_CTRL; }
//      else if (key == GLFW_KEY_RIGHT_CONTROL ) { key_key = KEY_RIGHT_CTRL; }
//      else if (key == GLFW_KEY_LEFT_SUPER ) { key_key = KEY_LEFT_SUPER; }
//      else if (key == GLFW_KEY_RIGHT_SUPER ) { key_key = KEY_RIGHT_SUPER; }
//      else if (key == GLFW_KEY_CAPS_LOCK ) { key_key = KEY_CAPS_LOCK; }

      // OEM special keys: All German Umlaute so far and special characters
      // I dont own US or Japanese keyboards. Please add your special chars if needed.
      case VK_OEM_1: return ::DarkGDK::KEY_OEM_1;            // Ü, VK_OEM_1 = 186 = 0xBA
      case VK_OEM_PLUS: return ::DarkGDK::KEY_OEM_PLUS;      // VK_OEM_PLUS = 187 = 0xBB
      case VK_OEM_COMMA: return ::DarkGDK::KEY_OEM_COMMA;    // VK_OEM_COMMA = 188 = 0xBC
      case VK_OEM_MINUS: return ::DarkGDK::KEY_OEM_MINUS;    // VK_OEM_MINUS = 189 = 0xBD
      case VK_OEM_PERIOD: return ::DarkGDK::KEY_OEM_PERIOD;  // VK_OEM_PERIOD = 190 = 0xBE
      case VK_OEM_2: return ::DarkGDK::KEY_OEM_2;            // VK_OEM_2 = 191 = 0xBF
      case VK_OEM_3: return ::DarkGDK::KEY_OEM_3;            // Ö, VK_OEM_3 = 192 = 0xC0
      case VK_OEM_4: return ::DarkGDK::KEY_OEM_4;            // ß, VK_OEM_4 = 219 = 0xDB
      case VK_OEM_5: return ::DarkGDK::KEY_OEM_5;            // VK_OEM_5 = 220 = 0xDC
      case VK_OEM_6: return ::DarkGDK::KEY_OEM_6;            // VK_OEM_6 = 221 = 0xDD
      case VK_OEM_7: return ::DarkGDK::KEY_OEM_7;            // Ä, VK_OEM_7 = 222 = 0xDE
      case VK_OEM_102: return ::DarkGDK::KEY_OEM_102;        // <|>, VK_OEM_8 = 226 = 0xDE

      // 4x Arrow keys:
      case VK_UP: return ::DarkGDK::KEY_UP;
      case VK_LEFT: return ::DarkGDK::KEY_LEFT;
      case VK_DOWN: return ::DarkGDK::KEY_DOWN;
      case VK_RIGHT: return ::DarkGDK::KEY_RIGHT;

      // 3x control buttons:
      case VK_SNAPSHOT: return ::DarkGDK::KEY_SNAPSHOT; // PRINT SCREEN, not the PRINT (only) key
      case VK_SCROLL: return ::DarkGDK::KEY_SCROLL_LOCK;
      case VK_PAUSE: return ::DarkGDK::KEY_PAUSE;

      // 6x control buttons:
      case VK_INSERT: return ::DarkGDK::KEY_INSERT;
      case VK_HOME: return ::DarkGDK::KEY_HOME;
      case VK_END: return ::DarkGDK::KEY_END;
      case VK_DELETE: return ::DarkGDK::KEY_DELETE;
      case VK_PRIOR: return ::DarkGDK::KEY_PAGE_UP;
      case VK_NEXT: return ::DarkGDK::KEY_PAGE_DOWN;

      // Numpad:
      case VK_NUMLOCK: return ::DarkGDK::KEY_NUM_LOCK;
      case VK_NUMPAD0: return ::DarkGDK::KEY_KP_0;
      case VK_NUMPAD1: return ::DarkGDK::KEY_KP_1;
      case VK_NUMPAD2: return ::DarkGDK::KEY_KP_2;
      case VK_NUMPAD3: return ::DarkGDK::KEY_KP_3;
      case VK_NUMPAD4: return ::DarkGDK::KEY_KP_4;
      case VK_NUMPAD5: return ::DarkGDK::KEY_KP_5;
      case VK_NUMPAD6: return ::DarkGDK::KEY_KP_6;
      case VK_NUMPAD7: return ::DarkGDK::KEY_KP_7;
      case VK_NUMPAD8: return ::DarkGDK::KEY_KP_8;
      case VK_NUMPAD9: return ::DarkGDK::KEY_KP_9;
      case VK_MULTIPLY: return ::DarkGDK::KEY_KP_MULTIPLY; // Is that correct mapping?
      case VK_ADD: return ::DarkGDK::KEY_KP_ADD; // Is that correct mapping?
      //case VK_SEPARATOR: return ::DarkGDK::KEY_KP_SEPARATOR; // Is that correct mapping?
      case VK_SUBTRACT: return ::DarkGDK::KEY_KP_SUBTRACT; // Is that correct mapping?
      case VK_DECIMAL: return ::DarkGDK::KEY_KP_DECIMAL; // Is that correct mapping?
      case VK_DIVIDE: return ::DarkGDK::KEY_KP_DIVIDE; // Is that correct mapping?


      default:
      {
         #ifdef BENNI_USE_COUT
         std::cout << "Cant translate WinKey(" << winKey << ")" << std::endl;
         #endif
         return KEY_UNKNOWN;
      }
   }
}



LRESULT CALLBACK
OpenGLWindow_WGL_WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
   DarkWindow_WGL* glwin = nullptr;
   if ( message == WM_NCCREATE )
   {
      glwin = static_cast< DarkWindow_WGL* >(
                  reinterpret_cast< CREATESTRUCT* >( lParam )->lpCreateParams ); // MinGW wraps nicely
      /*
      win->m_HWnd = hWnd;
      */
      SetLastError( 0 );
      if ( !SetWindowLongPtr( hwnd, GWL_USERDATA, reinterpret_cast< LONG_PTR >( glwin ) ) )
      {
         DWORD const e = ::GetLastError();
         if ( e != 0 )
         {
            #ifdef BENNI_USE_COUT
            std::cout << "Async SetUserData failed " << std::endl;
            #endif
            // DE_ERROR( "AsyncWindow create failed ", getErrorString( dwError ) )
            //return FALSE;
         }
      }
   }
   else
   {
      glwin = reinterpret_cast< DarkWindow_WGL* >( GetWindowLongPtr( hwnd, GWL_USERDATA ) );
   }

//   if ( win )
//   {
//      return win->handleEvent( uMsg, wParam, lParam );
//   }
//   else
//   {
//      return DefWindowProc( hWnd, uMsg, wParam, lParam );
//   }


   switch (message)
   {
      case WM_NCCREATE: {
         #ifdef BENNI_USE_COUT
         std::cout << "WM_NCCREATE " << hwnd << std::endl;
         #endif
         break;
      }
      case WM_CREATE: {
         #ifdef BENNI_USE_COUT
         std::cout << "WM_CREATE " << hwnd << std::endl;
         #endif
         //setWindowIcon( u64(hwnd), aaaa );
         //setResizable( hwnd, true, 800, 600 );
         //createMenu( hwnd );
         break;
      }
      case WM_DESTROY: {
         #ifdef BENNI_USE_COUT
         std::cout << "WM_DESTROY " << hwnd << std::endl;
         #endif
         PostQuitMessage(0);
         return 0;
      }
      case WM_ERASEBKGND:
         return 0;
      case WM_PAINT: {
         PAINTSTRUCT ps;
         HDC hDC = BeginPaint(hwnd, &ps);
/*
         int w = 640;
         int h = 480;
         if ( glwin )
         {
            w = glwin->m_screenWidth;
            h = glwin->m_screenHeight;
         }

         HBRUSH solidBrush = CreateSolidBrush(RGB(0x3C, 0x43, 0x54));
         SelectObject(hDC, solidBrush);
         Rectangle(hDC, -1, -1, w+1, h+1);
         DeleteObject(solidBrush);
*/
         EndPaint(hwnd, &ps);
         return 0;
      }
      case WM_MOVE: {
         if ( glwin )
         {
            if ( glwin->getEventReceiver() )
            {
               WindowMoveEvent windowMoveEvent;
               windowMoveEvent.x = int( LOWORD( lParam ) );
               windowMoveEvent.y = int( HIWORD( lParam ) );
               //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
               //glwin->getEventReceiver()->onEvent( SEvent( windowMoveEvent ) );
               glwin->getEventReceiver()->windowMoveEvent( windowMoveEvent );
            }
         }
         return 0;
      }

      case WM_SIZE: {
         int w = int( LOWORD( lParam ) );
         int h = int( HIWORD( lParam ) );
         /*
         int dw = GetSystemMetrics( SM_CXSCREEN );
         int dh = GetSystemMetrics( SM_CYSCREEN );

         std::ostringstream o;
         o << "DarkWindow_WGL (c) 2023 by BenjaminHampe@gmx.de | "
              "Desktop("<<dw<<","<<dh<<"), Screen("<<w<<","<<h<<")";
         setWindowTitle( hwnd, o.str().c_str() );
         */
         if ( glwin )
         {
            glwin->m_screenWidth = w;
            glwin->m_screenHeight = h;

            if ( glwin->getEventReceiver() )
            {
               ResizeEvent resizeEvent;
               resizeEvent.w = w;
               resizeEvent.h = h;
               //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
               //glwin->m_eventReceiver->onEvent( SEvent( resizeEvent ) );
               glwin->getEventReceiver()->resizeEvent( resizeEvent );
            }
         }
         return 0;
      }

      // Mouse events:
      case WM_MOUSEMOVE: {
         int mx = int( LOWORD( lParam ) );
         int my = int( HIWORD( lParam ) );

         if ( glwin )
         {
            //glwin->m_mouseX = mx;
            //glwin->m_mouseY = my;

            if ( glwin->getEventReceiver() )
            {
               MouseMoveEvent mouseMoveEvent;
               mouseMoveEvent.x = mx;
               mouseMoveEvent.y = my;
               //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
               //glwin->m_eventReceiver->onEvent( SEvent( mouseMoveEvent ) );
               glwin->getEventReceiver()->mouseMoveEvent( mouseMoveEvent );
            }
         }
         return 0;
      }
      case WM_MOUSEWHEEL: {
         if ( glwin )
         {
            float mouseWheelY = float( int16_t( HIWORD( wParam ) ) ) / float( WHEEL_DELTA );

            if ( glwin->getEventReceiver() )
            {
               MouseWheelEvent mouseWheelEvent;
               mouseWheelEvent.x = 0.0f;
               mouseWheelEvent.y = mouseWheelY;
               //std::lock_guard< std::mutex > guard( os::win32::s_Mutex );
               glwin->getEventReceiver()->mouseWheelEvent( mouseWheelEvent );
            }
         }
         return 0;
      }

      case WM_LBUTTONDOWN: {
         if ( glwin->getEventReceiver() )
         {
            MousePressEvent mousePressEvent;
            mousePressEvent.x = LOWORD(lParam);
            mousePressEvent.y = HIWORD(lParam);
            mousePressEvent.buttons = MouseButton::Left;
            mousePressEvent.flags = 0;
            //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( SEvent( mousePressEvent ) );
            glwin->getEventReceiver()->mousePressEvent( mousePressEvent );
         }
         return 0;
      }
      case WM_RBUTTONDOWN: {
         if ( glwin->getEventReceiver() )
         {
            MousePressEvent mousePressEvent;
            mousePressEvent.x = LOWORD(lParam);
            mousePressEvent.y = HIWORD(lParam);
            mousePressEvent.buttons = MouseButton::Right;
            mousePressEvent.flags = 0;
            //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( SEvent( mousePressEvent ) );
            glwin->getEventReceiver()->mousePressEvent( mousePressEvent );
         }
         return 0;
      }
      case WM_MBUTTONDOWN: {
         if ( glwin->getEventReceiver() )
         {
            MousePressEvent mousePressEvent;
            mousePressEvent.x = LOWORD(lParam);
            mousePressEvent.y = HIWORD(lParam);
            mousePressEvent.buttons = MouseButton::Middle;
            mousePressEvent.flags = 0;
            //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( SEvent( mousePressEvent ) );
            glwin->getEventReceiver()->mousePressEvent( mousePressEvent );
         }
         return 0;
      }

      case WM_LBUTTONUP: {
         if ( glwin->getEventReceiver() )
         {
            MouseReleaseEvent mouseReleaseEvent;
            mouseReleaseEvent.x = LOWORD(lParam);
            mouseReleaseEvent.y = HIWORD(lParam);
            mouseReleaseEvent.buttons = MouseButton::Left;
            mouseReleaseEvent.flags = 0;
            //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( SEvent( mouseReleaseEvent ) );
            glwin->getEventReceiver()->mouseReleaseEvent( mouseReleaseEvent );
         }
         return 0;
      }
      case WM_RBUTTONUP: {
         if ( glwin->getEventReceiver() )
         {
            MouseReleaseEvent mouseReleaseEvent;
            mouseReleaseEvent.x = LOWORD(lParam);
            mouseReleaseEvent.y = HIWORD(lParam);
            mouseReleaseEvent.buttons = MouseButton::Right;
            mouseReleaseEvent.flags = 0;
            //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( SEvent( mouseReleaseEvent ) );
            glwin->getEventReceiver()->mouseReleaseEvent( mouseReleaseEvent );
         }
         return 0;
      }
      case WM_MBUTTONUP: {
         if ( glwin->getEventReceiver() )
         {
            MouseReleaseEvent mouseReleaseEvent;
            mouseReleaseEvent.x = LOWORD(lParam);
            mouseReleaseEvent.y = HIWORD(lParam);
            mouseReleaseEvent.buttons = MouseButton::Middle;
            mouseReleaseEvent.flags = 0;
            //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( SEvent( mouseReleaseEvent ) );
            glwin->getEventReceiver()->mouseReleaseEvent( mouseReleaseEvent );
         }
         return 0;
      }

      //case WM_XBUTTONDOWN:
      //case WM_XBUTTONUP:

   // Keyboard:
   case WM_INPUTLANGCHANGE:
      {
         if ( glwin )
         {
            auto hkl = GetKeyboardLayout( 0 ); // get the new codepage used for keyboard input

            glwin->m_KEYBOARD_INPUT_HKL = hkl; // get the new codepage used for keyboard input
            glwin->m_KEYBOARD_INPUT_CODEPAGE = convertLocaleIdToCodepage( LOWORD( hkl ) );
         }
         return 0;
      }
   case WM_SYSKEYDOWN:{
         if ( glwin && glwin->getEventReceiver() )
         {
            BYTE allKeys[ 256 ];
            GetKeyboardState( allKeys );
            bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
            bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

            // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
            // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
            UINT32 unicode = 0;
            wchar_t singleChar = 0;
            WORD keyChars[ 2 ];
            UINT scanCode = HIWORD( lParam );
            int conversionResult = ::ToAsciiEx( UINT(wParam),
                                              scanCode,
                                              allKeys,
                                              keyChars,
                                              0,
                                              glwin->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( SEvent( keyPressEvent ) );
            glwin->getEventReceiver()->keyPressEvent( keyPressEvent );
         }

         return 0;
//         if (message == WM_SYSKEYDOWN)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;
//         break;
      }
   case WM_KEYDOWN: {
         if ( glwin && glwin->getEventReceiver() )
         {
            BYTE allKeys[ 256 ];
            GetKeyboardState( allKeys );
            bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
            bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

            // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
            // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
            UINT32 unicode = 0;
            wchar_t singleChar = 0;
            WORD keyChars[ 2 ];
            UINT scanCode = HIWORD( lParam );
            int conversionResult = ::ToAsciiEx( UINT(wParam),
                                              scanCode,
                                              allKeys,
                                              keyChars,
                                              0,
                                              glwin->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( SEvent( keyPressEvent ) );
            glwin->getEventReceiver()->keyPressEvent( keyPressEvent );
         }

//         if (message == WM_SYSKEYDOWN)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;

         return 0;
      }

   case WM_SYSKEYUP:
   {
         if ( glwin && glwin->getEventReceiver() )
         {
            BYTE allKeys[ 256 ];
            GetKeyboardState( allKeys );
            bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
            bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

            // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
            // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
            UINT32 unicode = 0;
            wchar_t singleChar = 0;
            WORD keyChars[ 2 ];
            UINT scanCode = HIWORD( lParam );
            int conversionResult = ::ToAsciiEx( UINT(wParam),
                                              scanCode,
                                              allKeys,
                                              keyChars,
                                              0,
                                              glwin->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( SEvent( keyReleaseEvent ) );
            glwin->getEventReceiver()->keyReleaseEvent( keyReleaseEvent );
         }
//         if (message == WM_SYSKEYUP)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;
//         break;
            return 0;
      }
   case WM_KEYUP: {
         if ( glwin && glwin->getEventReceiver() )
         {
            BYTE allKeys[ 256 ];
            GetKeyboardState( allKeys );
            bool const isShift = ( ( allKeys[ VK_SHIFT ] & 0x80 ) != 0 );
            bool const isCtrl = ( ( allKeys[ VK_CONTROL ] & 0x80 ) != 0 );

            // Handle unicode and deadkeys in a way that works since Windows 95 and nt4.0
            // Using ToUnicode instead would be shorter, but would to my knowledge not run on 95 and 98.
            UINT32 unicode = 0;
            wchar_t singleChar = 0;
            WORD keyChars[ 2 ];
            UINT scanCode = HIWORD( lParam );
            int conversionResult = ::ToAsciiEx( UINT(wParam),
                                              scanCode,
                                              allKeys,
                                              keyChars,
                                              0,
                                              glwin->m_KEYBOARD_INPUT_HKL );
            if (conversionResult == 1)
            {
               WORD unicodeChar;
               ::MultiByteToWideChar( glwin->m_KEYBOARD_INPUT_CODEPAGE,
                                      MB_PRECOMPOSED, // default
                                      reinterpret_cast<LPCSTR>(keyChars),
                                      sizeof( keyChars ),
                                      reinterpret_cast<WCHAR*>(&unicodeChar),
                                      1 );
               singleChar = unicodeChar;
               unicode = unicodeChar;
            }

            KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( SEvent( keyReleaseEvent ) );
            glwin->getEventReceiver()->keyReleaseEvent( keyReleaseEvent );
         }
         return 0;
//         if (message == WM_SYSKEYUP)
//            return DefWindowProc(hwnd, message, wParam, lParam);
//         else
//            return 0;
      }

   case WM_SYSCOMMAND:
//      {
//         if ( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
//              ( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
//              ( wParam & 0xFFF0 ) == SC_KEYMENU )
//         {
//            return 0; // prevent screensaver or monitor powersave mode from starting
//         }
//      }
//
      break;
   case WM_USER:
      //event.UserEvent.UserData1 = (irr::s32)wParam;
      //event.UserEvent.UserData2 = (irr::s32)lParam;
      return 0;
   case WM_SETCURSOR:
#if 0
   dev = getDeviceFromHWnd(hWnd); // because Windows forgot about that in the meantime
      if (dev)
      {
         dev->getCursorControl()->setActiveIcon( dev->getCursorControl()->getActiveIcon() );
         dev->getCursorControl()->setVisible( dev->getCursorControl()->isVisible() );
      }
#endif
   break;
   case WM_COMMAND:
   {
/*
      switch( wParam )
      {
      case ID_FILE_EXIT:
         {
            DestroyWindow( hwnd );
         }
         break;
      case ID_FILE_LOAD:
         {
            std::string uri = dbOpenFileA();
         }
         break;
      case ID_FILE_SAVE:
         {
         }
         break;
      case ID_HELP_ABOUT:
         {
            // doModalAboutDialog(m_hWindow);
         }
         break;
      default:
         break;
      }
*/
   } break;

   case WM_SETFOCUS:
      return 0;

   default:
      break;
   }

   return DefWindowProc(hwnd, message, wParam, lParam);
}



// [Keyboard] Convert Benni's EKEY to Windows VK_Key [ used in getKeyState() ]

int
convert_EKEY_to_WinVK( EKEY ekey )
{
   switch ( ekey )
   {
      case KEY_0: return 48; // No VK enums for these? Are they all language dependent aswell?
      case KEY_1: return 49;
      case KEY_2: return 50;
      case KEY_3: return 51;
      case KEY_4: return 52;
      case KEY_5: return 53;
      case KEY_6: return 54;
      case KEY_7: return 55;
      case KEY_8: return 56;
      case KEY_9: return 57;

      case KEY_A: return 65; // No VK enums for these? Are they all language dependent aswell?
      case KEY_B: return 66;
      case KEY_C: return 67;
      case KEY_D: return 68;
      case KEY_E: return 69;
      case KEY_F: return 70;
      case KEY_G: return 71;
      case KEY_H: return 72;
      case KEY_I: return 73;
      case KEY_J: return 74;
      case KEY_K: return 75;
      case KEY_L: return 76;
      case KEY_M: return 77;
      case KEY_N: return 78;
      case KEY_O: return 79;
      case KEY_P: return 80;
      case KEY_Q: return 81;
      case KEY_R: return 82;
      case KEY_S: return 83;
      case KEY_T: return 84;
      case KEY_U: return 85;
      case KEY_V: return 86;
      case KEY_W: return 87;
      case KEY_X: return 88;
      case KEY_Y: return 89;
      case KEY_Z: return 90;

      case KEY_F1: return VK_F1;
      case KEY_F2: return VK_F2;
      case KEY_F3: return VK_F3;
      case KEY_F4: return VK_F4;
      case KEY_F5: return VK_F5;
      case KEY_F6: return VK_F6;
      case KEY_F7: return VK_F7;
      case KEY_F8: return VK_F8;
      case KEY_F9: return VK_F9;
      case KEY_F10: return VK_F10;
      case KEY_F11: return VK_F11;
      case KEY_F12: return VK_F12;
      case KEY_F13: return VK_F13;
      case KEY_F14: return VK_F14;
      case KEY_F15: return VK_F15;
      case KEY_F16: return VK_F16;
      case KEY_F17: return VK_F17;
      case KEY_F18: return VK_F18;
      case KEY_F19: return VK_F19;
      case KEY_F20: return VK_F20;
      case KEY_F21: return VK_F21;
      case KEY_F22: return VK_F22;
      case KEY_F23: return VK_F23;
      case KEY_F24: return VK_F24;

      case KEY_ESCAPE: return VK_ESCAPE;
      case KEY_ENTER: return VK_RETURN;
      case KEY_BACKSPACE: return VK_BACK;
      case KEY_SPACE: return VK_SPACE;
      case KEY_TAB: return VK_TAB;
      case KEY_LEFT_SHIFT: return VK_LSHIFT;
      case KEY_RIGHT_SHIFT: return VK_RSHIFT;

      case KEY_LEFT_SUPER: return VK_LWIN;
      case KEY_RIGHT_SUPER: return VK_RWIN;
      case KEY_LEFT_CTRL: return VK_LCONTROL;
      case KEY_RIGHT_CTRL: return VK_RCONTROL;

      case KEY_LEFT_ALT: return VK_LMENU;
      case KEY_RIGHT_ALT: return VK_RMENU;

      // OEM special keys: All German Umlaute so far and special characters
      // I dont own US or Japanese keyboards. Please add your special chars if needed.
      case KEY_OEM_1: return VK_OEM_1;            // Ü, VK_OEM_1 = 186 = 0xBA
      case KEY_OEM_PLUS: return VK_OEM_PLUS;      // VK_OEM_PLUS = 187 = 0xBB
      case KEY_OEM_COMMA: return VK_OEM_COMMA;    // VK_OEM_COMMA = 188 = 0xBC
      case KEY_OEM_MINUS: return VK_OEM_MINUS;    // VK_OEM_MINUS = 189 = 0xBD
      case KEY_OEM_PERIOD: return VK_OEM_PERIOD;  // VK_OEM_PERIOD = 190 = 0xBE
      case KEY_OEM_2: return VK_OEM_2;            // VK_OEM_2 = 191 = 0xBF
      case KEY_OEM_3: return VK_OEM_3;            // Ö, VK_OEM_3 = 192 = 0xC0
      case KEY_OEM_4: return VK_OEM_4;            // ß, VK_OEM_4 = 219 = 0xDB
      case KEY_OEM_5: return VK_OEM_5;            // VK_OEM_5 = 220 = 0xDC
      case KEY_OEM_6: return VK_OEM_6;            // VK_OEM_6 = 221 = 0xDD
      case KEY_OEM_7: return VK_OEM_7;            // Ä, VK_OEM_7 = 222 = 0xDE
      case KEY_OEM_102: return VK_OEM_102;        // <|>, VK_OEM_8 = 226 = 0xDE

      // 4x Arrow keys:
      case KEY_UP: return VK_UP;
      case KEY_LEFT: return VK_LEFT;
      case KEY_DOWN: return VK_DOWN;
      case KEY_RIGHT: return VK_RIGHT;

      // 3x control buttons:
      case KEY_SNAPSHOT: return VK_SNAPSHOT; // PRINT SCREEN, not the PRINT (only) key
      case KEY_SCROLL_LOCK: return VK_SCROLL;
      case KEY_PAUSE: return VK_PAUSE;

      // 6x control buttons:
      case KEY_INSERT: return VK_INSERT;
      case KEY_HOME: return VK_HOME;
      case KEY_END: return VK_END;
      case KEY_DELETE: return VK_DELETE;
      case KEY_PAGE_UP: return VK_PRIOR;
      case KEY_PAGE_DOWN: return VK_NEXT;

      // Numpad:
      case KEY_NUM_LOCK: return VK_NUMLOCK;
      case KEY_KP_0: return VK_NUMPAD0;
      case KEY_KP_1: return VK_NUMPAD1;
      case KEY_KP_2: return VK_NUMPAD2;
      case KEY_KP_3: return VK_NUMPAD3;
      case KEY_KP_4: return VK_NUMPAD4;
      case KEY_KP_5: return VK_NUMPAD5;
      case KEY_KP_6: return VK_NUMPAD6;
      case KEY_KP_7: return VK_NUMPAD7;
      case KEY_KP_8: return VK_NUMPAD8;
      case KEY_KP_9: return VK_NUMPAD9;

      case KEY_KP_MULTIPLY: return VK_MULTIPLY; // Is that correct mapping?
      case KEY_KP_ADD: return VK_ADD; // Is that correct mapping?
      //case KEY_KP_SEPARATOR: return VK_SEPARATOR; // Is that correct mapping?
      case KEY_KP_SUBTRACT: return VK_SUBTRACT; // Is that correct mapping?
      case KEY_KP_DECIMAL: return VK_DECIMAL; // Is that correct mapping?
      case KEY_KP_DIVIDE: return VK_DIVIDE; // Is that correct mapping?

      default:
      {
         #ifdef BENNI_USE_COUT
         std::cout << "Cant translate EKEY(" << EKEY_to_String(ekey) << ")" << std::endl;
         #endif
         return 0;
      }
   }
}

bool
DarkWindow_WGL::getKeyState( EKEY ekey ) const
{
   int winKey = convert_EKEY_to_WinVK( ekey );
   return 0 != GetKeyState( winKey );
}

int
DarkWindow_WGL::getDesktopWidth() const
{
   return GetSystemMetrics( SM_CXSCREEN );
}

int
DarkWindow_WGL::getDesktopHeight() const
{
   return GetSystemMetrics( SM_CYSCREEN );
}

int
DarkWindow_WGL::getWindowWidth() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.right - r_window.left;
}

int
DarkWindow_WGL::getWindowHeight() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.bottom - r_window.top;
}

int
DarkWindow_WGL::getWindowPosX() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.left;
}

int
DarkWindow_WGL::getWindowPosY() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.top;
}

int
DarkWindow_WGL::getClientWidth() const
{
   return m_screenWidth;
}

int
DarkWindow_WGL::getClientHeight() const
{
   return m_screenHeight;
}

void
DarkWindow_WGL::setWindowTitle( char const* title )
{
   // SendMessage instead of SetText for cases where HWND was created in a different thread
   DWORD_PTR dwResult;
   SendMessageTimeoutA(
      m_hWnd,
      WM_SETTEXT,
      0,
      reinterpret_cast< LPARAM >( title ),
      SMTO_ABORTIFHUNG,
      2000,
      &dwResult
   );
}

void
DarkWindow_WGL::setWindowIcon( int iRessourceID )
{
   if ( !m_hWnd )
   {
      return;
   }

   if ( m_hInstance == nullptr )
   {
      m_hInstance = GetModuleHandle( nullptr );
   }

   HICON hIcon = LoadIcon( m_hInstance, MAKEINTRESOURCE(iRessourceID) );
   if ( hIcon )
   {
      LONG_PTR ptr = reinterpret_cast<LONG_PTR>( hIcon );
      SetClassLongPtr( m_hWnd, GCLP_HICON, ptr );
      SetClassLongPtr( m_hWnd, GCLP_HICONSM, ptr );
   }
}

void
DarkWindow_WGL::setResizable( bool resizable )
{
   if ( !m_hWnd )
   {
      return;
   }

   int screenWidth = getClientWidth();    // TODO: Change GetClientRect() to m_screenWidth;
   int screenHeight = getClientHeight();  // TODO: Change GetClientRect() to m_screenHeight;

   DWORD style = m_windowedStyle;

   if (resizable)
   {
      style |= DWORD(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
   }
   else
   {
      style &= ~DWORD(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
   }

   // Should be ok to set it here
   m_windowedStyle = style;
   m_params.isResizable = resizable;

   // If we are in fullscreen mode, abort now...
   // We changed the window mode style and wait now for the user to call setFullscreen(false).
   if ( m_params.isFullscreen )
   {
      return;
   }

   // TODO: Separate this code into a reusable function setWindowStyle().
   if ( !SetWindowLongPtr( m_hWnd, GWL_STYLE, LONG_PTR(style) ) )
   {
      printf("Cant change window style.\n");
      return;
   }

   m_windowStyle = style; // Update 'current style' only on Success

   // This part is needed, or Window will freeze in the background.
   RECT r_window;
   r_window.top = 0;
   r_window.left = 0;
   r_window.right = screenWidth;
   r_window.bottom = screenHeight;

   AdjustWindowRect( &r_window, m_windowStyle, FALSE );

   int winW = r_window.right - r_window.left;
   int winH = r_window.bottom - r_window.top;

   int winX = (GetSystemMetrics(SM_CXSCREEN) - winW) / 2;
   int winY = (GetSystemMetrics(SM_CYSCREEN) - winH) / 2;

   winX = std::max( winX, 0 );
   winY = std::max( winY, 0 );

   SetWindowPos( m_hWnd, HWND_TOP, winX, winY, winW, winH,
                  SWP_FRAMECHANGED | SWP_SHOWWINDOW); //  | SWP_NOMOVE
}

void DarkWindow_WGL::setFullScreen( bool fullscreen )
{
   DWORD style;

   if ( fullscreen )
   {
      style = m_fullscreenStyle;
   }
   else
   {
      style = m_windowedStyle;
   }

   // TODO: Separate this code into a reusable function setWindowStyle().
   if ( !SetWindowLongPtr( m_hWnd, GWL_STYLE, LONG_PTR(style) ) )
   {
      printf("Cant change window style.\n");
      //std::cout << "Could not change window style." << std::endl;
   }

   m_params.isFullscreen = fullscreen;
   m_windowStyle = style; // Update 'current style' only on Success

   int const desktopWidth = GetSystemMetrics(SM_CXSCREEN);
   int const desktopHeight = GetSystemMetrics(SM_CYSCREEN);

   // This part is needed, or Window will freeze in the background.
   RECT r_window;

   if (m_params.isFullscreen)
   {
      r_window.top = 0;
      r_window.left = 0;
      r_window.right = desktopWidth;
      r_window.bottom = desktopHeight;
   }
   else
   {
      r_window.top = 0;
      r_window.left = 0;
      r_window.right = m_params.width;
      r_window.bottom = m_params.height;
   }

   AdjustWindowRect( &r_window, m_windowStyle, FALSE );

   int winW = r_window.right - r_window.left;
   int winH = r_window.bottom - r_window.top;
   int winX = (desktopWidth - winW) / 2;
   int winY = (desktopHeight - winH) / 2;

   if ( m_params.isFullscreen )
   {
      winX = 0;
      winY = 0;
   }
   else
   {
      winX = std::max( winX, 0 ); // Thick Border
      winY = std::max( winY, 0 );
   }
   SetWindowPos( m_hWnd, HWND_TOP, winX, winY, winW, winH,
                  SWP_FRAMECHANGED | SWP_SHOWWINDOW); //  | SWP_NOMOVE
}


//! runs the device. Returns false if device wants to be deleted
bool DarkWindow_WGL::run()
{
   //os::Timer::tick();

   //static_cast<CCursorControl*>(CursorControl)->update();

   //handleSystemMessages();

/*
   MSG msg;
   while (GetMessage (&msg, nullptr, 0, 0))
   {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
      // glwin.beginScene();
      // glwin.endScene();
   }
*/

   MSG msg;

   while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
   {
      // No message translation because we don't use WM_CHAR and it would conflict with our
      // deadkey handling.

      //if (ExternalWindow && msg.hwnd == m_hWnd)
      //   WndProc(HWnd, msg.message, msg.wParam, msg.lParam);
      //else
         DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
      {
         m_shouldRun = false;
      }
   }

   //if (!Close)
   //   resizeIfNecessary();

   //if(!Close && JoyControl)
   //   JoyControl->pollJoysticks();

   //_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
   return m_shouldRun;
}


/*

//! runs the device. Returns false if device wants to be deleted
bool CIrrDeviceWin32::run()
{
   os::Timer::tick();

   static_cast<CCursorControl*>(CursorControl)->update();

   handleSystemMessages();

   if (!Close)
      resizeIfNecessary();

   if(!Close && JoyControl)
      JoyControl->pollJoysticks();

   _IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
   return !Close;
}

//! Process system events
void CIrrDeviceWin32::handleSystemMessages()
{
   MSG msg;

   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
   {
      // No message translation because we don't use WM_CHAR and it would conflict with our
      // deadkey handling.

      if (ExternalWindow && msg.hwnd == HWnd)
         WndProc(HWnd, msg.message, msg.wParam, msg.lParam);
      else
         DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
         Close = true;
   }
}

//! Remove all messages pending in the system message loop
void CIrrDeviceWin32::clearSystemMessages()
{
   MSG msg;
   while (PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
   {}
   while (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
   {}
}

//! Minimizes the window.
void CIrrDeviceWin32::minimizeWindow()
{
   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(HWnd, &wndpl);
   wndpl.showCmd = SW_SHOWMINNOACTIVE;
   SetWindowPlacement(HWnd, &wndpl);
}


//! Maximizes the window.
void CIrrDeviceWin32::maximizeWindow()
{
   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(HWnd, &wndpl);
   wndpl.showCmd = SW_SHOWMAXIMIZED;
   SetWindowPlacement(HWnd, &wndpl);
}

//! Restores the window to its original size.
void CIrrDeviceWin32::restoreWindow()
{
   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(HWnd, &wndpl);
   wndpl.showCmd = SW_SHOWNORMAL;
   SetWindowPlacement(HWnd, &wndpl);
}

*/

// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
uint32_t
convertLocaleIdToCodepage( uint32_t localeId )
{
   switch ( localeId )
   {
      case 1098:  // Telugu
      case 1095:  // Gujarati
      case 1094:  // Punjabi
      case 1103:  // Sanskrit
      case 1111:  // Konkani
      case 1114:  // Syriac
      case 1099:  // Kannada
      case 1102:  // Marathi
      case 1125:  // Divehi
      case 1067:  // Armenian
      case 1081:  // Hindi
      case 1079:  // Georgian
      case 1097:  // Tamil
         return 0;
      case 1054:  // Thai
         return 874;
      case 1041:  // Japanese
         return 932;
      case 2052:  // Chinese (PRC)
      case 4100:  // Chinese (Singapore)
         return 936;
      case 1042:  // Korean
         return 949;
      case 5124:  // Chinese (Macau S.A.R.)
      case 3076:  // Chinese (Hong Kong S.A.R.)
      case 1028:  // Chinese (Taiwan)
         return 950;
      case 1048:  // Romanian
      case 1060:  // Slovenian
      case 1038:  // Hungarian
      case 1051:  // Slovak
      case 1045:  // Polish
      case 1052:  // Albanian
      case 2074:  // Serbian (Latin)
      case 1050:  // Croatian
      case 1029:  // Czech
         return 1250;
      case 1104:  // Mongolian (Cyrillic)
      case 1071:  // FYRO Macedonian
      case 2115:  // Uzbek (Cyrillic)
      case 1058:  // Ukrainian
      case 2092:  // Azeri (Cyrillic)
      case 1092:  // Tatar
      case 1087:  // Kazakh
      case 1059:  // Belarusian
      case 1088:  // Kyrgyz (Cyrillic)
      case 1026:  // Bulgarian
      case 3098:  // Serbian (Cyrillic)
      case 1049:  // Russian
         return 1251;
      case 8201:  // English (Jamaica)
      case 3084:  // French (Canada)
      case 1036:  // French (France)
      case 5132:  // French (Luxembourg)
      case 5129:  // English (New Zealand)
      case 6153:  // English (Ireland)
      case 1043:  // Dutch (Netherlands)
      case 9225:  // English (Caribbean)
      case 4108:  // French (Switzerland)
      case 4105:  // English (Canada)
      case 1110:  // Galician
      case 10249:  // English (Belize)
      case 3079:  // German (Austria)
      case 6156:  // French (Monaco)
      case 12297:  // English (Zimbabwe)
      case 1069:  // Basque
      case 2067:  // Dutch (Belgium)
      case 2060:  // French (Belgium)
      case 1035:  // Finnish
      case 1080:  // Faroese
      case 1031:  // German (Germany)
      case 3081:  // English (Australia)
      case 1033:  // English (United States)
      case 2057:  // English (United Kingdom)
      case 1027:  // Catalan
      case 11273:  // English (Trinidad)
      case 7177:  // English (South Africa)
      case 1030:  // Danish
      case 13321:  // English (Philippines)
      case 15370:  // Spanish (Paraguay)
      case 9226:  // Spanish (Colombia)
      case 5130:  // Spanish (Costa Rica)
      case 7178:  // Spanish (Dominican Republic)
      case 12298:  // Spanish (Ecuador)
      case 17418:  // Spanish (El Salvador)
      case 4106:  // Spanish (Guatemala)
      case 18442:  // Spanish (Honduras)
      case 3082:  // Spanish (International Sort)
      case 13322:  // Spanish (Chile)
      case 19466:  // Spanish (Nicaragua)
      case 2058:  // Spanish (Mexico)
      case 10250:  // Spanish (Peru)
      case 20490:  // Spanish (Puerto Rico)
      case 1034:  // Spanish (Traditional Sort)
      case 14346:  // Spanish (Uruguay)
      case 8202:  // Spanish (Venezuela)
      case 1089:  // Swahili
      case 1053:  // Swedish
      case 2077:  // Swedish (Finland)
      case 5127:  // German (Liechtenstein)
      case 1078:  // Afrikaans
      case 6154:  // Spanish (Panama)
      case 4103:  // German (Luxembourg)
      case 16394:  // Spanish (Bolivia)
      case 2055:  // German (Switzerland)
      case 1039:  // Icelandic
      case 1057:  // Indonesian
      case 1040:  // Italian (Italy)
      case 2064:  // Italian (Switzerland)
      case 2068:  // Norwegian (Nynorsk)
      case 11274:  // Spanish (Argentina)
      case 1046:  // Portuguese (Brazil)
      case 1044:  // Norwegian (Bokmal)
      case 1086:  // Malay (Malaysia)
      case 2110:  // Malay (Brunei Darussalam)
      case 2070:  // Portuguese (Portugal)
         return 1252;
      case 1032:  // Greek
         return 1253;
      case 1091:  // Uzbek (Latin)
      case 1068:  // Azeri (Latin)
      case 1055:  // Turkish
         return 1254;
      case 1037:  // Hebrew
         return 1255;
      case 5121:  // Arabic (Algeria)
      case 15361:  // Arabic (Bahrain)
      case 9217:  // Arabic (Yemen)
      case 3073:  // Arabic (Egypt)
      case 2049:  // Arabic (Iraq)
      case 11265:  // Arabic (Jordan)
      case 13313:  // Arabic (Kuwait)
      case 12289:  // Arabic (Lebanon)
      case 4097:  // Arabic (Libya)
      case 6145:  // Arabic (Morocco)
      case 8193:  // Arabic (Oman)
      case 16385:  // Arabic (Qatar)
      case 1025:  // Arabic (Saudi Arabia)
      case 10241:  // Arabic (Syria)
      case 14337:  // Arabic (U.A.E.)
      case 1065:  // Farsi
      case 1056:  // Urdu
      case 7169:  // Arabic (Tunisia)
         return 1256;
      case 1061:  // Estonian
      case 1062:  // Latvian
      case 1063:  // Lithuanian
         return 1257;
      case 1066:  // Vietnamese
         return 1258;
   }
   return 65001;   // utf-8
}


} // end namespace DarkGDK.
