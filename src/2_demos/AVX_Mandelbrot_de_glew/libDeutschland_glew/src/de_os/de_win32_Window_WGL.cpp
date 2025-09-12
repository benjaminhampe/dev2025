#include <de_os/de_win32_Window_WGL.h>
#include <de_core/de_RuntimeError.h>
#include <de_opengl.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <GL/wglew.h>
    #include <GL/wglext.h>
#else
    #define APIENTRY
#endif

#include <de_opengl.h>

namespace de {

IWindow* createWindow_WGL( CreateParams const & params )
{
   auto window = new Window_WGL();
   if ( !window->create( params ) )
   {
      delete window;
      DE_RUNTIME_ERROR(params.toString(),"Cant create WGL window, abort app!");
      return nullptr;
   }
   else
   {
      return window;
   }
}

LRESULT CALLBACK
Window_WGL_WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

Window_WGL::Window_WGL()
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

Window_WGL::~Window_WGL()
{
   destroy();
}

void Window_WGL::yield( int ms )
{
   if ( ms < 1 ) ms = 1;
   if ( ms > 10000 ) ms = 10000;
   Sleep( DWORD(ms) );
}

void
Window_WGL::setEventReceiver( de::IEventReceiver* receiver )
{
   m_eventReceiver = receiver;
}

de::IEventReceiver*
Window_WGL::getEventReceiver()
{
   return m_eventReceiver;
}

void
Window_WGL::requestClose()
{
   if ( m_hWnd )
   {
      PostMessage( m_hWnd, WM_QUIT, 0, 0 );
   }
}

void
Window_WGL::swapBuffers()
{
   //wglSwapIntervalEXT()

   // TODO: rework this. Seems like enabled vsync = 1 leads to 30 fps, not 60
   //       which should mean we swapped one time too often.
   //       Do we need SwapBuffers at all in combi with DwmFlush ?
   //       Should it rather be only one of them, not both at once?

   // TODO: Do we need glFlush() at all for modern GL ( non fixed pipeline )???
   // glFlush();

#if 0
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

#endif
   bool ok = SwapBuffers( m_hDC ) == TRUE;
}



void
Window_WGL::destroy()
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
Window_WGL::create( CreateParams params )
{
   if ( params.receiver )
   {
      m_eventReceiver = params.receiver;
   }

   m_params = params;
   m_hInstance = GetModuleHandle( nullptr );
   std::string className = "de_Window_WGL";
   std::string dummyName = "de_Window_WGL_dummy2";
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
   wcex.lpfnWndProc  = Window_WGL_WndProc;

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
                           "Window_WGL",
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


   wglewInit();


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

      float fAttributes[] = {0.0f, 0.0f};

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

   int32_t m_shaderVersionMajor = 0;
   int32_t m_shaderVersionMinor = 0;
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
   float pointSize[2] { 0,0 };
   float pointSizeRange[4] { 0,0,0,0 };
   glGetFloatv( GL_POINT_SIZE, pointSize );
   glGetFloatv( GL_POINT_SIZE_RANGE, pointSizeRange );

   float lineWidth[4] { 0,0,0,0 };
   float lineWidthRange[4] { 0,0,0,0 };
   float lineWidthGranularity[4] { 0,0,0,0 };
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
      int32_t samples[4] { 0,0,0,0 };
      glGetIntegerv( GL_MULTISAMPLE, samples );
      float mlwRange[4] { 0,0,0,0 };
      glGetFloatv( GL_MULTISAMPLE_LINE_WIDTH_RANGE, mlwRange );
      float mlwGranularity[4] { 0,0,0,0 };
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

LRESULT CALLBACK
Window_WGL_WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
   Window_WGL* glwin = nullptr;
   if ( message == WM_NCCREATE )
   {
      glwin = static_cast< Window_WGL* >(
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
      glwin = reinterpret_cast< Window_WGL* >( GetWindowLongPtr( hwnd, GWL_USERDATA ) );
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
               de::WindowMoveEvent windowMoveEvent;
               windowMoveEvent.x = int( LOWORD( lParam ) );
               windowMoveEvent.y = int( HIWORD( lParam ) );
               //std::lock_guard< std::mutex > guard( de::os::win32::s_Mutex );
               //glwin->getEventReceiver()->onEvent( de::SEvent( windowMoveEvent ) );
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
         o << "Window_WGL (c) 2023 by BenjaminHampe@gmx.de | "
              "Desktop("<<dw<<","<<dh<<"), Screen("<<w<<","<<h<<")";
         setWindowTitle( hwnd, o.str().c_str() );
         */
         if ( glwin )
         {
            glwin->m_screenWidth = w;
            glwin->m_screenHeight = h;

            if ( glwin->getEventReceiver() )
            {
               de::ResizeEvent resizeEvent;
               resizeEvent.w = w;
               resizeEvent.h = h;
               //std::lock_guard< std::mutex > guard( de::os::win32::s_Mutex );
               //glwin->m_eventReceiver->onEvent( de::SEvent( resizeEvent ) );
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
               de::MouseMoveEvent mouseMoveEvent;
               mouseMoveEvent.x = mx;
               mouseMoveEvent.y = my;
               //std::lock_guard< std::mutex > guard( de::os::win32::s_Mutex );
               //glwin->m_eventReceiver->onEvent( de::SEvent( mouseMoveEvent ) );
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
               de::MouseWheelEvent mouseWheelEvent;
               mouseWheelEvent.x = 0.0f;
               mouseWheelEvent.y = mouseWheelY;
               //std::lock_guard< std::mutex > guard( de::os::win32::s_Mutex );
               glwin->getEventReceiver()->mouseWheelEvent( mouseWheelEvent );
            }
         }
         return 0;
      }

      case WM_LBUTTONDOWN: {
         if ( glwin->getEventReceiver() )
         {
            de::MousePressEvent mousePressEvent;
            mousePressEvent.x = LOWORD(lParam);
            mousePressEvent.y = HIWORD(lParam);
            mousePressEvent.buttons = de::MouseButton::Left;
            mousePressEvent.flags = 0;
            //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( de::SEvent( mousePressEvent ) );
            glwin->getEventReceiver()->mousePressEvent( mousePressEvent );
         }
         return 0;
      }
      case WM_RBUTTONDOWN: {
         if ( glwin->getEventReceiver() )
         {
            de::MousePressEvent mousePressEvent;
            mousePressEvent.x = LOWORD(lParam);
            mousePressEvent.y = HIWORD(lParam);
            mousePressEvent.buttons = de::MouseButton::Right;
            mousePressEvent.flags = 0;
            //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( de::SEvent( mousePressEvent ) );
            glwin->getEventReceiver()->mousePressEvent( mousePressEvent );
         }
         return 0;
      }
      case WM_MBUTTONDOWN: {
         if ( glwin->getEventReceiver() )
         {
            de::MousePressEvent mousePressEvent;
            mousePressEvent.x = LOWORD(lParam);
            mousePressEvent.y = HIWORD(lParam);
            mousePressEvent.buttons = de::MouseButton::Middle;
            mousePressEvent.flags = 0;
            //mousePressEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mousePressEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( de::SEvent( mousePressEvent ) );
            glwin->getEventReceiver()->mousePressEvent( mousePressEvent );
         }
         return 0;
      }

      case WM_LBUTTONUP: {
         if ( glwin->getEventReceiver() )
         {
            de::MouseReleaseEvent mouseReleaseEvent;
            mouseReleaseEvent.x = LOWORD(lParam);
            mouseReleaseEvent.y = HIWORD(lParam);
            mouseReleaseEvent.buttons = de::MouseButton::Left;
            mouseReleaseEvent.flags = 0;
            //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( de::SEvent( mouseReleaseEvent ) );
            glwin->getEventReceiver()->mouseReleaseEvent( mouseReleaseEvent );
         }
         return 0;
      }
      case WM_RBUTTONUP: {
         if ( glwin->getEventReceiver() )
         {
            de::MouseReleaseEvent mouseReleaseEvent;
            mouseReleaseEvent.x = LOWORD(lParam);
            mouseReleaseEvent.y = HIWORD(lParam);
            mouseReleaseEvent.buttons = de::MouseButton::Right;
            mouseReleaseEvent.flags = 0;
            //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( de::SEvent( mouseReleaseEvent ) );
            glwin->getEventReceiver()->mouseReleaseEvent( mouseReleaseEvent );
         }
         return 0;
      }
      case WM_MBUTTONUP: {
         if ( glwin->getEventReceiver() )
         {
            de::MouseReleaseEvent mouseReleaseEvent;
            mouseReleaseEvent.x = LOWORD(lParam);
            mouseReleaseEvent.y = HIWORD(lParam);
            mouseReleaseEvent.buttons = de::MouseButton::Middle;
            mouseReleaseEvent.flags = 0;
            //mouseReleaseEvent.flags.Shift = ((LOWORD(wParam) & MK_SHIFT) != 0);
            //mouseReleaseEvent.flags.Control = ((LOWORD(wParam) & MK_CONTROL) != 0);
            //glwin->m_eventReceiver->onEvent( de::SEvent( mouseReleaseEvent ) );
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

            de::KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= de::KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= de::KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( de::SEvent( keyPressEvent ) );
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

            de::KeyPressEvent keyPressEvent;
            keyPressEvent.key = translateWinKey( UINT(wParam) );
            keyPressEvent.unicode = unicode;
            keyPressEvent.modifiers = 0;
            keyPressEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyPressEvent.modifiers |= de::KeyModifier::Shift;
            if ( isCtrl ) keyPressEvent.modifiers |= de::KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( de::SEvent( keyPressEvent ) );
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

            de::KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= de::KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= de::KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( de::SEvent( keyReleaseEvent ) );
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

            de::KeyReleaseEvent keyReleaseEvent;
            keyReleaseEvent.key = translateWinKey( UINT(wParam) );
            keyReleaseEvent.unicode = unicode;
            keyReleaseEvent.modifiers = 0;
            keyReleaseEvent.scancode = UINT(wParam); // scanCode;
            if ( isShift ) keyReleaseEvent.modifiers |= de::KeyModifier::Shift;
            if ( isCtrl ) keyReleaseEvent.modifiers |= de::KeyModifier::Ctrl;

            //glwin->m_eventReceiver->onEvent( de::SEvent( keyReleaseEvent ) );
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
      //event.UserEvent.UserData1 = (irr::int32_t)wParam;
      //event.UserEvent.UserData2 = (irr::int32_t)lParam;
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

bool
Window_WGL::getKeyState( de::EKEY ekey ) const
{
   int winKey = convert_EKEY_to_WinVK( ekey );
   return 0 != GetKeyState( winKey );
}

int
Window_WGL::getDesktopWidth() const
{
   return GetSystemMetrics( SM_CXSCREEN );
}

int
Window_WGL::getDesktopHeight() const
{
   return GetSystemMetrics( SM_CYSCREEN );
}

int
Window_WGL::getWindowWidth() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.right - r_window.left;
}

int
Window_WGL::getWindowHeight() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.bottom - r_window.top;
}

int
Window_WGL::getWindowPosX() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.left;
}

int
Window_WGL::getWindowPosY() const
{
   RECT r_window;
   GetWindowRect( m_hWnd, &r_window );
   return r_window.top;
}

int
Window_WGL::getClientWidth() const
{
   return m_screenWidth;
}

int
Window_WGL::getClientHeight() const
{
   return m_screenHeight;
}

void
Window_WGL::setWindowTitle( char const* title )
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
Window_WGL::setWindowIcon( int iRessourceID )
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
Window_WGL::setResizable( bool resizable )
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

void Window_WGL::setFullScreen( bool fullscreen )
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
bool Window_WGL::run()
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

} // end namespace de
