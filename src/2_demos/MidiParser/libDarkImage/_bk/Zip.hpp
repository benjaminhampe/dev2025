//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <de/Logger.hpp>

struct Zipper
{
   DE_CREATE_LOGGER("Zipper")

   /// @param In FileName of zip file to decompress
   /// @param In Output directory of decompressed zip file.
   /// @return true for success, else false.
   static bool
   createZipFile( std::string zipFileName, std::string inputFileOrDir );

   /// @param In FileName of zip file to decompress
   /// @param In Output directory of decompressed zip file.
   /// @return true for success, else false.
   static bool
   extractZipFile( std::string zipFile, std::string outDir = "" );

};
