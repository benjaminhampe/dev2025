/*
 *  FileSystem.h
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#pragma once
#include <de_core/de_Logger.h>
#include <cstdint>
#include <sstream>
#include <vector>

namespace de {

/////////////////////////////////////////////////////////////////////
struct FileSystem
/////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("de.FileSystem")

   static std::string
   loadText( std::string const & uri );

   static bool
   saveText( std::string const & uri, std::string const & txt );

   static bool
   existFile( std::string const & uri );

   static bool
   existDirectory( std::string const & dir );

   // 1GB RAM limit (2^30) for a midi file, else regarded as error/malformed/broken.

   static bool
   loadByteVector( std::vector< uint8_t > & bv,
                   std::string const & uri,
                   uint64_t byteLimit = (uint64_t(1) << 30) );

   static std::string
   fileName( std::string const & uri );

   static std::string
   fileBase( std::string const & uri );

   static std::string
   fileSuffix( std::string const & uri );

   static std::string
   fileDir( std::string const & uri );

   static std::string
   makeAbsolute( std::string uri, std::string baseDir = "" );

   static void
   createDirectory( std::string const & uri );

   static void
   removeFile( std::string const & uri );

   static bool
   copyFile( std::string src, std::string dst );

   static std::string
   createUniqueFileName( std::string const & fileName = "untitled_file_" );
};

} // end namespace de
