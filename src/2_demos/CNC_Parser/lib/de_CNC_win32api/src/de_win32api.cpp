#include <de_win32api.hpp>

#if 0

struct SimpleWindow
{
   HINSTANCE m_hInstance = nullptr;
   HWND m_hWnd = nullptr;
   int m_clientWidth = 0;
   int m_clientHeight = 0;
   int m_mouseX = 0;
   int m_mouseY = 0;
};

SimpleWindow g_app;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
//Deklaration der Windows-Nachrichten-Prozedur

int main( int argc, char* argv[] )
//int WINAPI WinMain (HINSTANCE hI,
//                    HINSTANCE hPrI,
//                    PSTR szCmdLine,
//                    int iCmdShow)
{
   int w = 1024; //dbDesktopWidth();
   int h = 768; //dbDesktopHeight();

   // Make DPI aware window because of stupid effects
   InitCommonControls();

/*
   char szName[] = "Fensterklasse";
   WNDCLASS wc;

   wc.style         = CS_HREDRAW | CS_VREDRAW;   // CS = "class style"
   wc.lpfnWndProc   = WndProc;
   wc.cbClsExtra    = 0;
   wc.cbWndExtra    = 0;
   wc.hInstance     = nullptr;
   wc.hIcon         = LoadIcon (nullptr, IDI_WINLOGO);
   wc.hCursor       = LoadCursor (nullptr, IDC_ARROW);
   wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject( BLACK_BRUSH ));
   wc.lpszMenuName  = nullptr;
   wc.lpszClassName = szName;

   RegisterClass (&wc);
*/

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
   wc.lpfnWndProc = WndProc; //( WNDPROC )&SyncWindow::globalEventHandler;
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
      return 0;
   }


   /*
      HWND hwnd = CreateWindow (szName, "", WS_SYSMENU | WS_THICKFRAME,
                                0, 0, 200, 100, NULL, NULL, hI, NULL);

      ShowWindow   (hwnd, SW_SHOWDEFAULT );
      UpdateWindow (hwnd);

      // Nachrichten-Schleife
      MSG msg;
      while (GetMessage (&msg, NULL, 0, 0))
      {
         TranslateMessage (&msg);
         DispatchMessage (&msg);
      }
      return msg.wParam;
   */


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

   // RECT r_curr;
   // GetClientRect( m_HWnd, &r_curr );
   // int rw = r_curr.right - r_curr.left;
   // int rh = r_curr.bottom - r_curr.top;
   // std::cout << "AsyncWindow.ClientRect = " << r_curr.left << "," << r_curr.top << ","
   // << rw << "," << rh << std::endl;


   // Nachrichten-Schleife
   MSG msg;
   while (GetMessage (&msg, nullptr, 0, 0))
   {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
   }

   // DE_DEBUG("Destroy AsyncWindow window")
   WINDOWINFO windowInfo;
   windowInfo.cbSize = sizeof( WINDOWINFO );
   ::GetWindowInfo( g_app.m_hWnd, &windowInfo );
   DestroyWindow( g_app.m_hWnd );
   g_app.m_hWnd = nullptr;
   UnregisterClass( reinterpret_cast< LPCTSTR >( windowInfo.atomWindowType ),
                    GetModuleHandle( nullptr ) );

   return int(msg.wParam);
}

inline INT_PTR CALLBACK
AboutDlgProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
   switch(Message)
   {
   case WM_INITDIALOG:
      return TRUE;
   case WM_COMMAND:
      switch(LOWORD(wParam))
      {
      case IDOK:
         EndDialog(hwnd, IDOK);
      break;
      case IDCANCEL:
         EndDialog(hwnd, IDCANCEL);
      break;
      }
      break;
   default:
      return FALSE;
   }
   return TRUE;
}

