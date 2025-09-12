#include <de/ffmpeg/GetCoverArt.hpp>
#include <de/ffmpeg/File.hpp>

struct CoverArtGrabber
{
   static bool
   getCoverArt( de::Image & img, std::string uri, int streamIndex )
   {
      DE_DEBUG("--------- dbGetCoverArt(",uri,") ----------")

      std::unique_ptr< de::ffmpeg::File > file( new de::ffmpeg::File() );
      if ( !file->open( uri ) )
      {
         DE_ERROR("Cant open uri ",uri )
         return false;
      }

      if ( streamIndex < 0 )
      {
         streamIndex = file->getBestVideoStreamIndex();
      }

      if ( !file->isStream( streamIndex ) )
      {
         DE_ERROR("No video stream")
         return false;
      }

      file->disableStreams();
      file->enableStream( streamIndex );

      bool foundCoverArt = false;

      auto appendToBufferList =
         [&]( std::unique_ptr< de::ffmpeg::FilePayload > payload )
         {
            if ( payload->streamIndex == streamIndex &&
                 payload->typ == AVMEDIA_TYPE_VIDEO &&
                 payload->img )
            {
               img = *payload->img;
               foundCoverArt = true;
            }
         };

      while ( !foundCoverArt )
      {
         int e = file->readFrame( appendToBufferList );
         if ( e == AVERROR_EOF )
         {
            break;
         }
      }

      if ( !foundCoverArt )
      {
         DE_ERROR("No coverArt found in ", uri)
      }
      else
      {
         DE_DEBUG("Found coverArt ", img.str() )
      }

      return foundCoverArt;
   }
};

bool
dbGetCoverArt( de::Image & img, std::string uri, int streamIndex )
{
   return CoverArtGrabber::getCoverArt( img, uri, streamIndex );
}
