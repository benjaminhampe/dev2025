#include <de/FileSystem.hpp>
#include <codecvt>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <fstream>

// STL C++17
// #include <filesystem>
// namespace fs = std::filesystem;

// GCC 7.3.0 needs extra link with -lstdc++fs
#include <experimental/filesystem> // I use GCC 7.3.0 and link with -lstdc++fs
#include <de/String.hpp>

//#include <time.h>

namespace de {

namespace fs = std::experimental::filesystem;


std::string
FileSystem::cwd( bool enabled )
{
   std::string uri = fs::absolute( "." ).string();
   uri = dbMakePosix( uri, enabled );
   //DE_DEBUG("uri = ", uri)
   return uri;
}

std::string
FileSystem::caturi( std::string const & a, std::string const & b )
{
//   if ( baseDir.empty() )
//   {
//      return makeAbsolute( uri );
//   }

//   fs::path puri( uri );
//   if ( puri.is_absolute() )
//   {
//      DE_DEBUG("uri(",uri,") is already absolute!")
//      return makeAbsolute( uri );
//   }
   auto uri = a + std::string("/") + b;

   try
   {
      auto c = fs::canonical(uri).string();
      uri = dbMakePosix(c);
   }
   catch ( std::exception & e )
   {
      DE_DEBUG("Got filesystem exception ",e.what(),", uri = ",uri )
   }

   return uri;
}


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
         p = fs::absolute( file, dir );
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
      //DE_DEBUG("exception what(",e.what(),"), uri = ",uri )
      s = uri;
   }

   //DE_DEBUG("makeAbsolute(",uri,") = ",s)
   return s;
}

/*
std::string
FileSystem::makeAbsolute( std::string const & uri, std::string const & baseDir )
{
   if ( baseDir.empty() )
   {
      return makeAbsolute( uri );
   }

   fs::path puri( uri );
   if ( puri.is_absolute() )
   {
      DE_DEBUG("uri(",uri,") is already absolute!")
      return makeAbsolute( uri );
   }

   std::string tmp = uri;

   try
   {
      fs::path dir( baseDir );
      if ( !dir.is_absolute() )
      {
         throw std::runtime_error( "BaseDir must be absolute" );
      }

      tmp = fs::absolute( dir.concat( uri ) ).string();
   }
   catch ( std::exception & e )
   {
      DE_DEBUG("Got filesystem exception ", e.what(), ", uri = ", uri )
      tmp = uri;
   }

   return dbMakePosix( tmp );
}
*/

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

std::vector< std::string >
FileSystem::loadTextLnFromMemory( char const * data )
{
   std::string content = data;
   dbStrReplace( content, "\r\n", "\n" );
   dbStrReplace( content, "\n\r", "\n" );
   std::vector< std::string > lines = dbStrSplit( content, '\n' );
   return lines;
}


std::vector< std::string >
FileSystem::loadTextLines( std::string const & uri )
{
   std::string content = loadText( uri );
   dbStrReplace( content, "\r\n", "\n" );
   dbStrReplace( content, "\n\r", "\n" );
   std::vector< std::string > lines = dbStrSplit( content, '\n' );
   return lines;
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
FileSystem::existFile( std::wstring const & uri )
{
   if ( uri.empty() ) { return false; }
   fs::file_status s = fs::status( fs::path(uri) );
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

bool
FileSystem::existDirectory( std::wstring const & fileName )
{
   if ( fileName.empty() ) return false;
   fs::file_status s = fs::status( fileName );
   bool ok = fs::exists( s ) && fs::is_directory( s );
   return ok;
}

std::string
FileSystem::parentDir( std::string const & uri )
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
   dbStrReplace( tmp, "\\", "/" );
   if ( dbStrEndsWith( tmp, "/" ) )
   {
      tmp = tmp.substr( 0, tmp.size() - 1 );
   }
   return tmp;
}

std::wstring
FileSystem::parentDir( std::wstring const & uri )
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

   std::wstring tmp = p.wstring();
   dbStrReplace( tmp, L"\\", L"/" );
   if ( dbStrEndsWith( tmp, L"/" ) )
   {
      tmp = tmp.substr( 0, tmp.size() - 1 );
   }
   return tmp;
}

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

   dbStrLowerCase( ext );
   return ext;
}

std::wstring
FileSystem::fileSuffix( std::wstring const & uri )
{
   if ( uri.empty() )
   {
      return {};
   }

   std::wstring ext = fs::path( uri ).extension().wstring();
   if ( ext.empty() )
   {
      return {};
   }

   if (ext[0] == L'.')
   {
      ext.erase( 0, 1 );
   }

   dbStrLowerCase( ext );
   return ext;
}

bool
FileSystem::isAbsolute( std::string const & uri )
{
   return fs::path( uri ).is_absolute();
}

