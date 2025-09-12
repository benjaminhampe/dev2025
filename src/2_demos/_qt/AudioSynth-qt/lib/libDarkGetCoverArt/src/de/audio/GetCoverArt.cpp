#include <de/audio/GetCoverArt.hpp>
#include <de/audio/de_taglib.hpp>

namespace de {
namespace audio {

struct CoverArtUtils
{
   static std::string
   fileSuffixFromImageMagic( TagLib::ByteVector const & in )
   {
      if ( in.size() < 8 ) return "";
      uint32_t const MAGIC_JPG 		= 0xFFD8FF00;
      //uint32_t const MAGIC_JPG_JFIF = 0x4A464946;
      //uint32_t const MAGIC_JPG_Exif = 0x45786966;
      //uint32_t const MAGIC_JPG_EXIF = 0x45584946;
      uint32_t const MAGIC_PNG_1 	= 0x89504E47;
      uint32_t const MAGIC_PNG_2 	= 0x0D0A1A0A;
      uint16_t const MAGIC_BMP 		= 0x424D;
      uint16_t const MAGIC_BMP2 		= 0x24D4;
      uint32_t const MAGIC_GIF_1 	= 0x47494638;
      uint16_t const MAGIC_GIF_2 	= 0x3761;
      uint16_t const MAGIC_GIF_3 	= 0x3961;
      uint32_t const MAGIC_TIFF 		= 0x49204900;
      uint32_t const MAGIC_TIFF_LE 	= 0x49492A00;
      uint32_t const MAGIC_TIFF_BE 	= 0x4D4D002A;
      uint32_t mA = in.mid(0,4).toUInt();
      uint32_t mB = in.mid(5,4).toUInt();
      if ( (mA & 0xFFFFFF00) == MAGIC_JPG )
      {
         return "jpg";
      }
      else if ( (mA == MAGIC_PNG_1) && (mB == MAGIC_PNG_2) )
      {
         return "png";
      }
      else if ( ((mA & 0xFFFF) == MAGIC_BMP) || ((mA & 0xFFFF) == MAGIC_BMP2) )
      {
         return "bmp";
      }
      else if ( (mA == MAGIC_GIF_1) && (((mB & 0xFFFF) == MAGIC_GIF_2) || ((mB & 0xFFFF) == MAGIC_GIF_3)) )
      {
         return "gif";
      }
      else if ( (mA == MAGIC_TIFF_LE) || (mA == MAGIC_TIFF_BE) || ((mA & 0xFFFFFF00) == MAGIC_TIFF) )
      {
         return "tif";
      }

      DE_ERROR("No magic found")
      return "";
   }

   static bool
   readAPIC( CoverArt & coverArt, TagLib::ID3v2::AttachedPictureFrame* apic )
   {
      if ( !apic ) return false;
      TagLib::ByteVector const & bytes = apic->picture();
      //if ( !bytes.data() ) return;
      if ( bytes.size() < 1 ) return false;
      std::string ext = fileSuffixFromImageMagic( bytes );     // --> check later
      std::string uri = "CoverArt." + ext;

   //         FILE* file = fopen( uri.c_str(), "w+b"); // Write extracted image to temporary file
   //         if ( file )
   //         {
   //            fwrite( bytes.data(), 1, bytes.size(), file );
   //            fclose( file );
   //         }

      //out->Description = toString( in->description() );
      //out->MimeType = toString( in->mimeType() );
      //out->PicType = toString( in->type() );

      auto pSrc = reinterpret_cast< uint8_t const* >( bytes.data() );
      if ( dbLoadImage( coverArt.img, pSrc, bytes.size(), uri ) )
      {
          DE_DEBUG("coverArt(",coverArt.img.str(),")")
         return true;
      }
      else
      {
         return false;
      }
   }
};

bool
CoverArt::getCoverArt( CoverArt & coverArt, std::wstring const & uri )
{
//       ID3v2::Tag* tag = f.ID3v2Tag();
//       if ( tag )
//       {
//          cout << "ID3v2."
//          << tag->header()->majorVersion()
//          << "."
//          << tag->header()->revisionNumber()
//          << ", "
//          << tag->header()->tagSize()
//          << " bytes in tag"
//          << endl;

//       ID3v2::FrameList::ConstIterator it = tag->frameList().begin();
//       for ( ; it != tag->frameList().end(); it++ )
//       {
//          cout << (*it)->frameID() << " - \"" << (*it)->toString() << "\"" << endl;
//       }


   TagLib::MPEG::File file( uri.c_str() );
   if ( !file.isOpen() || !file.isValid() )
   {
      return false;
   }

   auto id3v2 = file.ID3v2Tag(false);
   if (!id3v2) return false;

   auto const & frameList = id3v2->frameList();
   auto itBegin = frameList.begin();
   auto itEnd = frameList.end();
   //auto it = itBegin;
   for ( TagLib::ID3v2::FrameList::ConstIterator it = itBegin; it != itEnd; it++ )
   {
      TagLib::ID3v2::Frame* frame = *it;
      if ( frame )
      {
         if ( TagLib::String( frame->frameID() ) == "APIC" )
         {
            CoverArtUtils::readAPIC( coverArt, dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frame) );
         }
      }
   }

