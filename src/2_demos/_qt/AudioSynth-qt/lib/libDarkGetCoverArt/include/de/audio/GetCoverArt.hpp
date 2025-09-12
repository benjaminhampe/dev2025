#pragma once
#include <DarkImage.h>
//#include <audio.FileInfo.h>

namespace de {
namespace audio {

   struct CoverArt
   {
      static bool
      getCoverArt( CoverArt & coverArt, std::wstring const & uri );

      enum eCoverArtType
      {
         Other              = 0x00,
         FileIcon           = 0x01,
         OtherFileIcon      = 0x02,
         FrontCover         = 0x03,
         BackCover          = 0x04,
         LeafletPage        = 0x05,
         Media              = 0x06,
         LeadArtist         = 0x07,
         Artist             = 0x08,
         Conductor          = 0x09,
         Band               = 0x0A,
         Composer           = 0x0B,
         Lyricist           = 0x0C,
         RecordingLocation  = 0x0D,
         DuringRecording    = 0x0E,
         DuringPerformance  = 0x0F,
         MovieScreenCapture = 0x10,
         ColouredFish       = 0x11,
         Illustration       = 0x12,
         BandLogo           = 0x13,
         PublisherLogo      = 0x14,
         CoverArtTypeCount  = 0x15
      };


//      char const * const s_CoverArtType[] = {
//         "Other",                                  // 0
//         "32x32 pixels 'file icon' (PNG only)",    // 1
//         "Other file icon",                        // 2
//         "Cover (front)",                          // 3
//         "Cover (back)",                           // 4
//         "Leaflet page",                           // 5
//         "Media (e.g. label side of CD)",
//         "Lead artist/lead performer/soloist",
//         "Artist/performer",
//         "Conductor",
//         "Band/Orchestra",
//         "Composer",
//         "Lyricist/text writer",
//         "Recording Location",
//         "During recording",
//         "During performance",
//         "Movie/video screen capture",
//         "A bright coloured fish",
//         "Illustration",
//         "Band/artist logotype",
//         "Publisher/Studio logotype",
//         "ECoverArtTypeCount"
//      };


      //eFileMagic fileMagic;
      //std::string uri;       // --> mostly a temporary file to create

      //std::string TextEncoding;
      //std::string MimeType;      // Dont trust the mime type, trust the filemagic of data.
      //std::string PicType;
      //std::string Description;
      //char* PicData;
      std::string uri;  // Temporary created fileName, is not unicode.
      de::Image img;


   };

   // // ============================================================================
   // struct CoverArtList
   // // ============================================================================
   // {
   //    std::vector< CoverArt > covers;
   // };

} // end namespace audio
} // end namespace de