bool
FileSystem::isAbsolute( std::wstring const & uri )
{
   return fs::path( uri ).is_absolute();
}

std::string
FileSystem::fileBase( std::string const & uri )
{
   return fs::path( uri ).stem().string();
}

std::wstring
FileSystem::fileBase( std::wstring const & uri )
{
   return fs::path( uri ).stem().wstring();
}

int64_t
FileSystem::fileSize( std::string const & uri )
{
   return int64_t( fs::file_size( uri ) );
}

int64_t
FileSystem::fileSize( std::wstring const & uri )
{
   return int64_t( fs::file_size( uri ) );
}

std::string
FileSystem::fileName( std::string const & uri )
{
   //DE_TRACE("uri = ", uri)
   //std::string s1 = fs::path( uri ).filename().string();
   //std::string s2 = fs::path( uri ).stem().string();
   //DE_TRACE("fs::path( uri ).filename() = ", uri)
   //DE_TRACE("fs::path( uri ).stem() = ", s2.size())
   auto fn = fs::path( uri ).filename().string();
   if ( fn.empty() ) return uri;
   return fn;
}

std::wstring
FileSystem::fileName( std::wstring const & uri )
{
   auto fn = fs::path( uri ).filename().wstring();
   if ( fn.empty() ) return uri;
   return fn;
}

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
      std::cout << "CopiedFile(" << src << "," << dst << ")" << std::endl;
      return true;
   }
   else
   {
      std::cout << "[Error] CopyFile(" << src << "," << dst << ")" << std::endl;
      return false;
   }
}

bool
FileSystem::copyDirectory( std::string const & src, std::string const & dst )
{
//   std::error_code e;
//   fs::create_directories( fs::path( uri ), e );
//   if ( e )
//   {
//      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
//   }
   return false;
}

void
FileSystem::entryList(  std::string baseDir, bool recursive, bool withFiles, bool withDirs,
               std::function< void( std::string const & ) > const & userFunc )
{
   baseDir = FileSystem::makeAbsolute( baseDir );
   if ( !FileSystem::existDirectory( baseDir ) )
   {
      return;
   }

   if ( recursive )
   {
      fs::recursive_directory_iterator it( baseDir );
      while ( it != fs::recursive_directory_iterator() )
      {
         fs::path p = it->path();
         std::string absUri = FileSystem::makeAbsolute( p.string() );
         dbStrReplace( absUri, "\\", "/" ); // make posix path

         if ( withDirs && fs::is_directory( p ) )
         {
            userFunc( absUri );
         }

         if ( withFiles && fs::is_regular_file( p ) )
         {
            userFunc( absUri );
         }

         std::error_code ec;
         it.increment( ec );
         if ( ec )
         {
            // DE_ERROR("Recursive find : ",absUri," :: ",ec.message() )
            break;
         }
      }
   }
   else
   {
      fs::directory_iterator it( baseDir );
      while ( it != fs::directory_iterator() )
      {
         fs::path p = it->path();
         std::string absUri = FileSystem::makeAbsolute( p.string() );
         dbStrReplace( absUri, "\\", "/" ); // make posix path

         if ( withDirs && fs::is_directory( p ) )
         {
            userFunc( absUri );
         }

         if ( withFiles && fs::is_regular_file( p ) )
         {
            userFunc( absUri );
         }

         std::error_code ec;
         it.increment( ec );
         if ( ec )
         {
            // DE_ERROR("Iterative find : ",absUri," :: ",ec.message() )
            break;
         }
      }
   }
}

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
      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
   }
}


void
FileSystem::removeFile( std::string const & uri )
{
   std::error_code e;
   fs::remove( uri, e );
   if ( e )
   {
      std::cout << "[Error] " << __func__ << "(" << uri << ") :: " << e.message() << std::endl;
   }
}

void
FileSystem::removeFile( std::wstring const & uri )
{
   std::error_code e;
   fs::remove( uri, e );
   if ( e )
   {
      std::wcout << "[Error] " << __func__ << "(" << uri << ") :: " << e.message().c_str() << std::endl;
   }
}

void
FileSystem::removeDirectory( std::string const & uri )
{
   std::error_code e;
   fs::remove_all( fs::path( uri ), e );
   if ( e )
   {
      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got error " << e.message() << std::endl;
   }
   fs::remove( uri, e );
   if ( e )
   {
      std::cout << "[Error] " << __func__ << "( uri:" << uri << ") :: Got 2nd error " << e.message() << std::endl;
   }
}

} // end namespace de

size_t
dbGetEntryCount( std::string const & uri, bool recursive, bool withFiles, bool withDirs )
{
   size_t entryCount = 0;
   de::FileSystem::entryList( uri, recursive, withFiles, withDirs,
      [&] ( std::string const & ) { ++entryCount; } );
   // DE_DEBUG("Found (", entryCount, ") entries in uri(", uri, "), recursive(", recursive, ")" )
   return entryCount;
}

