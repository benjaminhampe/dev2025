#pragma once
#include <cstdint>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <de/Logger.hpp>

namespace de {

struct FileSystem
{
   DE_CREATE_LOGGER("de.FileSystem")

   static std::string
   loadText( std::string const & uri );

   static std::vector< std::string >
   loadTextLines( std::string const & uri );

   static std::vector< std::string >
   loadTextLnFromMemory( char const * data );

   static bool
   saveText( std::string const & uri, std::string const & txt );

   static bool
   existFile( std::string const & uri );

   static bool
   existFile( std::wstring const & uri );

   static bool
   existDirectory( std::string const & dir );

   static bool
   existDirectory( std::wstring const & dir );

   static void
   removeFile( std::string const & uri );

   static void
   removeFile( std::wstring const & uri );

   static std::string
   parentDir( std::string const & uri );

   static std::wstring
   parentDir( std::wstring const & uri );

   inline static std::string
   fileDir( std::string const & uri ) { return parentDir( uri ); }

   inline static std::wstring
   fileDir( std::wstring const & uri ) { return parentDir( uri ); }

   static bool
   isAbsolute( std::string const & uri );

   static bool
   isAbsolute( std::wstring const & uri );

   static std::string
   fileBase( std::string const & uri );

   static std::wstring
   fileBase( std::wstring const & uri );

   static int64_t
   fileSize( std::string const & uri );

   static int64_t
   fileSize( std::wstring const & uri );

   static std::string
   fileName( std::string const & uri );

   static std::wstring
   fileName( std::wstring const & uri );

   static std::string
   fileSuffix( std::string const & uri );

   static std::wstring
   fileSuffix( std::wstring const & uri );

   static std::string
   cwd(bool makePosix = true);

   static std::string
   caturi( std::string const & a, std::string const & b );

   static std::string
   makeAbsolute( std::string uri, std::string baseDir = "" );

//   static std::string
//   makeAbsolute( std::string const & uri, std::string const & baseDir );

   static bool
   copyFile( std::string src, std::string dst );

   static bool
   copyDirectory( std::string const & src, std::string const & dst );

   static void
   entryList( std::string baseDir, bool recursive, bool withFiles, bool withDirs,
              std::function< void( std::string const & ) > const & userFunc );

   static void
   createDirectory( std::string const & uri );

   static void
   removeDirectory( std::string const & uri );


};

} // end namespace de

// DarkGDK API
//inline std::string dbMakeAbsolute( std::string const & uri )
//{
//   return de::FileSystem::makeAbsolute( uri );
//}

//inline std::string dbMakeAbsolute( std::string const & uri, std::string const & baseDir )
//{
//   return de::FileSystem::makeAbsolute( uri, baseDir );
//}

