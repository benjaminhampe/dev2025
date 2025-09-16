#ifndef DE_AUDIO_ID3V2_HPP
#define DE_AUDIO_ID3V2_HPP

#include <de/audio/de_taglib.hpp>
#include <de/audio/GetCoverArt.hpp>

namespace de {
namespace audio {

// ============================================================================
struct ID3v2
// ============================================================================
{
   bool m_hasID3v2 = false;
   int m_layer = -1;
   int m_versionMajor = -1;
   int m_versionMinor = -1;

   bool m_isVBR = false;
   bool m_isCompressed = false;
   bool m_isEncrypted = false;
   bool m_hasCRC = false;
   bool m_hasCopyright = false;
   bool m_hasOriginal = false;

   int m_bitRate = -1;
   int m_year = -1;
   int m_trackNr = -1;
   std::wstring m_title;
   std::wstring m_artist;
   std::wstring m_album;
   std::wstring m_comment;
   std::wstring m_genre;

   std::wstring m_composer;
   std::wstring m_publisher;
   std::wstring m_mixArtist;
   std::wstring m_measure;
   std::wstring m_key;

   std::wstring m_copyright;
   std::wstring m_software;
   std::wstring m_date;
   std::wstring m_license;
   std::wstring m_ranking;
   int m_rating = -1;
   float m_bpm = -1.0f;
   // int silencePos;
   //  QString MediaName;
   //  int DeleteFile; --> Konflikt with Win32 API macro
   //  int DiskID;
   //  int FileID;
   int m_playCounter = -1;
   int m_timesPlayed = -1;
   //CDateTime lastPlayed;
   //std::wstring dummyS4
   //uint32_t color
   //int originalSortOrder; // for playlists
   //int dummyI4

   CoverArt m_coverArt;
   //CuePoints cuePoints
   //FadeParams fadeParams

   //ID3v2FrameList FrameList;

public:
   //ID3v2();
   //~ID3v2();
   //void clear();
   //std::string toString() const;
   //std::string toXML() const;

   // std::string toString() const;
   bool
   ok() const { return m_hasID3v2; }

   bool
   write( std::wstring const & uri ) const { return writeID3v2( *this, uri ); }

   bool
   read( TagLib::ID3v1::Tag* tag ) { return readID3v2( *this, tag ); }


   static bool
   readTALB( ID3v2 & out, TagLib::ID3v2::TextIdentificationFrame* in )
   {
      if ( !in ) return false;

      auto album = in->toString().toWString();
      DE_DEBUG("Found TALB = ", album)

      if ( out.m_album.empty() || out.m_album.size() < album.size() )
      {
         DE_DEBUG("Overwrite existing Album = ", out.m_album)
         out.m_album = album;
      }

      return true;
   }

   static bool
   readTCOM( ID3v2 & out, TagLib::ID3v2::TextIdentificationFrame* in )
   {
      if ( !in ) return false;

      auto composer = in->toString().toWString();
      DE_DEBUG("Found TCOM = ", composer)

      if ( out.m_composer.empty() || out.m_composer.size() < composer.size() )
      {
         DE_DEBUG("Overwrite existing composer = ", out.m_composer)
         out.m_composer = composer;
      }

      return true;
   }

   static bool
   readTCOP( ID3v2 & out, TagLib::ID3v2::TextIdentificationFrame* in )
   {
      if ( !in ) return false;

      auto copyright = in->toString().toWString();
      DE_DEBUG("Found TCOP = ", copyright)

      if ( out.m_copyright.empty() || out.m_copyright.size() < copyright.size() )
      {
         DE_DEBUG("Overwrite existing copyright = ", out.m_copyright)
         out.m_copyright = copyright;
      }

      return true;
   }

   /*
     *   <li><b>TALB</b> Album/Movie/Show title</li>
     *   <li><b>TBPM</b> BPM (beats per minute)</li>
     *   <li><b>TCOM</b> Composer</li>
     *   <li><b>TCON</b> Content type</li>
     *   <li><b>TCOP</b> Copyright message</li>
     *   <li><b>TDEN</b> Encoding time</li>
     *   <li><b>TDLY</b> Playlist delay</li>
     *   <li><b>TDOR</b> Original release time</li>
     *   <li><b>TDRC</b> Recording time</li>
     *   <li><b>TDRL</b> Release time</li>
     *   <li><b>TDTG</b> Tagging time</li>
     *   <li><b>TENC</b> Encoded by</li>
     *   <li><b>TEXT</b> Lyricist/Text writer</li>
     *   <li><b>TFLT</b> File type</li>
     *   <li><b>TIPL</b> Involved people list</li>
     *   <li><b>TIT1</b> Content group description</li>
     *   <li><b>TIT2</b> Title/songname/content description</li>
     *   <li><b>TIT3</b> Subtitle/Description refinement</li>
     *   <li><b>TKEY</b> Initial key</li>
     *   <li><b>TLAN</b> Language(s)</li>
     *   <li><b>TLEN</b> Length</li>
     *   <li><b>TMCL</b> Musician credits list</li>
     *   <li><b>TMED</b> Media type</li>
     *   <li><b>TMOO</b> Mood</li>
     *   <li><b>TOAL</b> Original album/movie/show title</li>
     *   <li><b>TOFN</b> Original filename</li>
     *   <li><b>TOLY</b> Original lyricist(s)/text writer(s)</li>
     *   <li><b>TOPE</b> Original artist(s)/performer(s)</li>
     *   <li><b>TOWN</b> File owner/licensee</li>
     *   <li><b>TPE1</b> Lead performer(s)/Soloist(s)</li>
     *   <li><b>TPE2</b> Band/orchestra/accompaniment</li>
     *   <li><b>TPE3</b> Conductor/performer refinement</li>
     *   <li><b>TPE4</b> Interpreted, remixed, or otherwise modified by</li>
     *   <li><b>TPOS</b> Part of a set</li>
     *   <li><b>TPRO</b> Produced notice</li>
     *   <li><b>TPUB</b> Publisher</li>
     *   <li><b>TRCK</b> Track number/Position in set</li>
     *   <li><b>TRSN</b> Internet radio station name</li>
     *   <li><b>TRSO</b> Internet radio station owner</li>
     *   <li><b>TSOA</b> Album sort order</li>
     *   <li><b>TSOP</b> Performer sort order</li>
     *   <li><b>TSOT</b> Title sort order</li>
     *   <li><b>TSRC</b> ISRC (international standard recording code)</li>
     *   <li><b>TSSE</b> Software/Hardware and settings used for encoding</li>
     *   <li><b>TSST</b> Set subtitle</li>
   */
   static bool
   readTBPM( ID3v2 & out, TagLib::ID3v2::TextIdentificationFrame* in )
   {
      if ( !in ) return false;
      out.m_bpm = atof( toString( in->toString() ).c_str() );
      DE_DEBUG("Found TBPM = ", out.m_bpm)
      return true;
   }

   static bool
   readID3v2( ID3v2 & out, TagLib::ID3v2::Tag* tag )
   {
      if ( !tag ) return false;
      out.m_hasID3v2 = true;
      out.m_year = (int)tag->year();
      out.m_trackNr = (int)tag->track();
      out.m_album = tag->album().toWString();
      out.m_artist = tag->artist().toWString();
      out.m_genre = tag->genre().toWString();
      out.m_title = tag->title().toWString();
      out.m_comment = tag->comment().toWString();

      // Loop frames
      TagLib::ID3v2::FrameList const & frameList = tag->frameList();
      for ( TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); it++)
      {
         TagLib::ID3v2::Frame* frame = *it;
         if ( !frame ) continue;
         auto frameId = TagLib::String( frame->frameID() );
         if ( frameId == "APIC" )
         {
            CoverArt::readAPIC( out.m_coverArt, dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frame) );
         }
         else if ( frameId == "TBPM" )
         {
            readTBPM( out, dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(frame) );
         }
      }
      return true;
   }

