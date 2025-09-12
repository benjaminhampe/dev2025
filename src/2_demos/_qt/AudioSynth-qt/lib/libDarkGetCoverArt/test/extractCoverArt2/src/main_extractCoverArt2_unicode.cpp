#ifdef UNICODE

#include <de/AudioDecoderFFMPEG.hpp>
#include <de/VideoDecoderFFMPEG.hpp>

#include <de/Core.h>
#include <de/os/CommonDialogs.hpp>
// #include <de/audio/buffer/BufferIO.hpp>

namespace de {

struct extractCoverArt
{
   DE_CREATE_LOGGER("extractCoverArt")

   static bool
   run( int argc, wchar_t** argv )
   {
      std::wstring loadUri;
      if ( argc > 1 )
      {
         loadUri = argv[1];
      }

      if ( !dbExistFile(loadUri) )
      {
         loadUri = dbOpenFileDlgW(
            L"Load file (mp3,wav,mkv,avi,mp4,etc..) that is supported by libAVCodec ( vlc )",
            0,0,800,600,
            L"",
            L"../../", true );
      }

//      if ( !dbExistFile(loadUri) )
//      {
//         DE_ERROR("Empty loadUri, program exits now... Bye bye, loadUri = ", loadUri)
//         DE_FLUSH
//         return false;
//      }

      std::wstring saveUri;
      if ( argc > 2 )
      {
         saveUri = argv[ 2 ];
      }
      saveUri = loadUri + L".coverArt.png";

      //DE_DEBUG("[LoadMP3] loadUri = ", loadUri )
      //Buffer workBuffer;

      PerformanceTimer perf;
      perf.start();

      VideoFile file;
      if ( !file.open( loadUri, -1, true ) )
      {
         DE_ERROR("No decoder open, ",dbStr(loadUri))
         return false;
      }

      bool foundCoverArt = false;

      int loopCounter = 0;

      std::shared_ptr< Image > img;

      //auto handlePayload =

      while ( !foundCoverArt )
      {
         int e = file.readFrame(
            [&]( ImagePayload payload )
            {
               if ( payload.img && !payload.img->empty() )
               {
                  foundCoverArt = true;
                  img = payload.img;
                  loopCounter++;
               }
            }, true );
         if ( e == AVERROR_EOF )
         {
            break;
         }
      }

      file.close();

      perf.stop();

      if ( img )
      {
         DE_DEBUG("Loaded CoverArt img(",img->toString(),") after ",loopCounter," loops. ",dbStr(loadUri) )
         dbSaveImage( *img, dbStr(saveUri) );
         return true;
      }
      else
      {
         DE_ERROR("No CoverArt after ",loopCounter," loops. ",dbStr(loadUri) )
         return false;
      }

      DE_FLUSH

   }

};

} // end namespace de


//========================================================================
int main(int argc, wchar_t** argv)
//========================================================================
{
   bool ok = de::extractCoverArt::run( argc, argv );
   return 0;
}


#endif