std::vector< std::string >
dbGetFilesAndDirs( std::string const & uri, bool recursive, bool withFiles, bool withDirs )
{
   size_t entryCount = dbGetEntryCount( uri, recursive, withFiles, withDirs );
   if ( entryCount < 1 )
   {
      return {};
   }

   std::vector< std::string > fileList;
   fileList.reserve( entryCount );
   de::FileSystem::entryList( uri, recursive, withFiles, withDirs,
         [&] ( std::string const & f ) { fileList.emplace_back( f ); } );
   // DE_DEBUG("Found (", entryCount, ") entries in uri(", uri, "), recursive(", recursive, ")" )
   return fileList;
}

std::vector< std::string >
dbGetDirs( std::string const & uri, bool recursive )
{
   return dbGetFilesAndDirs( uri, recursive, false, true );
}

std::vector< std::string >
dbGetFiles( std::string const & uri, bool recursive )
{
   return dbGetFilesAndDirs( uri, recursive, true, false );
}

std::vector< std::string >
dbGetFilteredFiles( std::string const & uri, bool recursive, std::vector< std::string > const & exts )
{
   if ( uri.empty() )
   {
      return {};
   }

   if ( exts.empty())
   {
      return dbGetFiles( uri, recursive );
   }

   std::vector< std::string > files;

   de::FileSystem::entryList( uri, recursive, true, false,
      [&] ( std::string const & fileName )
      {
         std::string const ext = de::FileSystem::fileSuffix( fileName );

         int32_t found = -1;

         for ( size_t i = 0; i < exts.size(); ++i )
         {
            if ( ext == exts[ i ] )
            {
               found = int32_t( i );
               break;
            }
         }

         if ( found > -1 )
         {
            // DE_DEBUG( "Found file(", files.size(),") with extension(", ext, ") in fileName(", fileName, ")")
            files.emplace_back( fileName ); // extension is in given list.
         }
      }
   );

   //DE_DEBUG("Found (",files.size(),") files with given exts(", exts.size(), ")" )
   return files;
}

std::string
dbFirstSubDirectory( std::string const & uri, bool recursive )
{
   de::fs::path p( uri );
   if ( de::fs::is_regular_file( p ) )
   {
      p = p.parent_path();
   }
   if ( !de::fs::is_directory( p ) )
   {
      return "";
   }
   if ( !de::fs::exists( p ) )
   {
      return "";
   }

   if ( recursive )
   {
      de::fs::recursive_directory_iterator it( p );
      while ( it != de::fs::recursive_directory_iterator() )
      {
         de::fs::path p = it->path();
         if ( de::fs::is_directory( p ) )
         {
            return de::FileSystem::makeAbsolute( p.string() );
         }
         std::error_code ec;
         it.increment( ec );
         if ( ec )
         {
            break;
         }
      }
   }
   else
   {
      de::fs::directory_iterator it( p );
      while ( it != de::fs::directory_iterator() )
      {
         de::fs::path p = it->path();
         if ( de::fs::is_directory( p ) )
         {
            return de::FileSystem::makeAbsolute( p.string() );
         }
         std::error_code ec;
         it.increment( ec );
         if ( ec )
         {
            break;
         }
      }
   }
   return "";
}

#if 0

std::vector< std::string >
dbGetAbsoluteFileNames( std::vector< std::string > fileNames, std::string baseDir )
{
   // make filenames absolute using dir from given filename
   //std::string baseDir = getAbsoluteFilePath( fileName );

   makePosixPath( baseDir );

   if ( baseDir.empty() )
   {
      baseDir = dbCWD();
   }

   if ( !endsWith( baseDir, "/" ) )
   {
      baseDir += "/";
   }

   for ( size_t i = 0; i < fileNames.size(); ++i )
   {
      std::string & fileName = fileNames[ i ];
      fileName = dbMakeAbsolute( baseDir + fileName );
   }

   return fileNames;
}

std::string
dbStringifyFilePermsissions( fs::perms const perm )
{
   std::stringstream s;
   s << ((perm & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
     << ((perm & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
     << ((perm & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
     << ((perm & fs::perms::group_read) != fs::perms::none ? "r" : "-")
     << ((perm & fs::perms::group_write) != fs::perms::none ? "w" : "-")
     << ((perm & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
     << ((perm & fs::perms::others_read) != fs::perms::none ? "r" : "-")
     << ((perm & fs::perms::others_write) != fs::perms::none ? "w" : "-")
     << ((perm & fs::perms::others_exec) != fs::perms::none ? "x" : "-");
   return s.str();
}

#endif
