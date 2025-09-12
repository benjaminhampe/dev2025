#pragma once
#include <de_core/de_StringUtil.h>
#include <de_core/de_FileSystem.h>
//#include <de/CommonDialogs.hpp>

//#include <cstdint>
//#include <cstdlib>
//#include <cstdio>
//#include <vector>
//#include <sstream>
//#include <fstream>

namespace de {

// ============================================================================
struct FileToHeader
// ============================================================================
{
   DE_CREATE_LOGGER("de.FileToHeader")

   static bool
   writeHeaderFromMemory( std::string const & dataName, uint8_t const* bytes, size_t byteCount )
   {
      std::ostringstream o; o <<
      "#pragma once\n"
      "\n"
      "static uint8_t const " << dataName << "[" << byteCount << "] = \n"
      "{\n"
      "\t";

      uint64_t k = 0;
      for ( size_t i = 0; i < byteCount; ++i )
      {
         o << "0x" << StringUtil::hex( bytes[ i ] );
         if ( byteCount > 1 && (i + 1 < byteCount) )
         {
            o << ", ";
         }

         // Better formatting
         ++k;
         if ( k >= 16 )
         {
            k -= 16;
            o << "\n\t";
            // o << " // " << i << "\n\t";
         }
      }

      o << "};\n";

      std::string const saveUri = dataName + ".h";

      bool const ok = FileSystem::saveText( saveUri, o.str() );
      if ( ok )
      {
         DE_DEBUG("Saved C++ header file ", saveUri)
      }
      else
      {
         DE_ERROR("Cant save C++ header file ", saveUri)
      }

      return ok;
   }

   static bool
   writeHeaderFromFile( std::string dataName, std::string loadUri )
   {
      std::vector< uint8_t > byteVector;
      if ( !FileSystem::loadByteVector( byteVector, loadUri ) )
      {
         DE_ERROR("Cant load ", loadUri)
         return false;
      }

      return writeHeaderFromMemory( dataName, byteVector.data(), byteVector.size() );
   }
};

} // end namespace de.

