#include <App_create.hpp>
#include <App_eventHandler.hpp>
#include <../res/resource.h>

void GCodeParserApp_onWMCreate( HWND hwnd )
{
   HINSTANCE hInstance = GetModuleHandle( nullptr );
   HICON hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(aaaa) );
   if ( hIcon )
   {
      SetClassLongPtr( hwnd, GCLP_HICON, (LONG_PTR)hIcon );
      SetClassLongPtr( hwnd, GCLP_HICONSM, (LONG_PTR)hIcon );
   }
   else
   {
      dbMessageW(L"Error, failed to load icon", L"aaaa");
   }

   HMENU hMenu = CreateMenu();
   HMENU hMenuFile = CreateMenu();
   HMENU hMenuHelp = CreateMenu();

   AppendMenu( hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuFile), TEXT("Start") );
   AppendMenu( hMenuFile, MF_STRING, ID_FILE_LOAD, TEXT("&Load file") );
   AppendMenu( hMenuFile, MF_STRING, ID_FILE_SAVE, TEXT("&Save file") );
   AppendMenu( hMenuFile, MF_SEPARATOR, 0, nullptr );
   AppendMenu( hMenuFile, MF_STRING, ID_FILE_EXIT, TEXT("&Exit application") );

   AppendMenu( hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hMenuHelp), TEXT("Help") );
   AppendMenu( hMenuHelp, MF_SEPARATOR, 0, nullptr );
   AppendMenu( hMenuHelp, MF_STRING, ID_HELP_ABOUT, TEXT("About") );

   SetMenu( hwnd, hMenu );

   //HINSTANCE hInstance = GetModuleHandle( nullptr );

   int w = g_app.m_clientWidth;
   int h = g_app.m_clientHeight;

   std::wstring txt = L"Hello World!\nHello Benni!\n";


   createStaticText(txt.c_str(),w - 20 - 300, 20, 300, 230, hwnd,1);

   createButton(L"Estimate duration",20, 50, 80, 25, hwnd,ID_BTN_ESTIMATE);
   createButton(L"Show 3D Simulation",20, 100, 80, 25, hwnd,ID_BTN_SHOW3D);
   createCheckBox(L"Show Logger",20, 150, 185, 35, hwnd,ID_BTN_SHOWLOGGER);

   g_app.m_edtFeedSpeed =
   createDemoEditBox( L"...", 20,250,250,32,hwnd,ID_EDT_FEEDSPEED );

//      g_app.m_comboBox = createComboBox(L"Combobox",20,700,400,110,hwnd,ID_CBX_UNITMODE);
//      for (int i = 0; i < 4; i++ )
//      {
//         SendMessageW(g_app.m_comboBox, CB_ADDSTRING, 0, (LPARAM) items[i]);
//      }


   HWND textArea = createDemoTextArea(L"TextArea1", w-20-460, 400, 460, 380, hwnd, 49000);

   SetWindowTextW( textArea, L"Hello TextAreaW\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n"
      L"Hello World\r\n");

   createDemoPanel( 300, 500, 200,400, hwnd, 49001 );
}



