#ifndef UNICODE

#include <de/audio/GetCoverArt.hpp>

#include <de/Core.h>
#include <de/os/CommonDialogs.hpp>
// #include <de/audio/buffer/BufferIO.hpp>

namespace de {
namespace audio {

std::string
repairString( std::wstring loadUri )
{
   //std::wstring w = dbStrW( loadUri );
   //std::wcout << "LoadUri = " << w << L'\n';

   std::stringstream s;
   for ( size_t i = 0; i < loadUri.size(); ++i )
   {
      uint32_t c = loadUri[ i ];
      if ( c <= 30 ) c = '_';
      if ( c > 'z' ) c = '_';
      s << char( c );
   }

   std::string r = s.str();
   std::cout << "RepairedUri = " << r << std::endl;
   return r;

}

struct extractCoverArt
{
   DE_CREATE_LOGGER("extractCoverArt")

   static void
   run( int argc, char** argv )
   {
      std::wstring loadUri;
//      if ( argc > 1 )
//      {
//         loadUri = argv[1];
//      }
      loadUri = dbOpenFileDlgW(
         L"Load file (mp3,wav,mkv,avi,mp4,etc..) that is supported by libAVCodec ( vlc )",
         0,0,800,600,
         L"",
         L"../../", true );

      std::wstring saveUri;
//      if ( argc > 2 )
//      {
//         saveUri = argv[ 2 ];
//      }

      CoverArt coverArt;
      CoverArt :: getCoverArt( coverArt, loadUri );


      PerformanceTimer perf;
      perf.start();


      perf.stop();

//      if ( img )
//      {
//         DE_DEBUG("Loaded CoverArt img(",img->toString(),") after ",loopCounter," loops. ",loadUriSTL )
//         dbSaveImage( *img, saveUri );
//         return true;
//      }
//      else
//      {
//         DE_ERROR("No CoverArt after ",loopCounter," loops. ",loadUriSTL )
//         return false;
//      }

//      DE_FLUSH

   }

};

} // end namespace audio
} // end namespace de


//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   de::audio::extractCoverArt::run( argc, argv );
   return 0;
}

#endif
