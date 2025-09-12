#pragma once
#include "App_create.hpp"

inline bool GCodeParserApp_run()
{  
   // Nachrichten-Schleife
   MSG msg;
   if (GetMessage (&msg, nullptr, 0, 0))
   {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
      return true;
   }
   
   return false;
}


inline void GCodeParserApp_destroy()
{  
   HWND hwnd = g_app.m_hWnd;

   WINDOWINFO windowInfo;
   windowInfo.cbSize = sizeof( WINDOWINFO );
   GetWindowInfo( hwnd, &windowInfo );
   DestroyWindow( hwnd );
   UnregisterClass( reinterpret_cast< LPCTSTR >( windowInfo.atomWindowType ),
                    GetModuleHandle( nullptr ) );

   g_app.m_hWnd = nullptr;

   // return 0; // int(msg.wParam)
}