// Windows-Nachrichten-Prozedur
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

   switch (message)
   {
   case WM_CREATE:
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
         //MessageBoxW(hwnd, L"Failed to load icon", L"Error", MB_OK);
      }

     // g_app.m_hWnd = hwnd;

      //createMenus();
      HMENU hMenu = CreateMenu();
      HMENU hMenuFile = CreateMenu();
      HMENU hMenuHelp = CreateMenu();

      AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuFile, TEXT("Start") );
      AppendMenu( hMenuFile, MF_STRING, ID_FILE_LOAD, TEXT("&Load file") );
      AppendMenu( hMenuFile, MF_STRING, ID_FILE_SAVE, TEXT("&Save file") );
      AppendMenu( hMenuFile, MF_SEPARATOR, 0, 0 );
      AppendMenu( hMenuFile, MF_STRING, ID_FILE_EXIT, TEXT("&Exit application") );

      AppendMenu( hMenu, MF_POPUP, (UINT_PTR)hMenuHelp, TEXT("Help") );
      AppendMenu( hMenuHelp, MF_SEPARATOR, 0, 0 );
      AppendMenu( hMenuHelp, MF_STRING, ID_HELP_ABOUT, TEXT("About") );

      SetMenu( hwnd, hMenu );
   }
   case WM_COMMAND:
   {
      switch( wParam )
      {
      case ID_FILE_EXIT:
         {
            DestroyWindow( hwnd );
         }
         break;
      case ID_FILE_LOAD:
         {
            //std::wstring uri = loadFileNameDlgW();
            //loadFile( uri );
         }
         break;
      case ID_FILE_SAVE:
         {
            //std::wstring uri = saveFileNameDlgW();
            //saveFile( uri );
         }
         break;
      case ID_HELP_ABOUT:
         {
            //std::cout << "About" << std::endl;
            int ret = DialogBoxParam( GetModuleHandle(NULL),
                              MAKEINTRESOURCE(IDD_ABOUTDIALOG),
                              hwnd, AboutDlgProc, (LPARAM)0 );
            if ( ret == IDOK )
            {
         //      MessageBox( hwnd,
         //                  TEXT("Dialog exited with IDOK."),
         //                  TEXT("Notice"),
         //                  MB_OK | MB_ICONINFORMATION);
            }
            else if( ret == IDCANCEL )
            {
         //      MessageBox( hwnd,
         //                  TEXT("Dialog exited with IDCANCEL."),
         //                  TEXT("Notice"), MB_OK | MB_ICONINFORMATION);
            }
            else if( ret == -1 )
            {
         //      MessageBox( hwnd,
         //                  TEXT("Dialog failed!"),
         //                  TEXT("Error"), MB_OK | MB_ICONINFORMATION);
            }
         }
         break;
      default:
         break;
      }
   } break;
   case WM_SIZE:
      {
         int w = int( LOWORD( lParam ) );
         int h = int( HIWORD( lParam ) );
         int dw = GetSystemMetrics( SM_CXSCREEN );
         int dh = GetSystemMetrics( SM_CYSCREEN );

         g_app.m_clientWidth = w;
         g_app.m_clientHeight = h;

         std::ostringstream o;
         o << "Desktop("<<dw<<","<<dh<<"), Screen(" << w<<","<< h<<")";
         SetWindowText( g_app.m_hWnd, o.str().c_str() );
         return 0;
      }
   case WM_LBUTTONDOWN:
   {
      //      fwKeys Value of wParam. Various virtual keys are down.
      //      This parameter can be any combination of the following values:
      //      MK_CONTROL Set if the ctrl key is down.
      //      MK_LBUTTON Set if the left mouse button is down.
      //      MK_MBUTTON Set if the middle mouse button is down.
      //      MK_RBUTTON Set if the right mouse button is down.
      //      MK_SHIFT Set if the shift key is down.
      int fwKeys  =  wParam;                     // key flags
      int xPos      =  LOWORD(lParam);   // horizontal position of cursor
      int yPos      =  HIWORD(lParam);    // vertical position of cursor

//      HDC hdc = GetDC(hwnd);

//      std::ostringstream o;
//      o << hwnd << "," << message << "," << wParam << "," << lParam;
//      std::string s = o.str();
//      TextOut (hdc, xPos + 20, yPos + 20, s.c_str(), s.size());
//      ReleaseDC(hwnd,hdc);
      return 0;
   }
   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint (hwnd, &ps);

      int w = g_app.m_clientWidth;
      int h = g_app.m_clientHeight;

      //HBRUSH MyBrush = CreateSolidBrush( RGB( 0, 150, 255 ) );
      HPEN hPen0 = CreatePen(PS_SOLID, 1, RGB(0, 150, 255));
      HBRUSH hBrush0 = CreateSolidBrush(RGB(0, 150, 255));
      SelectObject(hdc, hPen0);
      SelectObject(hdc, hBrush0);
      Rectangle(hdc, 0, 0, w, h);
      DeleteObject(hPen0);
      DeleteObject(hBrush0);

      HPEN hPen2 = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
      SelectObject(hdc, hPen2);

      HBRUSH hBrush1 = CreateSolidBrush(RGB(121, 90, 0));
      SelectObject(hdc, hBrush1);
      Rectangle(hdc, 10, 10, 300, 300);

      DeleteObject(hPen2);
      DeleteObject(hBrush1);



