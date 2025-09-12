#ifndef DE_AUDIO_ID3V1_HPP
#define DE_AUDIO_ID3V1_HPP

#include <cstdint>
#include <de/audio/de_taglib.hpp>
#include <de/Logger.hpp>

namespace de {
namespace audio {

// ============================================================================
struct ID3_Waveformat
// ============================================================================
{
   uint32_t channels = 0;
   uint32_t sampleRate = 0;
   double duration = 0.0;
   uint32_t bitRate = 0;
};

inline bool
scanID3_WaveFormat( ID3_WaveFormat & out, TagLib::AudioProperties* in)
{
   if ( !in ) return false;
   out.channelCount = in->channels();
   out.sampleRate = in->sampleRate();
   out.duration = in->length();
   out.bitRate = in->bitrate();
   return true;
}

// ============================================================================
struct ID3v1
// ============================================================================
{
   ID3v1()
      : m_hasID3v1( false ), m_year(0), m_trackNr( 0 ) {}

   bool m_hasID3v1;
   int m_year;
   int m_trackNr;
   std::wstring m_title;
   std::wstring m_artist;
   std::wstring m_album;
   std::wstring m_comment;
   std::wstring m_genre;

   // std::string toString() const;
   bool
   ok() const { return m_hasID3v1; }

   bool
   write( std::wstring const & uri ) const { return writeID3v1( *this, uri ); }

   bool
   read( TagLib::ID3v1::Tag* tag ) { return readID3v1( *this, tag ); }

   static bool
   readID3v1( ID3v1 & out, TagLib::ID3v1::Tag* tag )
   {
      if ( !tag ) return false;
      out.m_hasID3v1 = true;
      out.m_year = (int)tag->year();
      out.m_trackNr = (int)tag->track();
      out.m_album = tag->album().toWString();
      out.m_artist = tag->artist().toWString();
      out.m_genre = tag->genre().toWString();
      out.m_title = tag->title().toWString();
      out.m_comment = tag->comment().toWString();
      return true;
   }

   static bool
   hasID3v1( std::wstring const & uri )
   {
      TagLib::MPEG::File file( uri.c_str() );
      if ( !file.isOpen() || !file.isValid() )
      {
         DE_ERROR("No file open")
         return false;
      }

      return file.hasID3v1Tag();
   }

   static bool
   writeID3v1( ID3v1 const & dat, std::wstring const & uri )
   {
      TagLib::MPEG::File file( uri.c_str() );
      if ( !file.isOpen() || !file.isValid() )
      {
         DE_ERROR("Cant open file")
         return false;
      }

      auto tag = file.ID3v1Tag( true );
      if ( !tag )
      {
         DE_ERROR("Cant create id3v1 tag")
         return false;
      }
      tag->setAlbum( TagLib::String( dat.m_album ) );
      tag->setArtist( TagLib::String( dat.m_artist ) );
      tag->setGenre( TagLib::String( dat.m_genre ) );
      //tag->setGenreNumber( TagLib::String( dat.m_genre ) );
      tag->setComment( TagLib::String( dat.m_comment ) );
      tag->setTitle( TagLib::String( dat.m_title ) );
      tag->setYear( dat.m_year );
      tag->setTrack( dat.m_trackNr );
      //tag->setProperties(  );

      DE_ERROR("Wrote id3v1 tag")
      return true;
   }
};

} // end namespace audio
} // end namespace de

#endif