/*
inline std::string dbLoadText( std::string const & uri )
{ return de::FileSystem::loadText( uri ); }
inline bool dbSaveText( std::string const & uri, std::string const & txt )
{ return de::FileSystem::saveText( uri, txt ); }
inline std::vector< std::string > dbLoadTextLn( std::string const & uri )
{ return de::FileSystem::loadTextLines( uri ); }
inline std::vector< std::string > dbLoadTextLnFromMemory( char const * xpm_data )
{ return de::FileSystem::loadTextLnFromMemory( xpm_data ); }
inline bool dbExistFile( std::string const & uri )
{ return de::FileSystem::existFile( uri ); }
inline bool dbExistFile( std::wstring const & uri )
{ return de::FileSystem::existFile( uri ); }
inline void dbRemoveFile( std::string const & uri )
{ de::FileSystem::removeFile( uri ); }
inline void dbRemoveFile( std::wstring const & uri )
{ de::FileSystem::removeFile( uri ); }

inline bool dbExistDirectory( std::string const & uri )
{ return de::FileSystem::existDirectory( uri ); }
inline void dbMakeDirectory( std::string const & uri )
{ de::FileSystem::createDirectory( uri ); }
inline void dbRemoveDirectory( std::string const & uri )
{ de::FileSystem::removeDirectory( uri ); }
inline std::string dbGetParentDir( std::string uri )
{ return de::FileSystem::parentDir( uri ); }
inline std::string dbGetFileDir( std::string const & uri )
{ return de::FileSystem::parentDir( uri ); }
inline std::string dbGetFileSuffix( std::string const & uri )
{ return de::FileSystem::fileSuffix( uri ); }
inline bool dbIsAbsolute( std::string const & uri )
{ return de::FileSystem::isAbsolute( uri ); }
inline std::string dbGetFileBaseName( std::string const & uri )
{ return de::FileSystem::fileBase( uri ); }
inline int64_t dbGetFileSize( std::string const & uri )
{ return de::FileSystem::fileSize( uri ); }
inline std::string dbGetFileName( std::string const & uri )
{ return de::FileSystem::fileName( uri ); }
inline std::string dbCWD(bool makePosix = true)
{ return de::FileSystem::cwd(makePosix); }
inline bool dbCopyFile( std::string const & src, std::string const & dst )
{ return de::FileSystem::copyFile( src, dst ); }
inline bool dbCopyDirectory( std::string const & src, std::string const & dst )
{ return de::FileSystem::copyDirectory( src, dst ); }
inline void dbGetEntries(  std::string baseDir, bool recursive, bool withFiles, bool withDirs,
         std::function< void( std::string const & ) > const & userFunc )
{ de::FileSystem::entryList( baseDir, recursive, withFiles, withDirs, userFunc ); }

*/
size_t
dbGetEntryCount( std::string const & uri, bool recursive, bool withFiles, bool withDirs );

std::vector< std::string >
dbGetFilesAndDirs( std::string const & uri, bool recursive, bool withFiles, bool withDirs );

std::vector< std::string >
dbGetDirs( std::string const & uri, bool recursive );

std::vector< std::string >
dbGetFiles( std::string const & uri, bool recursive );

std::vector< std::string >
dbGetFilteredFiles(
   std::string const & uri,
   bool recursive,
   std::vector< std::string > const & exts );

inline std::vector< std::string >
dbGetImageFiles( std::string const & uri, bool recursive )
{
   std::vector< std::string > const imageExt {
   "jpg", "jpeg", "j2k", "bmp", "png", "gif", "tga", "dds", "psd", "pcx", "pic"
   };
   return dbGetFilteredFiles( uri, recursive, imageExt );
}

inline std::vector< std::string >
dbGetAudioFiles( std::string const & uri, bool recursive )
{
   std::vector< std::string > const audioExt {
      "mp3", "wma", "wav", "flac", "ogg", "m4a"
   };
   return dbGetFilteredFiles( uri, recursive, audioExt );
}

inline std::vector< std::string >
dbGetMovieFiles( std::string const & uri, bool recursive )
{
   std::vector< std::string > const videoExt {
      "flv", "mkv", "mp4", "mpeg2", "avi", "3gp", "rm"
   };
   return dbGetFilteredFiles( uri, recursive, videoExt );
}

inline std::vector< std::string >
dbGetModelFiles( std::string const & uri, bool recursive )
{
   std::vector< std::string > const modelExt {
      "x", "dxf", "dwg", "stl", "step", "3ds", "f360z", "cob", "cob"
   };
   return dbGetFilteredFiles( uri, recursive, modelExt );
}

inline std::vector< std::string >
dbGetSVGFiles( std::string const & uri, bool recursive )
{
   std::vector< std::string > const svgExt {
      "svg"
   };
   return dbGetFilteredFiles( uri, recursive, svgExt );
}

inline std::vector< std::string >
dbGetPDFFiles( std::string const & uri, bool recursive )
{
   std::vector< std::string > const pdfExt {
      "pdf"
   };
   return dbGetFilteredFiles( uri, recursive, pdfExt );
}