   static bool
   hasID3v2( std::wstring const & uri )
   {
      TagLib::MPEG::File file( uri.c_str() );
      if ( !file.isOpen() || !file.isValid() )
      {
         DE_ERROR("No file open")
         return false;
      }

      return file.hasID3v2Tag();
   }

   static bool
   writeID3v2( ID3v2 const & dat, std::wstring const & uri )
   {
      TagLib::MPEG::File file( uri.c_str() );
      if ( !file.isOpen() || !file.isValid() )
      {
         DE_ERROR("Cant open file")
         return false;
      }

      auto tag = file.ID3v2Tag( true );
      if ( !tag )
      {
         DE_ERROR("Cant create id3v2 tag")
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

      DE_ERROR("Wrote id3v2 tag, no CoverArt yet")
      return true;
   }
};

/*

// ============================================================================
class Property
// ============================================================================
{
public:
    std::string Key;
    std::vector<std::string> Data;

    Property();

    std::string toString() const;
    std::string toXML() const;

};

// ============================================================================
class PropertyList : public std::vector<Property>
// ============================================================================
{
public:
    std::string toString() const;
    std::string toXML() const;

};
// ============================================================================
struct ID3v2Frame
// ============================================================================
{
    std::string FrameID;   ///< The ID3v2 text field id, like TALB, TPE2
    std::string Lang; ///< Three-letter language code
    std::string Description; ///< Empty for the generic comment...
    std::string Text;        ///< ...
    int Version;

    std::string toString() const;
    std::string toXML() const;
};

// ============================================================================
class ID3v2FrameList : public std::vector<ID3v2Frame>
// ============================================================================
{
public:
    std::string toString() const;
    std::string toXML() const;

};


*/

} // end namespace audio
} // end namespace de

#endif
