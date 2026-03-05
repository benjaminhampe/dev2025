#include "StringConv.h"
#include <QString>

std::string
StringConv::toStr( std::wstring const & w )
{
   return QString::fromStdWString( w ).toStdString();
}

std::wstring
StringConv::toWStr( std::string const & s )
{
   return QString::fromStdString( s ).toStdWString();
}


#if 0

#ifndef UNICODE
#define UNICODE
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600 // CreateEventEx() needs atleast this API version = WinXP or so.

#include <windows.h>
#include <synchapi.h>
#include <process.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
//#include <commdlg.h>
//#include <QFileDialog>
//#define ASSERT_THROW(c,e) if(!(c)) { throw std::runtime_error(e); }
//#define CLOSE_HANDLE(x)   if((x)) { CloseHandle(x); x = nullptr; }
//#define RELEASE(x)        if((x)) { (x)->Release(); x = nullptr; }

//#pragma warning(push)
//#pragma warning(disable : 4996)
#include <pluginterfaces/vst2.x/aeffectx.h>  // de_vst2sdk
#include <pluginterfaces/vst2.x/aeffect.h>
//#pragma warning(pop)

// Plugin needs path/directory of itself
inline std::string
toMultiByte( std::wstring const & msg )
{
   if ( msg.empty() ) return "";

   std::array< char, 4096 > mbuf{ 0 };

   auto n = WideCharToMultiByte( CP_OEMCP, 0, msg.data(), -1, mbuf.data(), mbuf.size(), 0, 0 );

   return mbuf.data();
}


// // Plugin needs path/directory of itself
// inline std::wstring
// getFullPathNameW( std::wstring const & msg )
// {
   // wchar_t buf[ MAX_PATH + 1 ] {};
   // wchar_t* namePtr = nullptr;
   // auto const r = GetFullPathName( msg.c_str(), _countof(buf), buf, &namePtr );
   // if ( r && namePtr )
   // {
      // return std::wstring( namePtr );
   // }
   // return std::wstring();
// }

#endif
