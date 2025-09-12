#include <fn_copyImageRect.hpp>
#include <fn_genImageNormalMap.hpp>

using namespace de;

int main(int argc, char** argv )
{
   int x = 0;
   int y = 0;
   int w = 0;
   int h = 0;
   std::string loadUri;
   std::string saveUri;

   if ( argc < 3 )
   {
      std::cout << "[cutImage] Need atleast 2 user params: x y <w> <h> <in-file> <out-file>" << std::endl;
      return 0;
   }

   x = atol( argv[ 1 ] );
   y = atol( argv[ 2 ] );

   if ( argc > 3 )
   {
      w = atol( argv[ 3 ] );
   }

   if ( argc > 4 )
   {
      h = atol( argv[ 4 ] );
   }

   std::cout << "[cutImage] Got x = " << x << std::endl;
   std::cout << "[cutImage] Got y = " << y << std::endl;
   std::cout << "[cutImage] Got w = " << w << std::endl;
   std::cout << "[cutImage] Got h = " << h << std::endl;

   if ( argc > 5 )
   {
      loadUri = argv[ 5 ];
   }

   if ( !de::FileSystem::existFile( loadUri ) )
   {
      loadUri = dbOpenFileDlg( "Load image file (bmp,xpm,png,tga,jpg,tif,gif,dds,etc..)" );
   }

   if ( !de::FileSystem::existFile( loadUri ) )
   {
      std::cout << "[cutImage] No loadUri " << loadUri << std::endl;
      return 0;
   }

   std::cout << "[cutImage] Got loadUri = " << loadUri << std::endl;

   if ( argc > 6 )
   {
      saveUri = argv[ 6 ];
   }

   if ( saveUri.empty() )
   {
//      std::string caption, int x, int y, int w, int h,
//                     std::string filter,
//                     std::string initDir,
//                     std::string initFileName, bool newui)

      int dw = dbDesktopWidth() - 400;
      int dh = dbDesktopHeight() - 400;
      int dx = 200;
      int dy = 200;
      saveUri = dbSaveFileDlg("Save image (bmp,png,jpg,gif,tga,dds,tif)",
         dx,dy,dw,dh,
         "All Files (*.*)\0*.*\0", "", "",
         true );
      //            "Bitmap (*.bmp)\0*.bmp\0"
      //            "Portable Network Graphic (*.png)\0*.png\0"
      //            "JPEG (*.jpg)\0*.jpg\0"
      //            "Graphic interchange format (*.gif)\0*.gif\0"
      //            "TrueVision (*.tga)\0*.tga\0"
      //            "DDS (*.dds)\0*.dds\0"
      //            "Tif (*.tif)\0*.tif\0"
      //            "\0",
   }

   std::cout << "[cutImage] Got saveUri = " << saveUri << std::endl;

   de::PerformanceTimer perf;

   perf.start();
   de::Image img;
   bool ok = dbLoadImage( img, loadUri );
   perf.stop();

   std::cout << "[cutImage] LoadImage needed " << perf.ms() << " ms, "
                "loadUri = " << loadUri << std::endl;

   if ( !ok )
   {
      std::cout << "[cutImage] Cant load image. Nothing todo." << std::endl;
      return 0;
   }

   perf.start();
   Image subImage = copyImageRect( img, Recti(x,y,w,h) );
   perf.stop();

   std::cout << "[cutImage] CopyImage needed " << perf.us() << " us."
             << std::endl;

   perf.start();
   ok = dbSaveImage( subImage, saveUri );
   perf.stop();

   std::cout << "[cutImage] SaveImage needed " << perf.ms() << " ms, "
                "saveUri = " << saveUri << std::endl;

   if ( !ok )
   {
      std::cout << "[cutImage] Cant save image." << std::endl;
   }
   return 0;
}