   return true;
}


} // end namespace audio
} // end namespace de





#if 0
TagList scanMP3_TagList( const std::string& fileName );

/// specialized for MP3 files
//ID3v1*          scanMP3_ID3v1( ID3v1* out, TagLib::ID3v1::Tag* tag );
//ID3v2*          scanMP3_ID3v2( ID3v2* out, TagLib::ID3v1::Tag* tag );

void scanMP3_ID3v1( FileInfo* out, TagLib::ID3v1::Tag* tag );
void scanMP3_ID3v2( FileInfo* out, TagLib::ID3v2::Tag* tag );
void scanMP3_TBPM( FileInfo* out, TagLib::ID3v2::TextIdentificationFrame* in );
void scanMP3_TIPL( FileInfo* out, TagLib::ID3v2::PopularimeterFrame* in );
void scanMP3_APIC( bpmCoverArt* out, TagLib::ID3v2::AttachedPictureFrame* in);
void scanMP3_CoverArt( bpmCoverArt* out, TagLib::ID3v2::Tag* tag );
void scanMP3_FileInfo( FileInfo* out, const std::string& fileName );

/// scans all types of audio-files
void scanWaveFormat( WaveFormat* out, TagLib::AudioProperties* in);

void scanFileInfo( FileInfo* out, const std::string& fileName );

FileInfo* createAndScanFileInfo( const std::string& fileName );
//IFile*      scanFile( IFile* file, const std::string& fileName );



void test_TagScanner();

namespace audio {

// ============================================================================
void scanFileInfo( FileInfo* out, const std::string& fileName )
// ============================================================================
{
    if (!out)
        return;

    out->FileName = getAbsoluteFileName(fileName);

    if (isFileMP3(fileName))
    {
        scanMP3_FileInfo(out, fileName);
    }
/*
    else if (isFileM4A(fileName))
    {
        // scanM4A_FileInfo(out, fileName);
    }
    else if (isFileOGG(fileName))
    {
        // scanOGG_FileInfo(out, fileName);
    }
    else if (isFileWAV(fileName))
    {
        // scanWAV_FileInfo(out, fileName);
    }
    else if (isFileFLAC(fileName))
    {
        // scanFLAC_FileInfo(out, fileName);
    }
    else if (isFileAAC(fileName))
    {
        // scanAAC_FileInfo(out, fileName);
    }
*/
    else
    {
        out->IsScanned = false;
        return;
    }

    if (out->Comment.size() > 0)
    {
        replace( out->Comment, "\r\n", "\n" );
        replace( out->Comment, "\n\r", "\n" );
        replace( out->Comment, "\n\n", "\n" );
        replace( out->Comment, "\n", " " );
        replace( out->Comment, "  ", " " );
    }

    out->IsScanned = true;
}

// ============================================================================
void scanMP3_FileInfo( FileInfo* out, const std::string& fileName )
// ============================================================================
{
    if (!out)
        return;

    TagLib::MPEG::File f( fileName.c_str());
    if (!f.isOpen())
    {
        /// MMError("File is not open\n");
        return;
    }

    if (!f.isValid())
    {
        ///MMError("File is not valid\n");
        return;
    }

    scanWaveFormat( &out->Format, f.audioProperties() );
//    out.BitRate = -1;
//    out.BPM = -1.0f;
//    out.BPMAuto = false;
//    out.ChannelCount = -1;
    scanMP3_ID3v1( out, f.ID3v1Tag(false) );
    scanMP3_ID3v2( out, f.ID3v2Tag(false) );

    /// Auswertung ( nimm das Beste aus beiden Tags )
    const TagLib::PropertyMap& map = f.properties();
    std::stringstream s;
    s << toString(map) << "\n";
    //MMDebug(s.str().c_str())
    //out->Comment = s.str();

}


// ============================================================================
void scanMP3_CoverArt( bpmCoverArt* cover, const std::string& fileName )
// ============================================================================
{
    if (!cover)
        return;

    TagLib::MPEG::File file( fileName.c_str() );
    if ( !file.isOpen() || !file.isValid() )
    {
        return;
    }

    TagLib::ID3v2::Tag* tag = file.ID3v2Tag(false);
    if (!tag)
        return;

    const TagLib::ID3v2::FrameList& frameList = tag->frameList();
    for( TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); it++)
    {
        TagLib::ID3v2::Frame* frame = *it;
        if (frame)
        {
            if (TagLib::String(frame->frameID()) == "APIC")
            {
                scanMP3_APIC( cover, dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frame) );
            }
        }
    }
}

