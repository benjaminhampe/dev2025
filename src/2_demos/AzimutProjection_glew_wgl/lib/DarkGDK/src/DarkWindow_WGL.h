#pragma once
#include "DarkWindow_details.h"
#include <de_opengl.h>

/*
#include <vector>
#include <array>
#include <cwchar>
#include <codecvt>
#include <cstring> // strlen()
#include <cmath>
#include <codecvt> // convert wstring ( unicode ) <-> string ( multibyte utf8 )
#include <algorithm>
#include <iomanip> // string_converter
#include <memory>
*/

#ifdef BENNI_USE_COUT
   #include <iostream>
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <dwmapi.h>

#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

//#include <uxtheme.h>
//#include <vssym32.h>  // for DPI awareness? We use embedded resource .xml for that.
//#include <commctrl.h>
#include <wchar.h>
#include <versionhelpers.h>
//#include <commdlg.h>
//#include <shellapi.h>
//#include <shlobj.h>
//#include <wctype.h>
//#include <dinput.h>   // For JOYCAPS
//#include <xinput.h>   // For JOYCAPS
//#include <mmsystem.h> // For JOYCAPS
//#include <dbt.h>

#if !defined(GET_X_LPARAM)
   #define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
   #define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

// ===================================================================
// INCLUDE: WGL
// ===================================================================
#ifndef WGL_WGLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES
#endif

#include <GL/wglext.h>

namespace DarkGDK {

// ===================================================================
// WGL hardware accelerated OpenGLWindow
// ===================================================================

struct DarkWindow_WGL
{
   DarkWindow_WGL();
   ~DarkWindow_WGL();

   // Most important thing to reduce power consumption.
   // But interferes with sync timings, vsync, fps wait duration computation, etc...
   // Saves 98% power and is therefore a must have.
   void yield( int ms = 1 );

   // Sync GL with WGL, still badly researched tested
   // Its not clear if DwmFlush() should be used with SwapBuffers().
   // Its not clear if glFlush() should be used at all.
   // Seems to reduce vsync to 30 fps, because we swapped one time too often.
   // I recommend disabling vsync, enabling double buffering and have fun.
   void swapBuffers();

   bool create( CreateParams params );
   void destroy();

   bool run();

   void requestClose();

   void setEventReceiver( IEventReceiver* receiver );
   IEventReceiver* getEventReceiver();

   bool getKeyState( EKEY key ) const;

   void setWindowTitle( char const* title );

   void setWindowIcon( int iRessourceID );

   bool isResizable() const { return m_params.isResizable; }
   void setResizable( bool resizable );

   bool isFullScreen() const { return m_params.isFullscreen; }
   void setFullScreen( bool fullscreen );

   int getDesktopWidth() const;
   int getDesktopHeight() const;

   int getWindowWidth() const;
   int getWindowHeight() const;
   int getWindowPosX() const;
   int getWindowPosY() const;

   int getClientWidth() const;
   int getClientHeight() const;

//   void setClearColor( float r, float g, float b, float a )
//   {
//      m_clearColorR = r;
//      m_clearColorG = g;
//      m_clearColorB = b;
//      m_clearColorA = a;
//   }

protected:
   CreateParams m_params;

   HINSTANCE m_hInstance;
   HWND m_hWnd;
   HDC m_hDC;
   HGLRC m_hRC;

   IEventReceiver* m_eventReceiver;

   bool m_shouldRun;

   DWORD m_windowStyle;
   DWORD m_windowedStyle;
   DWORD m_fullscreenStyle;

   DEVMODE m_desktopMode;

   HWND m_dummyWnd;
   HDC m_dummyDC;
   HGLRC m_dummyRC;

   //HMODULE m_opengl32;

   PIXELFORMATDESCRIPTOR m_pfd;

   //std::string m_wglExtensionString;

//   float m_clearColorR = 0.1f;
//   float m_clearColorG = 0.1f;
//   float m_clearColorB = 0.1f;
//   float m_clearColorA = 1.0f;
public:
   HKL m_KEYBOARD_INPUT_HKL;
   uint32_t m_KEYBOARD_INPUT_CODEPAGE; // default: 1252 (Portuguese?)

   int m_screenWidth;
   int m_screenHeight;

public:
/*
 * Used to pretty print WGL and GL extensions strings...
 *
   static std::vector< std::string >
   splitText( std::string const & txt, char searchChar )
   {
      std::vector< std::string > lines;

      std::string::size_type pos1 = 0;
      std::string::size_type pos2 = txt.find( searchChar, pos1 );

      while ( pos2 != std::string::npos )
      {
         std::string line = txt.substr( pos1, pos2-pos1 );
         if ( !line.empty() )
         {
            lines.emplace_back( std::move( line ) );
         }

         pos1 = pos2+1;
         pos2 = txt.find( searchChar, pos1 );
      }

      std::string line = txt.substr( pos1 );
      if ( !line.empty() )
      {
         lines.emplace_back( std::move( line ) );
      }

      return lines;
   }

   static std::string
   replaceText( std::string const & txt,
                std::string const & from,
                std::string const & to,
                size_t* nReplacements = nullptr )
   {
      size_t nReplaces = 0;
      std::string s = txt;

      if ( s.empty() )
      {
         return s;
      }

      if ( to.empty() )
      {
         size_t pos = s.find( from );
         while( pos != std::string::npos )
         {
            nReplaces++;
            s.erase( pos, pos + from.size() );
            pos = s.find( from, pos );
         }
      }
      else
      {
         size_t pos = s.find( from );
         while( pos != std::string::npos )
         {
            nReplaces++;
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            pos = s.find( from, pos ); // advance to next token, if any
         }
      }
      if ( nReplacements ) *nReplacements = size_t(nReplaces);
      return s;
   }
   */
};

} // end namespace DarkGDK.
