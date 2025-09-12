/*
 *  FileSystem.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#include <de_core/de_FileSystem.h>
#include <de_core/de_StringUtil.h>

// STL C++17
// #include <filesystem>
// namespace fs = std::filesystem;
// GCC 7.3.0 needs extra link with -lstdc++fs
//#include <experimental/filesystem> // GCC 7.3.0 and link with -lstdc++fs
#include <filesystem> // GCC 8.1.0+

#include <fstream>
//#include <iostream>

#include <de_core/de_PerformanceTimer.h>

namespace de {

//namespace fs = std::experimental::filesystem;
namespace fs = std::filesystem;

//static
bool
FileSystem::loadByteVector( std::vector< uint8_t > & bv, std::string const & uri, uint64_t byteLimit )
{
   bv.clear(); // bv = ByteVector

   // Open file as binary
   FILE* file = ::fopen64( uri.c_str(), "rb" );
   if ( !file )
   {
      DE_ERROR("Cant open file ", uri )
      return false;
   }

   // Read file size
   ::fseeko64( file, 0, SEEK_END );
   size_t fileBytes = size_t( ::ftello64( file ) );
   ::fseeko64( file, 0, SEEK_SET );

   // Validate file size
   if ( fileBytes < 1 ) // No data
   {
      DE_ERROR("Empty file ",uri)
      ::fclose( file );                               // close
      return false;
   }

   // Validate file size
   if ( fileBytes > byteLimit ) // Bad, reached RAM limit.
   {
      DE_ERROR("File reached RAM limit(",byteLimit,"), abort. ",uri)
      ::fclose( file );
      return false;
   }

   // Copy entire file to RAM
   bv.resize( fileBytes );
   ::fread( bv.data(), 1, bv.size(), file );
   ::fclose( file );                                  // close

   //DE_DEBUG("File has uri = ",uri)
   //DE_DEBUG("File has byteCount = ",bv.size())

   return true;
}

std::string
FileSystem::loadText( std::string const & uri )
{
   std::ifstream fin( uri.c_str() );
   std::stringstream s;
   if ( fin.is_open() )
   {
      s << fin.rdbuf();
   }
   else
   {
      s << "Not a file " << uri;
   }

   return s.str();
}

bool
FileSystem::saveText( std::string const & uri, std::string const & txt )
{
   std::ofstream fout( uri );
   if ( !fout.is_open() ) return false;
   fout << txt;
   fout.close();
   return true;
}

bool
FileSystem::existFile( std::string const & uri )
{
   if ( uri.empty() ) { return false; }
   fs::file_status s = fs::status( uri );
   if ( !fs::exists( s ) ) { return false; }
   if ( !fs::is_regular_file( s ) ) { return false; }
   return true;
}

bool
FileSystem::existDirectory( std::string const & fileName )
{
   if ( fileName.empty() ) return false;
   fs::file_status s = fs::status( fileName );
   bool ok = fs::exists( s ) && fs::is_directory( s );
   return ok;
}

std::string
FileSystem::fileName( std::string const & uri )
{
   auto fn = fs::path( uri ).filename().string();
   if ( fn.empty() ) return uri;
   return fn;
}

// static
std::string
FileSystem::fileBase( std::string const & uri )
{
   return fs::path( uri ).stem().string();
}

// static
std::string
FileSystem::fileSuffix( std::string const & uri )
{
   if ( uri.empty() )
   {
      return {};
   }

   std::string ext = fs::path( uri ).extension().string();
   if ( ext.empty() )
   {
      return {};
   }

   if (ext[0] == '.')
   {
      ext.erase( 0, 1 );
   }

   StringUtil::lowerCase( ext );
   return ext;
}

// static
std::string
FileSystem::fileDir( std::string const & uri )
{
   fs::path p( uri );
   if ( p.is_relative() )
   {
      p = fs::absolute( p );
   }

   if ( !fs::is_directory( p ) )
   {
      if ( p.has_parent_path() )
      {
         p = p.parent_path();
      }
   }

   std::string tmp = p.string();
   StringUtil::replace( tmp, "\\", "/" );
   if ( StringUtil::endsWith( tmp, "/" ) )
   {
      tmp = tmp.substr( 0, tmp.size() - 1 );
   }
   return tmp;
}

// static
std::string
FileSystem::makeAbsolute( std::string uri, std::string baseDir )
{
   std::string s;

   try
   {
      fs::path p;

      StringUtil::makeWinPath( uri );
      fs::path file( uri );
      //DE_DEBUG("fs::path(uri) = ",file.string() )

      if ( baseDir.empty() )
      {
         p = fs::absolute( file );
      }
      else
      {
         StringUtil::makeWinPath( baseDir );
         fs::path dir( baseDir );
         //DE_DEBUG("fs::path(dir) = ",dir.string() )
         //p = fs::absolute( file, dir ); // GCC 7.3
         p = fs::absolute( file ); // GCC 8
      }
      //fs::path cwd = fs::current_path();
      //DE_DEBUG("fs::current_path(",cwd.string(),")" )

      //DE_DEBUG("fs::path.absolute(",p.string(),")" )

      p = fs::canonical( p );

      //DE_DEBUG("fs::path.absolute.canonical(",p.string(),")" )

      s = p.string();

      //DE_DEBUG("fs::path.absolute.canonical.string(",s,")" )

      //s = StringUtil::makePosix( s );

      //DE_DEBUG("fs::path.absolute.canonical.string.posix(",s,")" )
   }
   catch ( std::exception & e )
   {
      DE_DEBUG("exception what(",e.what(),"), uri = ",uri )
      s = uri;
   }

   //DE_DEBUG("makeAbsolute(",uri,") = ",s)
   return s;
}

// static
void
FileSystem::createDirectory( std::string const & uri )
{
   if ( uri.empty() ) return;

   fs::path p( uri );

   fs::file_status s = fs::status( p );

   if ( fs::exists( s ) && fs::is_directory( s ) )
   {
      // std::cout << "[Warn] " << __func__ << "( uri:" << uri << ") :: Dir already exists." << std::endl;
      return;
   }

   std::error_code e;
   fs::create_directories( p, e );
   if ( e )
   {
      DE_ERROR("( uri:",uri,") :: Got error ",e.message())
   }
}

// static
void
FileSystem::removeFile( std::string const & uri )
{
   std::error_code e;
   fs::remove( uri, e );
   if ( e )
   {
      DE_ERROR("Cant remove file(",uri,") :: ",e.message())
   }
}

// static
bool
FileSystem::copyFile( std::string src, std::string dst )
{
   DE_DEBUG("src(",src,"), dst(",dst,")")

   if ( src.empty() )
   {
      return false;
   }
   if ( dst.empty() )
   {
      return false;
   }

   StringUtil::makeWinPath(src);

   DE_DEBUG("makeWinPath(src) = ",src)

   FileSystem::makeAbsolute(src);

   DE_DEBUG("makeAbsolute(src) = ",src)

   StringUtil::makeWinPath(dst);

   DE_DEBUG("makeWinPath(dst) = ",dst)

   fs::path p0( src );
   fs::file_status s0 = fs::status( p0 );
   if ( !fs::exists( s0 ) )
   {
      DE_ERROR("File not exist! src(",src,"), dst(",dst,")")
      return false;
   }
   else
   {
      DE_INFO("File src exist (",src,")")
   }
   if ( !fs::is_regular_file( s0 ) )
   {
      DE_ERROR("File not a regular file! src(",src,"), dst(",dst,")")
      return false;
   }
   else
   {
      DE_INFO("File src is regular (",src,")")
   }

   fs::path p1( dst );
//   fs::path p1 = dst.parent_path();
//   std::error_code e;
//   fs::create_directories( p_dir, e );
//   if ( e )
//   {
//      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
//   }

   if ( fs::exists( p1 ) )
   {
      removeFile( p1.string() );
   }

   fs::copy_file( p0, p1 ); //, fs::copy_options::recursive );
   if ( fs::exists( p1 ) )
   {
      DE_DEBUG("Copied file(",src," -> ",dst,")")
      return true;
   }
   else
   {
      DE_ERROR("Cant copy file(",src,",",dst,")")
      return false;
   }
}

//static
std::string
createUniqueFileName( std::string const & fileName )
{
   int ms = HighResolutionClock::GetTimeInMilliseconds();

   std::ostringstream o;

   int y=0,m=0,d=0,hh=0,mm=0,ss=0;

   time_t cur = ::time( nullptr );
   struct tm* t = gmtime( &cur );
   if (t)
   {
      y = t->tm_year+1900;
      m = t->tm_mon+1;
      d = t->tm_mday;
      hh = t->tm_hour;
      mm = t->tm_min;
      ss = t->tm_sec;
   }
   o << fileName
     << "_" << y << "-" << m << "-" << d << "_" << hh << "-" << mm << "-" << ss << "_" << ms;
   return o.str();
}


} // end namespace de