// ============================================================================
TagList scanMP3_TagList( const std::string& fileName )
// ============================================================================
{
    TagList tagList;

    TagLib::MPEG::File file( fileName.c_str() );

    if ( !file.isOpen() || !file.isValid() || !file.hasID3v2Tag() )
        return tagList;

    TagLib::ID3v2::Tag* id3tag = file.ID3v2Tag(false);
    if (!id3tag)
        return tagList;

    /// Loop all ID3 frames/tags
    const TagLib::ID3v2::FrameList& frames = id3tag->frameList();

    for( TagLib::ID3v2::FrameList::ConstIterator it = frames.begin();
         it != frames.end();  it++)
    {
        TagLib::ID3v2::Frame* frame = *it;
        if (frame)
        {
            std::string id = toString( TagLib::String(frame->frameID()) );
            std::string data = toString( frame->toString() );

            TAG tag;
            tag.ID = TagList::toTagType(id);
            tag.Data = id;
            tag.Data += "|";
            tag.Data += data;

            tagList.push_back( tag );
        }
    }

    return tagList;
}


// ============================================================================
void scanMP3_ID3v2( FileInfo* out, TagLib::ID3v2::Tag* tag )
// ============================================================================
{
    if (!out)
        return;

    if (!tag)
        return;

    out->HasID3v2 = true;

    { /// Album
        std::string tmp = toString( tag->album() );
        if (out->Album.size() < tmp.size())
            out->Album = tmp;
    }

    { /// Artist
        std::string tmp = toString( tag->artist() );
        if (out->Artist.size() < tmp.size())
            out->Artist = tmp;
    }

    { /// Genre
        std::string tmp = toString( tag->genre() );
        if (out->Genre.size() < tmp.size())
            out->Genre = tmp;
    }

    { /// Title
        std::string tmp = toString( tag->title() );
        if (out->Title.size() < tmp.size())
            out->Title = tmp;
    }

    { /// TrackNr
        int trackNo = (int)tag->track();
        if (out->TrackNr < trackNo)
            out->TrackNr = trackNo;
    }

    { /// Year
        int year = (int)tag->year();
        if (out->Year < year)
            out->Year = year;
    }

    { /// Comment
        std::string tmp = toString( tag->comment() );
        if (out->Comment.size() < tmp.size())
            out->Comment = tmp;
    }

    /// Loop all ID3 Frames

    const TagLib::ID3v2::FrameList& frameList = tag->frameList();
    for( TagLib::ID3v2::FrameList::ConstIterator it = frameList.begin(); it != frameList.end(); it++)
    {
        TagLib::ID3v2::Frame* frame = *it;
        if (frame)
        {
            if (TagLib::String(frame->frameID()) == "APIC")
                scanMP3_APIC( &out->CoverArt, dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frame) );

            if (TagLib::String(frame->frameID()) == "TBPM")
                scanMP3_TBPM( out, dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(frame) );
        }
    }
}

// ============================================================================
void scanMP3_TBPM( FileInfo* out, TagLib::ID3v2::TextIdentificationFrame* in )
// ============================================================================
{
    if (!out)
        return;

    if (!in)
        return;

    out->BPMValue = atof( toString(in->toString()).c_str() );
}

// ============================================================================
void scanMP3_APIC( bpmCoverArt* out, TagLib::ID3v2::AttachedPictureFrame* in)
// ============================================================================
{
    if (!out)
        return;

    if (!in)
        return;

    TagLib::ByteVector bytes = in->picture();

    if (!bytes.data())
        return;

    if (bytes.size() < 1)
        return;

    out->FileType = getFileType( bytes );     // --> check later

    std::string ext = getFileExtFromFileType( out->FileType );
    out->Description = toString( in->description() );
    out->MimeType = toString( in->mimeType() );
    out->PicType = toString( in->type() );

    std::stringstream s; s << os::getTempFileName() << ".";
    if (ext.empty())
        s << ".raw";
    else
        s << ext;

    out->FileName = s.str();

    if (!out->Image.loadFromData( (uchar*)bytes.data(), (int)bytes.size() ))
    {
        FILE* f = fopen( out->FileName.c_str(), "w+b");
        if (f)
        {
            /// Write extracted image to temporary file
            fwrite( bytes.data(), 1, bytes.size(), f);
            fclose( f );
        }
        else
        {
            /// Error
        }

        if (!existFile( out->FileName ))
        {
            /// Error
            return;
        }

        if (!out->Image.load( QString::fromStdString(out->FileName) ))
        {
            /// Error
            return;
        }
    }
}





} // end namespace audio

/**
// ============================================================================
EReturn scanFile_PropertyMap( TAudioFile& out, const TagLib::PropertyMap& in)
// ============================================================================
{
    if (in.isEmpty())
    {
        MMDebug( "File has no PropertyMap\n" );
        return RET_NOTHING_TODO;
    }

    for( TagLib::PropertyMap::ConstIterator it = in.begin(); it != in.end(); it++ )
    {
        const TagLib::StringList& data = it->second;
        const TagLib::uint count = data.size();

        TProperty property;
        property.Key = toString( it->first );
        for( TagLib::uint j = 0; j < count; j++ )
        {
            property.Data.push_back( toString( data[j] ) );
        }

        out.PropertyList.push_back( property );
    }

    return RET_SUCCESS;
}
*/

#endif