/*
      int w = 256;
      int h = 256;

      std::vector< uint8_t > pixels( w * h * size_t( 4 ) );
      uint8_t* dst = pixels.data();

      int x = 0;
      int y = 0;
      int cw = 32;
      int ch = 32;
      for ( int j = 0; j < 2*ch; ++j )
      {
         for ( int i = 0; i < 2*cw; ++i )
         {
            *dst++ = 200; *dst++ = 200; *dst++ = 200; *dst++ = 255;
         }
      }

      for ( int j = 0; j < ch; ++j )
      {
         for ( int i = 0; i < cw; ++i )
         {
            *dst++ = 100; *dst++ = 100; *dst++ = 100; *dst++ = 255;
         }
      }

      for ( int j = c; j < ch; ++j )
      {
         for ( int i = 0; i < cw; ++i )
         {
            *dst++ = 100; *dst++ = 100; *dst++ = 100; *dst++ = 255;
         }
      }

      HBITMAP hBitmap = CreateBitmap( w, h, 4, 8, (LPBYTE)tmp.data() );

      HDC mem = CreateCompatibleDC( hdc );
      HBITMAP old = (HBITMAP)SelectObject( mem, hBitmap );

      BITMAP bm;
      GetObject( hBitmap, sizeof( bm ), &bm );
      BitBlt( dc, 0, 0, bm.bmWidth, bm.bmHeight, mem, 0, 0, SRCCOPY );
      SelectObject( mem, old );
      DeleteDC( mem );
*/
      SetTextColor( hdc, RGB( 255,    0, 0) );  // rot
      SetBkColor  ( hdc, RGB( 255, 255,  0) );  // gelb
      TextOut (hdc, 20, 20, "Ich bin ein Fenster.", 20);

//      int SetBkMode
//      (
//          HDC hdc,          // handle of device context
//          int iBkMode      // flag specifying background mode
//      );

//      durch Wahl des Hintergrund-Modus TRANSPARENT die Hintergrundfarbe ignorieren.
//      Zurückschalten können Sie mit OPAQUE.

      EndPaint (hwnd, &ps);
      return 0;
   }
   case WM_ERASEBKGND:
      return 0;
   case WM_INPUTLANGCHANGE:
   {
//            KEYBOARD_INPUT_HKL = GetKeyboardLayout( 0 ); // get the new codepage used for keyboard input
//            KEYBOARD_INPUT_CODEPAGE = convertLocaleIdToCodepage( LOWORD( KEYBOARD_INPUT_HKL ) );
      return 0;
   }
   case WM_SYSKEYDOWN:
      return 0; // handleKeyEvent( uMsg, wParam, lParam );
   case WM_SYSKEYUP:
      return 0; // handleKeyEvent( uMsg, wParam, lParam );
   case WM_KEYDOWN:
      return 0; // handleKeyEvent( uMsg, wParam, lParam );
   case WM_KEYUP:
      return 0; // handleKeyEvent( uMsg, wParam, lParam );
   case WM_SYSCOMMAND:
//      {
//         if ( ( wParam & 0xFFF0 ) == SC_SCREENSAVE ||
//              ( wParam & 0xFFF0 ) == SC_MONITORPOWER ||
//              ( wParam & 0xFFF0 ) == SC_KEYMENU )
//         {
//            return 0; // prevent screensaver or monitor powersave mode from starting
//         }
//      }
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
   case WM_SETFOCUS:
      return 0;
   case WM_DESTROY:
       PostQuitMessage (0);
       return 0;
   }

   return DefWindowProc (hwnd, message, wParam, lParam);
}

