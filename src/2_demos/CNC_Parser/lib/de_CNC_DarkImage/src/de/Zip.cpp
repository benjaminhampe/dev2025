#include <de/Zip.hpp>
#include <minizip/zip.h>
#include <iostream>

bool
Zipper::createZipFile( std::string zipFileName, std::string inputFileOrDir )
{
   return true;
}

bool
Zipper::extractZipFile( std::string zipFile, std::string outDir )
{
   if ( zipFile.empty() )
   {
      return false;
   }

   if ( outDir.empty() )
   {
      outDir = zipFile+".tmpdir";
   }

   int r = zip_extract( zipFile.c_str(), outDir.c_str(), nullptr, nullptr );
   DE_DEBUG("zip_extract RESULT = ",r)
   return true;
}
