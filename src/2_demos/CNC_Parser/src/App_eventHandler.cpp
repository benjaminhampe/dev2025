#include <App_eventHandler.hpp>
#include <App_create.hpp>
#include <../res/resource.h>

LRESULT CALLBACK 
GCodeParserApp_eventHandler (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
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
   case WM_CREATE:
   {
      GCodeParserApp_onWMCreate( hwnd );

   } break;
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
            std::string uri = dbOpenFileA();

            std::string gcode = de::cnc::TextUtil::loadText( uri );
            if ( gcode.size() )
            {
               de::cnc::CNC_Machine computer;
               de::cnc::CNC_Parser parser;
               parser.addListener( &computer );
               parser.parse( uri, gcode );
               auto htmlUri = computer.save( uri );

               system( htmlUri.c_str() );
            }
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
            doModalAboutDialog(hwnd);
         }
         break;
      default:
         break;
      }

      if (HIWORD(wParam) == BN_CLICKED)
      {
         if (LOWORD(wParam) == ID_BTN_ESTIMATE)
         {
            MessageBeep(MB_OK);
         }
         else if (LOWORD(wParam) == ID_BTN_SHOW3D)
         {
            std::wstring msg = getEditBoxTextW( g_app.m_edtFeedSpeed );
            dbMessageW( msg, L"EditBox FeedSpeed" );
         }
         else if (LOWORD(wParam) == ID_BTN_BLUE)
         {
            dbMessageA("ID_BTN_BLUE","RadioButton");
         }
         else if (LOWORD(wParam) == ID_BTN_YELLOW)
         {
            dbMessageA("ID_BTN_YELLOW","RadioButton");
         }
      }
   } break;
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
