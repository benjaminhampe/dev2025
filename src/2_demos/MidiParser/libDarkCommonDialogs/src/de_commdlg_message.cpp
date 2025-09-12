#include <de_commdlg_message.hpp>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

//===========================================================
void dbMessageA( std::string const & msg, std::string const & caption )
//===========================================================
{
   MessageBoxA( nullptr, msg.c_str(), caption.c_str(), 0 );
}

//===========================================================
void dbMessageW( std::wstring const & msg, std::wstring const & caption )
//===========================================================
{
   MessageBoxW( nullptr, msg.c_str(), caption.c_str(), 0 );
}


/*
//===========================================================
std::string    dbGetDirA()
//===========================================================
{
   std::string tmp;
   tmp.resize( size_t( MAX_PATH ), 0x00 );
   GetCurrentDirectoryA( DWORD(tmp.size()), tmp.data() );
   std::cout << "GetCurrentDirectoryA() = " << tmp << std::endl;
   return tmp;
}

//===========================================================
std::wstring   dbGetDirW()
//===========================================================
{
   std::wstring tmp;
   tmp.resize( size_t( MAX_PATH ), 0x00 );
   GetCurrentDirectoryW( DWORD(tmp.size()), tmp.data() );
   std::wcout << L"GetCurrentDirectoryW() = " << tmp << std::endl;
   return tmp;
}

//===========================================================
void dbSetDirA( std::string const & dir )
//===========================================================
{
   SetCurrentDirectoryA( dir.c_str() );
}

//===========================================================
void dbSetDirW( std::wstring const & dir )
//===========================================================
{
   SetCurrentDirectoryW( dir.c_str() );
}
*/
