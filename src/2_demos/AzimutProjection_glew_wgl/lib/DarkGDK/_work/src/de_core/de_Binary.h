//#include <DarkImage.hpp>
//#include <de/FileSystem.hpp>
//#include <de/CommonDialogs.hpp>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <sstream>
#include <fstream>

//#include <iostream>
//#include <functional>
//#include <algorithm>
#include <cmath>
#include <codecvt> // convert wstring ( unicode ) <-> string ( multibyte utf8 )
#include <cwchar>
#include <algorithm>
#include <sstream>
#include <iomanip> // string_converter
#include <memory>

namespace de {

//// GET string FROM wstring
//std::string
//dbStr( std::wstring const & txt )
//{
//   if ( txt.size() < 1 ) return "";
//   std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
//   return converter.to_bytes( txt );
//}

//// GET string FROM wchar.
//std::string
//dbStr( wchar_t const wc )
//{
//   std::wstringstream w; w << wc;
//   std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
//   return converter.to_bytes( w.str() );
//}

//// GET wstring FROM string.
//std::wstring
//dbStrW( std::string const & txt )
//{
//   if ( txt.size() < 1 ) return L"";
//   std::wstring_convert< std::codecvt_utf8< wchar_t > > converter;
//   return converter.from_bytes( txt );
//}

//   static bool
//   dbSaveText( std::wstring const & uri, std::string const & txt )
//   {
//      return dbSaveText( dbStr(uri), txt );
//   }

//   static bool
//   dbSaveText( std::string const & uri, std::string const & txt )
//   {
//      std::ofstream fout( uri );
//      if ( !fout.is_open() ) return false;
//      fout << txt;
//      fout.close();
//      return true;
//   }

//   static char
//   dbStrLowNibble( uint8_t byte )
//   {
//      size_t lowbyteNibble = byte & 0x0F;
//      //     if ( lowbyteNibble == 0 ) return "0";
//      //else if ( lowbyteNibble == 1 ) return "1";

//      if ( lowbyteNibble >= 0 && lowbyteNibble < 10 )
//      {
//         return '0' + lowbyteNibble;
//      }
//      else
//      {
//         return 'A' + (lowbyteNibble-10);
//      }
//   }

//   static char
//   dbStrHighNibble( uint8_t byte )
//   {
//      return dbStrLowNibble( byte >> 4 );
//   }

//   static std::string
//   dbStrByte( uint8_t byte )
//   {
//      std::ostringstream s;
//      s << dbStrHighNibble( byte ) << dbStrLowNibble( byte );
//      return s.str();
//   }
   // Converts any file from disk (HDD) to c++ header file code.
   // 1. Reads HDD file 'loadUri' into RAM
   // 2. Converts binary to string to RAM. The string is valid C++ header file code
   //
   // static uint8_t const 'staticName' [ 2 ] =
   // {
   //    0x01, etc...
   // };
   //
   // 3. Writes this string to HDD with name 'saveUri', like
   //
   // std::string imgUri = name + "." + typ;
   //
   // Convert compressed image to binary C++ header file. Save hpp on HDD.
   //
   // std::string n_img = name + "_" + typ;
   // std::string hppUri = n_img + ".hpp";
   // // Convert compressed imgUri into header file hppUri wit static const name hppName.
   // uint64_t s_img = convertBinaryToHeaderFile( imgUri, n_img, hppUri );
   // std::string u_mtl = name + ".mtl";
   // uint64_t s_mtl = convertBinaryToHeaderFile( u_mtl, n_mtl, n_mtl + ".hpp" );
   // Write #includable c++ header file ( ca. 4x the file size ) from Binary.
   // Now you can embedd an image into your executable using your compiler.
   // This reduces dependencies of executables to external media files.
   // These media files are simply #included as code into the final monolithic program.
   // Decoding them is done using DarkImage dbLoadImageFromMemory()

//// ============================================================================
//struct Binary
//// ============================================================================
//{
//   std::vector< uint8_t > m_Blob;
//   uint8_t* m_Ptr;
//   uint64_t m_Count;
//   uint64_t m_Index;
//   uint64_t m_Avail;
//   std::string m_Uri;

//   Binary() : m_Ptr( nullptr ), m_Count( 0 ), m_Index( 0 ), m_Avail( 0 ) {}
//   Binary( std::string uri ) : Binary() { openFile( uri ); }
//   //~Binary() { close(); }
//   uint64_t size() const { return m_Count; }
//   uint8_t const* data() const { return m_Ptr; }
//   uint8_t* data() { return m_Ptr; }
//   std::string const & getUri() const { return m_Uri; }
//   bool is_open() const { return m_Ptr != nullptr; }
//   void close()
//   {
//      m_Ptr = nullptr;
//      m_Blob.clear();
//   }

//   bool
//   openMemory( uint8_t const * ptr, uint64_t count, std::string uri )
//   {
//      m_Blob.clear();
//      m_Ptr = const_cast< uint8_t* >( ptr );
//      m_Count = count;
//      m_Avail = count;
//      m_Index = 0;
//      m_Uri = uri;
//      return true;
//   }

//   bool
//   openFile( std::string uri )
//   {
//      m_Uri = uri;

//      FILE* file = ::fopen( uri.c_str(), "rb" );
//      if ( !file )
//      {
//         std::cout << "Cant open " << uri << "\n";
//         return false;
//      }

//      ::fseeko64( file, 0, SEEK_END );
//      size_t byteCount = size_t( ::ftello64( file ) );
//      ::fseeko64( file, 0, SEEK_SET );

//      if ( byteCount < 1 )
//      {
//         std::cout << "Empty file content " << uri << "\n";
//         return false;
//      }

//      m_Blob.clear();
//      try
//      {
//         m_Blob.resize( byteCount, 0x00 );
//         size_t bytesRead = ::fread( m_Blob.data(), 1, m_Blob.size(), file );
//         if ( bytesRead < byteCount )
//         {
//            std::cout << "bytesRead("<<bytesRead<<") < byteCount("<<byteCount<<"), "<< uri << "\n";
//            return false;
//         }
//      }
//      catch(...)
//      {
//         std::cout << "OOM" << std::endl;
//         return false;
//      }

//      ::fclose( file );
//      m_Ptr = m_Blob.data();
//      m_Count = m_Blob.size();
//      m_Avail = m_Blob.size();
//      m_Index = 0;
//      return true;
//   }

//   uint64_t
//   read( uint8_t* dst, uint64_t byteCount )
//   {
//      if ( !m_Ptr || m_Avail == 0 )
//      {
//         std::cout << "EOF" << std::endl;
//         return 0;
//      }

//      if ( byteCount > m_Avail )
//      {
//         auto retVal = m_Avail;
//         m_Index = m_Count - 1 - m_Avail;
//         ::memcpy( dst, m_Ptr + m_Index, m_Avail );
//         m_Avail = 0;
//         m_Index = 0;
//         return retVal;
//      }

//      ::memcpy( dst, m_Ptr + m_Index, byteCount );
//      m_Index += byteCount;
//      m_Avail -= byteCount;
//      return byteCount;
//   }
//};

} // end namespace de