/*

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
   case WM_CREATE:
   {
      //in Frame's constructor:
      //HWND hWnd = GetHandle();
      WINDOWINFO wi = {0};
      GetWindowInfo( hwnd, &wi );
      HINSTANCE hInstance = nullptr;

      HICON hIcon = LoadIcon( wc.hInstance, L"mspaint_xp.ico" );
      if ( !hIcon )
      {
         MessageBoxW(hwnd, L"Failed to load icon", L"Error", MB_OK);
      }
      SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)hIcon);
   }

   case WM_PAINT:
      {

      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      // Draw Custom Brush (Bitmap)
      UINT bits[8] = {  0x111111ff,
                        0xffffffff,
                        0xffffffff,
                        0xffffffff,
                        0x00000000,
                        0x00000000,
                        0x00000000,
                        0x00000000 };
      HBITMAP hBtm = CreateBitmap(8, 8, 1, 1, (LPBYTE) bits);
      HBRUSH hCustomBrush = CreatePatternBrush(hBtm);
      HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hCustomBrush);
      SelectObject(hdc, GetStockObject(NULL_PEN));
      Rectangle(hdc, 20, 20, 250, 160);
      SelectObject(hdc, hOldBrush);
      DeleteObject(hCustomBrush);
      SelectObject(hdc, GetStockObject(BLACK_PEN));
      DeleteObject(hBtm);

//      HBITMAP hBitmap = (HBITMAP)LoadImageW(NULL, L"C:\\prog\\slovakia.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
//      if (hBitmap == NULL)
//      {
//         MessageBoxW(hwnd, L"Failed to load image", L"Error", MB_OK);
//      }
//      HDC hdcMem = CreateCompatibleDC(hdc);
//      HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
//      GetObject(hBitmap, sizeof(bitmap), &bitmap);
//      BitBlt(hdc, 5, 5, bitmap.bmWidth, bitmap.bmHeight,hdcMem, 0, 0, SRCCOPY);
//      SelectObject(hdcMem, oldBitmap);
//      DeleteDC(hdcMem);

      // Draw Shapes
      Ellipse(hdc, 30, 30, 120, 90);
      RoundRect(hdc, 150, 30, 240, 90, 15, 20);
      Chord(hdc, 270, 30, 360, 90, 270, 45, 360, 45);
      POINT const polygon[10] = { 30, 145, 85, 165, 105, 110, 65, 125, 30, 105 };
      Polygon(hdc, polygon, 5);
      Rectangle(hdc, 150, 110, 230, 160);

      // Draw Texts
      DWORD color = GetSysColor(COLOR_BTNFACE);
      SetBkColor(hdc, color);
      HFONT hFont = CreateFontW(15, 0,0,0, FW_MEDIUM, 0,0,0,0, 0,0,0,0, L"Georgia");
      HFONT holdFont = (HFONT)SelectObject(hdc, hFont);

      wchar_t *ver1 = L"Not marble, nor the gilded monuments";
      wchar_t *ver2 = L"Of princes, shall outlive this powerful rhyme;";
      wchar_t *ver3 = L"But you shall shine more bright in these contents";
      wchar_t *ver4 = L"Than unswept stone, besmear'd with sluttish time.";
      wchar_t *ver5 = L"When wasteful war shall statues overturn,";
      wchar_t *ver6 = L"And broils root out the work of masonry,";
      wchar_t *ver7 = L"Nor Mars his sword, nor war's quick fire shall burn";
      wchar_t *ver8 = L"The living record of your memory.";
      wchar_t *ver9 = L"'Gainst death, and all oblivious enmity";
      wchar_t *ver10 = L"Shall you pace forth; your praise shall still find room";
      wchar_t *ver11 = L"Even in the eyes of all posterity";
      wchar_t *ver12 = L"That wear this world out to the ending doom.";
      wchar_t *ver13 = L"So, till the judgment that yourself arise,";
      wchar_t *ver14 = L"You live in this, and dwell in lovers' eyes.";

      TextOutW(hdc, 50, 20, ver1, lstrlenW(ver1));
      TextOutW(hdc, 50, 40, ver2, lstrlenW(ver2));
      TextOutW(hdc, 50, 60, ver3, lstrlenW(ver3));
      TextOutW(hdc, 50, 80, ver4, lstrlenW(ver4));
      TextOutW(hdc, 50, 100, ver5, lstrlenW(ver5));
      TextOutW(hdc, 50, 120, ver6, lstrlenW(ver6));
      TextOutW(hdc, 50, 140, ver7, lstrlenW(ver7));
      TextOutW(hdc, 50, 160, ver8, lstrlenW(ver8));
      TextOutW(hdc, 50, 180, ver9, lstrlenW(ver9));
      TextOutW(hdc, 50, 200, ver10, lstrlenW(ver10));
      TextOutW(hdc, 50, 220, ver11, lstrlenW(ver11));
      TextOutW(hdc, 50, 240, ver12, lstrlenW(ver12));
      TextOutW(hdc, 50, 260, ver13, lstrlenW(ver13));
      TextOutW(hdc, 50, 280, ver14, lstrlenW(ver14));
      SelectObject(hdc, holdFont);
      DeleteObject(hFont);


      Rectangle(hdc, 50, 50, 200, 100);

      MoveToEx(hdc, 50, 50, NULL);
      LineTo(hdc, 250, 50);
      HPEN hWhitePen = (HPEN)GetStockObject(WHITE_PEN);
      HPEN hOldPen = (HPEN)SelectObject(hdc, hWhitePen);
      MoveToEx(hdc, 50, 100, NULL);
      LineTo(hdc, 250, 100);

      DrawPixels(hwnd);

      POINT points[4] = { 20, 40, 320, 200, 330, 110, 450, 40 };
      PolyBezier(hdc, points, 4);



      HPEN hPen1 = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
      HPEN hPen2 = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
      HPEN hPen3 = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
      HPEN hPen4 = CreatePen(PS_DASHDOT, 1, RGB(0, 0, 0));
      HPEN hPen5 = CreatePen(PS_DASHDOTDOT, 1, RGB(0, 0, 0));
      HPEN holdPen = (HPEN)SelectObject(hdc, hPen1);

      MoveToEx(hdc, 50, 30, NULL);
      LineTo(hdc, 300, 30);
      SelectObject(hdc, hPen2);
      MoveToEx(hdc, 50, 50, NULL);
      LineTo(hdc, 300, 50);
      SelectObject(hdc, hPen2);
      MoveToEx(hdc, 50, 70, NULL);
      LineTo(hdc, 300, 70);
      SelectObject(hdc, hPen3);
      MoveToEx(hdc, 50, 90, NULL);
      LineTo(hdc, 300, 90);
      SelectObject(hdc, hPen4);
      MoveToEx(hdc, 50, 110, NULL);
      LineTo(hdc, 300, 110);
      SelectObject(hdc, holdPen);
      DeleteObject(hPen1);
      DeleteObject(hPen2);
      DeleteObject(hPen3);
      DeleteObject(hPen4);
      DeleteObject(hPen5);

      SelectObject(hdc, hOldPen);
      EndPaint(hwnd, &ps);

      DoDrawing(hwnd);
      DrawRectangles(hwnd);
      DrawHatchRectangles(hwnd);
      }
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
   }
   return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void DrawPixels(HWND hwnd)
{
   RECT r;
   GetClientRect( hwnd, &r );

   if ( r.bottom == 0 )
   {
      return;
   }

   PAINTSTRUCT ps;
   HDC hdc = BeginPaint(hwnd, &ps);

   for ( int i=0; i<1000; i++ )
   {
      int x = rand() % r.right;
      int y = rand() % r.bottom;
      SetPixel(hdc, x, y, RGB(255, 0, 0));
   }

   EndPaint(hwnd, &ps);

   DoDrawing(hwnd);
}


void DoDrawing(HWND hwnd)
{
   LOGBRUSH brush;
   COLORREF col = RGB(0, 0, 0);
   DWORD pen_style = PS_SOLID | PS_JOIN_MITER | PS_GEOMETRIC;
   brush.lbStyle = BS_SOLID;
   brush.lbColor = col;
   brush.lbHatch = 0;
   PAINTSTRUCT ps;
   HDC hdc = BeginPaint(hwnd, &ps);
   HPEN hPen1 = ExtCreatePen(pen_style, 8, &brush, 0, NULL);
   HPEN holdPen = (HPEN)SelectObject(hdc, hPen1);
   POINT points[5] = {  { 30, 30 }, { 130, 30 }, { 130, 100 },
                        { 30, 100 }, { 30, 30}  };
   Polygon(hdc, points, 5);
   pen_style = PS_SOLID | PS_GEOMETRIC | PS_JOIN_BEVEL;
   HPEN hPen2 = ExtCreatePen(pen_style, 8, &brush, 0, NULL);
   SelectObject(hdc, hPen2);
   DeleteObject(hPen1);
   POINT points2[5] = { { 160, 30 }, { 260, 30 }, { 260, 100 },
                        { 160, 100 }, {160, 30 }};
   MoveToEx(hdc, 130, 30, NULL);
   Polygon(hdc, points2, 5);
   pen_style = PS_SOLID | PS_GEOMETRIC | PS_JOIN_ROUND;
   HPEN hPen3 = ExtCreatePen(pen_style, 8, &brush, 0, NULL);
   SelectObject(hdc, hPen3);
   DeleteObject(hPen2);
   POINT points3[5] = { { 290, 30 }, { 390, 30 }, { 390, 100 },
                        { 290, 100 }, {290, 30 }   };
   MoveToEx(hdc, 260, 30, NULL);
   Polygon(hdc, points3, 5);
   SelectObject(hdc, holdPen);
   DeleteObject(hPen3);
   EndPaint(hwnd, &ps);
}

void DrawRectangles(HWND hwnd)
{
   PAINTSTRUCT ps;
   HDC hdc = BeginPaint(hwnd, &ps);
   HPEN hPen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
   HPEN holdPen = (HPEN)SelectObject(hdc, hPen);
   HBRUSH hBrush1 = CreateSolidBrush(RGB(121, 90, 0));
   HBRUSH hBrush2 = CreateSolidBrush(RGB(240, 63, 19));
   HBRUSH hBrush3 = CreateSolidBrush(RGB(240, 210, 18));
   HBRUSH hBrush4 = CreateSolidBrush(RGB(9, 189, 21));
   HBRUSH holdBrush = (HBRUSH)SelectObject(hdc, hBrush1);
   Rectangle(hdc, 30, 30, 100, 100);
   SelectObject(hdc, hBrush2);
   Rectangle(hdc, 110, 30, 180, 100);
   SelectObject(hdc, hBrush3);
   Rectangle(hdc, 30, 110, 100, 180);
   SelectObject(hdc, hBrush4);
   Rectangle(hdc, 110, 110, 180, 180);
   SelectObject(hdc, holdPen);
   SelectObject(hdc, holdBrush);
   DeleteObject(hPen);
   DeleteObject(hBrush1);
   DeleteObject(hBrush2);
   DeleteObject(hBrush3);
   DeleteObject(hBrush4);
   EndPaint(hwnd, &ps);
}

void DrawHatchRectangles(HWND hwnd)
{
   PAINTSTRUCT ps;
   HDC hdc = BeginPaint(hwnd, &ps);
   HPEN hPen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
   HPEN holdPen = (HPEN)SelectObject(hdc, hPen);
   HBRUSH hBrush1 = CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0));
   HBRUSH hBrush2 = CreateHatchBrush(HS_FDIAGONAL, RGB(0, 0, 0));
   HBRUSH hBrush3 = CreateHatchBrush(HS_CROSS, RGB(0, 0, 0));
   HBRUSH hBrush4 = CreateHatchBrush(HS_HORIZONTAL, RGB(0, 0, 0));
   HBRUSH hBrush5 = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 0));
   HBRUSH hBrush6 = CreateHatchBrush(HS_VERTICAL, RGB(0, 0, 0));
   HBRUSH holdBrush = (HBRUSH)SelectObject(hdc, hBrush1);
   DWORD col = GetSysColor(COLOR_BTNFACE);
   SetBkColor(hdc, col);
   Rectangle(hdc, 30, 30, 100, 80);
   SelectObject(hdc, hBrush2);
   Rectangle(hdc, 110, 30, 180, 80);
   SelectObject(hdc, hBrush3);
   Rectangle(hdc, 190, 30, 260, 80);
   SelectObject(hdc, hBrush4);
   Rectangle(hdc, 30, 110, 100, 160);
   SelectObject(hdc, hBrush5);
   Rectangle(hdc, 110, 110, 180, 160);
   SelectObject(hdc, hBrush6);
   Rectangle(hdc, 190, 110, 260, 160);
   SelectObject(hdc, holdPen);
   SelectObject(hdc, holdBrush);
   DeleteObject(hPen);
   DeleteObject(hBrush1);
   DeleteObject(hBrush2);
   DeleteObject(hBrush3);
   DeleteObject(hBrush4);
   DeleteObject(hBrush5);
   DeleteObject(hBrush6);
   EndPaint(hwnd, &ps);
}

inline
HBITMAP createHBitmap( de::Image const & img )
{
   size_t w = img.getWidth();
   size_t h = img.getHeight();

   std::vector< uint8_t > tmp( w * h * size_t( 4 ) );

   uint8_t const* src = img.data();
   uint8_t* dst = tmp.data();

   for ( size_t y = 0; y < h; ++y )
   {
      for ( size_t x = 0; x < w; ++x )
      {
         uint8_t b = *src++;
         uint8_t g = *src++;
         uint8_t r = *src++;
         uint8_t a = *src++;
         *dst++ = r;
         *dst++ = g;
         *dst++ = b;
         *dst++ = a;
      }
   }

   HBITMAP hBitmap = CreateBitmap( w, h, 4, 8, (LPBYTE)tmp.data() );
   return hBitmap;
}

inline
void drawHBitmap( HDC dc, HBITMAP hBitmap )
{
   if ( !dc )
   {
      std::cout << "No dc" << std::endl;
      return;
   }

   if ( !hBitmap )
   {
      std::cout << "No hBitmap" << std::endl;
      return;
   }

   HDC mem = CreateCompatibleDC( dc );
   if ( !mem )
   {
      std::cout << "No mem dc" << std::endl;
      return;
   }

   HBITMAP old = (HBITMAP)SelectObject( mem, hBitmap );

   BITMAP bm;
   GetObject( hBitmap, sizeof( bm ), &bm );
   BitBlt( dc, 0, 0, bm.bmWidth, bm.bmHeight, mem, 0, 0, SRCCOPY );
   SelectObject( mem, old );
   DeleteDC( mem );
}

struct Bitmap
{
   int w = 0;
   int h = 0;
   HBITMAP handle = nullptr;
};

Bitmap createBitmap( de::Image const & img )
{
   int w = img.getWidth();
   int h = img.getHeight();
   HBITMAP hBitmap = CreateBitmap( w, h, 4, 32, (LPBYTE) img.data() );
   Bitmap bmp;
   bmp.w = w;
   bmp.h = h;
   bmp.handle = hBitmap;
   return bmp;
}

void drawBitmap( HDC dc, Bitmap const & bmp )
{
//   PAINTSTRUCT ps;
//   HDC hdc = BeginPaint(hwnd, &ps);
   HDC cdc = CreateCompatibleDC( dc );
   //HBITMAP hbmOld = SelectObject(hdcMem, g_hbmBall);
   HBITMAP old = (HBITMAP)SelectObject( dc, bmp.handle );

   BITMAP bm;
   GetObject( bmp.handle, sizeof(bm), &bm );

   BitBlt( dc, 0, 0, bm.bmWidth, bm.bmHeight, cdc, 0, 0, SRCCOPY );
   SelectObject( cdc, old );
   DeleteDC( cdc );
}

*/

