/*
 *  FileSystem.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#include "FileSystem.hpp"

// STL C++17
// #include <filesystem>
// namespace fs = std::filesystem;
// GCC 7.3.0 needs extra link with -lstdc++fs
#include <experimental/filesystem> // I use GCC 7.3.0 and link with -lstdc++fs

#include <fstream>
//#include <iostream>

namespace de {


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

namespace fs = std::experimental::filesystem;


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

} // end namespace de
