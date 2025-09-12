#pragma once
#include <de_win32api.hpp>
#include <functional>

inline HWND
createButton( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND button = CreateWindowW( L"Button",
                  caption.c_str(),
                  WS_VISIBLE | WS_CHILD, //  | BS_CHECKBOX
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
createDemoButton( std::wstring caption, int x, int y, int w, int h, HWND parent, int id,
                  std::function< void() > const & callback = [](){} );

*/
