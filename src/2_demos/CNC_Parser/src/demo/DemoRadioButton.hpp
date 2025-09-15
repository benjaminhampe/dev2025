#pragma once
#include <cstdint>
#include <sstream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <functional>

inline HWND
createStaticText( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND label = CreateWindowW( L"Static",
                  caption.c_str(),
                  WS_CHILD | WS_VISIBLE | SS_LEFT,
                  x,y,w,h, parent,
                  reinterpret_cast<HMENU>(id), nullptr, nullptr );

//   SetWindowLongPtr( label, GWL_USERDATA,
//                     reinterpret_cast< LONG_PTR >( button ) );
   return label;
}

inline HWND
createGroupBox( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND button = CreateWindowW( L"Button",
                  caption.c_str(),
                  WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                  x,y,w,h, parent,
                  reinterpret_cast<HMENU>(id), nullptr, nullptr );

//   SetWindowLongPtr( button->m_hwnd, GWL_USERDATA,
//                     reinterpret_cast< LONG_PTR >( button ) );
   return button;
}

inline HWND
createRadioButton( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND button = CreateWindowW( L"Button",
                  caption.c_str(),
                  WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                  x,y,w,h, parent,
                  reinterpret_cast<HMENU>(id), nullptr, nullptr );

//   SetWindowLongPtr( button->m_hwnd, GWL_USERDATA,
//                     reinterpret_cast< LONG_PTR >( button ) );
   return button;
}

/*
struct Button
{
   int m_id = 0;
   HWND m_hwnd = nullptr;
   std::function< void() > m_callback;

   LRESULT CALLBACK
   handleEvent(UINT msg, WPARAM wParam, LPARAM lParam)
   {
      switch(msg)
      {
         case WM_COMMAND:
         {
            if (HIWORD(wParam) == BN_CLICKED)
            {
               if (LOWORD(wParam) == m_id)
               {
                  if (m_callback)
                  {
                     m_callback();
                  }
               }
            }
            break;
         }
         default: break;
      }
      return DefWindowProc(m_hwnd, msg, wParam, lParam);
   }

   static LRESULT CALLBACK
   globalEventHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
   {
      Button* btn = (Button*)GetWindowLongPtr( hWnd, GWL_USERDATA );
      if ( btn )
      {
         return btn->handleEvent( uMsg, wParam, lParam );
      }
      else
      {
         return DefWindowProc( hWnd, uMsg, wParam, lParam );
      }
   }
};

Button*
createDemoRadioButton( std::wstring caption, int x, int y, int w, int h, HWND parent, int id,
                  std::function< void() > const & callback = [](){} );

*/
