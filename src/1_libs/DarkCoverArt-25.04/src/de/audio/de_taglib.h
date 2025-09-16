#pragma once
#include <cstdint>
#include <sstream>
#include <vector>
#include <map>

// #ifdef USE_TAGLIB

#include <taglib.h>
#include <fileref.h>
#include <audioproperties.h>
#include <tpropertymap.h>
#include <tbytevector.h>
#include <mpeg/mpegfile.h>
#include <mpeg/mpegheader.h>
#include <mpeg/mpegproperties.h>
#include <mpeg/xingheader.h>
#include <mpeg/id3v1/id3v1genres.h>
#include <mpeg/id3v1/id3v1tag.h>
#include <mpeg/id3v2/id3v2tag.h>
#include <mpeg/id3v2/id3v2header.h>
#include <mpeg/id3v2/id3v2extendedheader.h>
#include <mpeg/id3v2/id3v2footer.h>
#include <mpeg/id3v2/id3v2frame.h>
#include <mpeg/id3v2/id3v2framefactory.h>
#include <mpeg/id3v2/id3v2synchdata.h>
#include <mpeg/id3v2/frames/attachedpictureframe.h>
//#include <mpeg/id3v2/frames/chapterframe.h>
#include <mpeg/id3v2/frames/commentsframe.h>
//#include <mpeg/id3v2/frames/eventtimingcodesframe.h>
#include <mpeg/id3v2/frames/generalencapsulatedobjectframe.h>
#include <mpeg/id3v2/frames/ownershipframe.h>
#include <mpeg/id3v2/frames/popularimeterframe.h>
#include <mpeg/id3v2/frames/privateframe.h>
#include <mpeg/id3v2/frames/relativevolumeframe.h>
//#include <mpeg/id3v2/frames/synchronizedlyricsframe.h>
//#include <mpeg/id3v2/frames/tableofcontentsframe.h>
#include <mpeg/id3v2/frames/textidentificationframe.h>
#include <mpeg/id3v2/frames/uniquefileidentifierframe.h>
#include <mpeg/id3v2/frames/unknownframe.h>
#include <mpeg/id3v2/frames/unsynchronizedlyricsframe.h>
#include <mpeg/id3v2/frames/urllinkframe.h>
#include <mp4/mp4tag.h>
#include <mp4/mp4atom.h>
#include <mp4/mp4coverart.h>
#include <mp4/mp4file.h>
#include <mp4/mp4item.h>
#include <mp4/mp4properties.h>

namespace de {

   struct TagLibTools
   {
      static std::string
      toString( TagLib::String const & in )
      {
         if ( in.isEmpty() ) return "";

         #ifdef _WIN32
         return in.to8Bit(true); // Windows ( UTF-16 = Unicode )
         #else
         return in.to8Bit(false); // Linux ( UTF-8 = Multibyte )
         #endif
      }
   };

   // std::string toString( TagLib::ID3v2::AttachedPictureFrame::Type type );
   // std::vector<std::string> toStringList( const TagLib::StringList& liste );
   // std::vector<std::string> toStringList( const TagLib::PropertyMap& map );
   // std::string toString( const TagLib::PropertyMap& map );

   // typedef std::map<std::string, std::string> StdMap;
   // void propertyMap_to_stlMap( StdMap& dst, const TagLib::PropertyMap& src);

} // end namespace de