bool GCodeParserApp_createApp()
{
   int w = 1024; //dbDesktopWidth();
   int h = 768; //dbDesktopHeight();

   // Make DPI aware window because of stupid effects
   InitCommonControls();

   INITCOMMONCONTROLSEX icex;
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_LISTVIEW_CLASSES
               | ICC_TREEVIEW_CLASSES
               | ICC_BAR_CLASSES
               | ICC_TAB_CLASSES | ICC_UPDOWN_CLASS| ICC_PROGRESS_CLASS
               | ICC_HOTKEY_CLASS | ICC_ANIMATE_CLASS| ICC_WIN95_CLASSES
               | ICC_DATE_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES
               | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS
               | ICC_NATIVEFNTCTL_CLASS | ICC_STANDARD_CLASSES| ICC_LINK_CLASS;
   InitCommonControlsEx(&icex);

   HBRUSH MyBrush = CreateSolidBrush( RGB( 0, 150, 255 ) );

   //   IDC_APPSTARTING    Standard arrow and small hourglass
   //   IDC_ARROW          Standard arrow
   //   IDC_CROSS          Crosshair
   //   IDC_HAND           Windows NT 5.0 and later: Hand
   //   IDC_HELP           Arrow and question mark
   //   IDC_IBEAM          I-beam
   //   IDC_NO             Slashed circle
   //   IDC_SIZEALL        Four-pointed arrow pointing north, south, east, and west
   //   IDC_SIZENESW       Double-pointed arrow pointing northeast and southwest
   //   IDC_SIZENS         Double-pointed arrow pointing north and south
   //   IDC_SIZENWSE       Double-pointed arrow pointing northwest and southeast
   //   IDC_SIZEWE         Double-pointed arrow pointing west and east
   //   IDC_UPARROW        Vertical arrow
   //   IDC_WAIT           Hourglass
   auto cursor = LoadCursor( nullptr, IDC_ARROW );

   // prepare window class
   g_app.m_hInstance = GetModuleHandle( nullptr );
   DWORD dwWndStyle = WS_OVERLAPPEDWINDOW;
   DWORD dwExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
   TCHAR szClsName[64] = TEXT( "ArcadeSynth_win32api.class" );
   TCHAR szWndName[64] = TEXT( "ArcadeSynth_win32api.window" );
   WNDCLASS wc;
   wc.lpszClassName = szClsName;                // Set the name of the Class
   wc.lpfnWndProc = GCodeParserApp_eventHandler; //( WNDPROC )&SyncWindow::globalEventHandler;
   wc.hInstance = g_app.m_hInstance;                  // Use this module for the module handle
   wc.hCursor = cursor; // Pick the default mouse cursor
   wc.hIcon = LoadIcon( nullptr, IDI_WINLOGO );// Pick the default windows icons
   wc.hbrBackground = MyBrush;                       // No Background
   wc.lpszMenuName = nullptr;                       // No menu for this window
   wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;// set styles for this class, specifically to catch, window redraws, unique DC, and resize
   wc.cbClsExtra = 0;                          // Extra class memory
   wc.cbWndExtra = 0;                          // Extra window memory
   //wc.hIcon = HICON( LoadImage( g_app.m_hInstance,
   //                                   __TEXT("../../media/ArcadeSynth/MEDIA_200.ico"),
   //                                   IMAGE_ICON,
   //                                   0,
   //                                   0,
   //                                   LR_LOADFROMFILE ) )
   // register window class
   if ( !RegisterClass( &wc ) )
   {
      // DE_ERROR("RegisterClass failed.")
      return false;
   }


   // apply windows visuals
   ::ShowCursor( TRUE );
   RECT r;
   r.left = 0;
   r.top = 0;
   r.right = w;
   r.bottom = h;
   ::AdjustWindowRectEx( &r, dwWndStyle, FALSE, dwExtStyle );
   int visualX = r.left;
   int visualY = r.top;
   int visualW = r.right - r.left;
   int visualH = r.bottom - r.top;
   // DE_DEBUG("Adjusted visualRect(",visualX,",",visualY,",",visualW,",",visualH,")")

   // Init static, TODO: multithreading safety, statics are ugly due to init test with locks.
   // create window
   g_app.m_hWnd = CreateWindowEx(
            dwExtStyle, // Extended style
            szClsName,  // class name
            szWndName,  // window name
            dwWndStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // window style
            200,          // win pos x
            200,          // win pos y
            visualW,    // win visual width
            visualH,    // win visual height
            nullptr,    // desktop is parent window
            nullptr,    // menu
            g_app.m_hInstance,  // instance
            nullptr );  // accessible by GlobalWndProc.WM_NCCREATE: this((WM_CREATESTRUCT*)( LPARAM ))->lpCreateParams

   ShowWindow( g_app.m_hWnd, SW_SHOWDEFAULT );
   UpdateWindow( g_app.m_hWnd );

   // HDC hdc = GetDC( m_HWnd );
   // float g_DPIScaleX = float( GetDeviceCaps(hdc, LOGPIXELSX) ) / 96.0f;
   // float g_DPIScaleY = float( GetDeviceCaps(hdc, LOGPIXELSY) ) / 96.0f;
   // ReleaseDC( m_HWnd , hdc);
   // std::cout << "DPI scale is " << g_DPIScaleX << " x " << g_DPIScaleY << std::endl;
   return true;
}
