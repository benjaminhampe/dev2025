#include <DarkImage.hpp>

namespace de {

struct convertImage
{
   DE_CREATE_LOGGER("convertImage")

   static int
   run( int argc, char** argv )
   {
      std::string loadUri;

      if ( argc > 1 )
      {
         loadUri = argv[ 1 ];
      }

      if ( !de::FileSystem::existFile( loadUri ) )
      {
         loadUri = dbOpenFileDlgW( L"Load/open any image file (bmp,xpm,png,tga,jpg,tif,gif,dds,etc..)" );
      }

      if ( !de::FileSystem::existFile( loadUri ) )
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
   //bool ok =
   de::convertImage::run( argc, argv );
   return 0;
}