/*
//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   if ( argc < 2 )
   {
      std::cout << "\n"
      "[Usage]: argv[0] = " << argv[0] << "\n"
      "\n"
      "\te.g. $ file2header.exe G:/arial.ttf arial_ttf G:/embedded_arial_ttf.hpp\n"
      "\n"
      "\targv[1] = loadUri ( G:/arial.ttf ) <- the binary to convert to embedded hpp code\n"
      "\targv[2] = hppName ( fontawesome_ttf ) <- used in static uint8_t const 'hppName'[42] = { 0xAB, 0x00, etc }\n"
      "\targv[3] = saveUri ( G:/embedded_arial.hpp ) <- the Output file .hpp \n"
      "\n"
      "[End]\n"
      "\n";
   }

   std::wstring exeFile = dbStrW( std::string( argv[ 0 ] ) );
   std::wstring loadUri;

   if ( argc > 1 )
   {
      loadUri = dbStrW( std::string( argv[ 1 ] ) );
   }

   if ( !dbExistFile( loadUri ) )
   {
      loadUri = dbOpenFileDlgW( L"Load/open any image file (bmp,xpm,png,tga,jpg,tif,gif,dds,etc..)" );
   }

   if ( !dbExistFile( loadUri ) )
   {
      return 0;
   }

   std::wstring hppName = de::FileSystem::fileBase( loadUri ) + L"_hpp";
   std::wstring saveUri = loadUri + L".hpp";


   uint64_t embeddedByteCount =
      de::FileToHeader::createHeaderFile( dbStr(loadUri),
                                          dbStr(hppName),
                                          dbStr(saveUri) );
   std::wcout << "\n[Exec]:\n"
   "\n"
   "\targv[ 0 ] ExeFile = " << exeFile << "\n"
   "\targv[ 1 ] LoadUri = " << loadUri << "\n"
   "\targv[ 2 ] hppName = " << hppName << "\n"
   "\targv[ 3 ] SaveUri = " << saveUri << "\n"
   "\n"
   "EmbeddedByteCount = " << embeddedByteCount << " in hpp.\n"
   "\n";

   return 0;
}

   if ( argc > 2 )
   {
      saveUri = dbStrW( std::string( argv[ 2 ] ) );
   }

   if ( saveUri.empty() )
   {
      int dw = dbDesktopWidth() - 400;
      int dh = dbDesktopHeight() - 400;
      int dx = 200;
      int dy = 200;
      saveUri = dbSaveFileDlgW(
         L"Save image (bmp,png,jpg,gif,tga,dds,tif)",
         L"All Files (*.*)\0*.*\0",
         L"",
         L"",
         true,
//            "Bitmap (*.bmp)\0*.bmp\0"
//            "Portable Network Graphic (*.png)\0*.png\0"
//            "JPEG (*.jpg)\0*.jpg\0"
//            "Graphic interchange format (*.gif)\0*.gif\0"
//            "TrueVision (*.tga)\0*.tga\0"
//            "DDS (*.dds)\0*.dds\0"
//            "Tif (*.tif)\0*.tif\0"
//            "\0",
         dx,dy,dw,dh );
   }

#include <DarkImage.hpp>

namespace de {

struct convertImage
{
   DE_CREATE_LOGGER("convertImage")

   static int
   run( int argc, char** argv )
   {
      std::wstring loadUri;

      if ( argc > 1 )
      {
         loadUri = dbStrW( std::string( argv[ 1 ] ) );
      }

      if ( !dbExistFile( loadUri ) )
      {
         loadUri = dbOpenFileDlgW( L"Load/open any image file (bmp,xpm,png,tga,jpg,tif,gif,dds,etc..)" );
      }

      if ( !dbExistFile( loadUri ) )
      {
         DE_ERROR("Nothing todo")
         return 0;
      }

      std::wstring saveUri;

      if ( argc > 2 )
      {
         saveUri = dbStrW( std::string( argv[ 2 ] ) );
      }

      if ( saveUri.empty() )
      {
         int dw = dbDesktopWidth() - 400;
         int dh = dbDesktopHeight() - 400;
         int dx = 200;
         int dy = 200;
         saveUri = dbSaveFileDlgW(
            L"Save image (bmp,png,jpg,gif,tga,dds,tif)",
            L"All Files (*.*)\0*.*\0",
            L"",
            L"",
            true,
//            "Bitmap (*.bmp)\0*.bmp\0"
//            "Portable Network Graphic (*.png)\0*.png\0"
//            "JPEG (*.jpg)\0*.jpg\0"
//            "Graphic interchange format (*.gif)\0*.gif\0"
//            "TrueVision (*.tga)\0*.tga\0"
//            "DDS (*.dds)\0*.dds\0"
//            "Tif (*.tif)\0*.tif\0"
//            "\0",
            dx,dy,dw,dh );
      }

      DE_DEBUG("Got saveUri = ", dbStr(saveUri))

      //DE_DEBUG("[LoadMP3] loadUri = ", loadUri )
      //Buffer workBuffer;

      de::PerformanceTimer perf;
      perf.start();

      de::Image img;
      bool ok = dbLoadImage( img, dbStr( loadUri ) );
      perf.stop();
      DE_DEBUG("[LoadImage] needed ", perf.ms(), " ms, uri = ", dbStr(loadUri) )

      if ( ok )
      {
         perf.start();
         ok = dbSaveImage( img, dbStr( saveUri ) );
         perf.stop();
         DE_DEBUG("[SaveImage] needed ", perf.ms(), " ms, uri = ", dbStr(saveUri) )

         if ( !ok )
         {
            std::wcout << "Cant save image " << saveUri << "\n";
         }
      }
      else
      {
         std::wcout << "Cant load image " << loadUri << "\n";
      }

      DE_DEBUG("[ConvertImage] program end." )
      return ok;
   }
};

} // end namespace de

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   bool ok = de::convertImage::run( argc, argv );
   return 0;
}

*/