/*

//! notifies the device that it should close itself
void closeDevice()
{
   MSG msg;
   PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
   PostQuitMessage(0);
   PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
   if (!m_IsExternalWindow)
   {
      DestroyWindow(m_HWnd);
      auto ClassName = __TEXT("IWindow");
      HINSTANCE hInstance = GetModuleHandle(0);
      UnregisterClass(ClassName, hInstance);
   }
   m_Close=true;
}

//! switches to fullscreen
bool switchToFullScreen(bool reset)
{
   if (!m_CreationParams.Fullscreen) return true;

   if (reset)
   {
      if (m_ChangedToFullScreen)
      {
         return (ChangeDisplaySettings(&m_DesktopMode,0)==DISP_CHANGE_SUCCESSFUL);
      }
      else
         return true;
   }

   // use default values from current setting
   DEVMODE dm;
   memset(&dm, 0, sizeof(dm));
   dm.dmSize = sizeof(dm);

   EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
   dm.dmPelsWidth = m_CreationParams.WindowSize.x;
   dm.dmPelsHeight = m_CreationParams.WindowSize.y;
   dm.dmBitsPerPel = m_CreationParams.Bits;
   dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

   LONG res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
   if (res != DISP_CHANGE_SUCCESSFUL)
   { // try again without forcing display frequency
      dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
      res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
   }

   bool ret = false;
   switch(res)
   {
   case DISP_CHANGE_SUCCESSFUL:
      m_ChangedToFullScreen = true;
      ret = true;
      break;
   case DISP_CHANGE_RESTART:
      //os::Printer::log("Switch to fullscreen: The computer must be restarted in order for the graphics mode to work.", ELL_ERROR);
      break;
   case DISP_CHANGE_BADFLAGS:
      //os::Printer::log("Switch to fullscreen: An invalid set of flags was passed in.", ELL_ERROR);
      break;
   case DISP_CHANGE_BADPARAM:
      //os::Printer::log("Switch to fullscreen: An invalid parameter was passed in. This can include an invalid flag or combination of flags.", ELL_ERROR);
      break;
   case DISP_CHANGE_FAILED:
      //os::Printer::log("Switch to fullscreen: The display driver failed the specified graphics mode.", ELL_ERROR);
      break;
   case DISP_CHANGE_BADMODE:
      //os::Printer::log("Switch to fullscreen: The graphics mode is not supported.", ELL_ERROR);
      break;
   default:
      //os::Printer::log("An unknown error occured while changing to fullscreen.", ELL_ERROR);
      break;
   }
   return ret;
}

//! Sets if the window should be resizable in windowed mode.
void setResizable(bool resize)
{
   if (m_IsExternalWindow || m_CreationParams.Fullscreen) return;

   LONG_PTR style = WS_POPUP;

   if (!resize)
      style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
   else
      style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

   if (!SetWindowLongPtr(m_HWnd, GWL_STYLE, style))
   {
      // os::Printer::log("Could not change window style.");
   }

   RECT clientSize;
   clientSize.top = 0;
   clientSize.left = 0;
   clientSize.right = 640; // TODO: store Width;
   clientSize.bottom = 480; // TODO: store Height;

   ::AdjustWindowRect(&clientSize, style, FALSE);

   int32_t const realWidth = clientSize.right - clientSize.left;
   int32_t const realHeight = clientSize.bottom - clientSize.top;

   int32_t const windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
   int32_t const windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

   ::SetWindowPos(m_HWnd, HWND_TOP, windowLeft, windowTop, realWidth, realHeight,
      SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);

   // static_cast<CCursorControl*>(m_CursorControl)->updateBorderSize(m_CreationParams.Fullscreen, resize);
}


//! Set the current Gamma Value for the Display
bool setGammaRamp( float red, float green, float blue, float brightness, float contrast )
{
   bool r;
   uint16_t ramp[3][256];

   calculateGammaRamp( ramp[0], red, brightness, contrast );
   calculateGammaRamp( ramp[1], green, brightness, contrast );
   calculateGammaRamp( ramp[2], blue, brightness, contrast );

   HDC dc = GetDC(0);
   r = SetDeviceGammaRamp ( dc, ramp ) == TRUE;
   ReleaseDC(m_HWnd, dc);
   return r;
}

//! Get the current Gamma Value for the Display
bool getGammaRamp( float &red, float &green, float &blue, float &brightness, float &contrast )
{
   bool r;
   uint16_t ramp[3][256];

   HDC dc = GetDC(0);
   r = GetDeviceGammaRamp ( dc, ramp ) == TRUE;
   ReleaseDC(m_HWnd, dc);

   if ( r )
   {
      calculateGammaFromRamp(red, ramp[0]);
      calculateGammaFromRamp(green, ramp[1]);
      calculateGammaFromRamp(blue, ramp[2]);
   }

   brightness = 0.f;
   contrast = 0.f;

   return r;

}


//! Process system events
void handleSystemMessages()
{
   MSG msg;

   while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
   {
      // No message translation because we don't use WM_CHAR and it would conflict with our
      // deadkey handling.

      if (m_IsExternalWindow && msg.hwnd == m_HWnd)
         doWndProc(m_HWnd, msg.message, msg.wParam, msg.lParam);
      else
         DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
         m_Close = true;
   }
}


//! Remove all messages pending in the system message loop
void clearSystemMessages()
{
   MSG msg;
   while (PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
   {}
   while (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
   {}
}

*/

#endif