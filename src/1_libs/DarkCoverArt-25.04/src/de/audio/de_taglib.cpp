#include <de/audio/de_taglib.h>

#if 0 // def USE_TAGLIB

#include <win32.FileSys.h>
#include <core.FileMagic.h>
#include <audio.ID3.h>

namespace de {

    // std::string toString( const TagLib::String& in );
    // std::string toString( TagLib::ID3v2::AttachedPictureFrame::Type type );
    // std::vector<std::string> toStringList( const TagLib::StringList& liste );
    // std::vector<std::string> toStringList( const TagLib::PropertyMap& map );
    // std::string toString( const TagLib::PropertyMap& map );

    // typedef std::map<std::string, std::string> StdMap;
    // void propertyMap_to_stlMap( StdMap& dst, const TagLib::PropertyMap& src);
    // std::string toString( const StdMap& map )

// ============================================================================


// ============================================================================
std::string toString( TagLib::ID3v2::AttachedPictureFrame::Type type )
// ============================================================================
{
    const u32 index = (u32)type;
    if (index >= (u32)CoverArtTypeCount )
        return "";

    return ECoverArtStrings[index];
}

// ============================================================================
std::vector<std::string> toStringList( const TagLib::StringList& liste )
// ============================================================================
{
    std::vector<std::string> result;

    const TagLib::uint itemCount = liste.size();

    std::stringstream s;
    s << "TagLib::StringList.size() = " << itemCount;
    result.push_back( s.str() );

    for( TagLib::uint i = 0; i < itemCount; i++ )
    {
        s.str("");
        s << "[" << i << "]=" << toString( liste[i] );
        result.push_back( s.str() );
    }

    return result;
}

// ============================================================================
std::vector<std::string> toStringList( const TagLib::PropertyMap& map )
// ============================================================================
{
    std::vector<std::string> result;

    std::stringstream s;

    TagLib::PropertyMap::ConstIterator it = map.begin();

    while ( it != map.end() )
    {
        const std::string mapKey = audio::toString( it->first );
        const TagLib::StringList& mapData = it->second;
        const TagLib::uint mapCount = mapData.size();

        s.str("");
        s << "Key=" << mapKey << " has " << mapCount << " values:";

        for( TagLib::uint j = 0; j < mapCount; j++ )
        {
            s << " [" << j << "]=" << toString( mapData[j] );
        }

        result.push_back( s.str() );
        it++;
    }
    return result;
}

// ============================================================================
std::string toString( const TagLib::PropertyMap& map )
// ============================================================================
{
    const std::vector<std::string> v = toStringList( map );

    std::stringstream s;
    s << "PropertyMap.size()=" << v.size() << "|";

    for ( size_t i = 0; i < v.size(); ++i )
    {
        s << v[i] << "|\n";
    }

    return s.str();
}

typedef std::map<std::string, std::string> StdMap;

// ============================================================================
void propertyMap_to_stlMap( StdMap& dst, const TagLib::PropertyMap& src)
// ============================================================================
{

    std::stringstream s;

    for( TagLib::PropertyMap::ConstIterator i = src.begin(); i != src.end(); i++ )
    {
        const std::string key = toString( i->first );
        const TagLib::StringList& lines = i->second;
        const TagLib::uint lineCount = lines.size();

        s.str("");
        //s << "Key=" << key << " has " << lineCount << " lines:";


        for( TagLib::uint j = 0; j < lineCount; j++ )
        {
            s << toString( lines[j] );
            if ( j < lineCount-1 ) s << "|";
        }

        dst[key] = s.str();
    }
}

//std::string toString( const StdMap& map )
//{
//    std::stringstream s;

//    for( size_t i = 0; i < map.size(); ++i )
//    {
//        const std::string key = audio::taglib::toString( i->first );
//        const TagLib::StringList& lines = i->second;
//        const TagLib::uint lineCount = lines.size();

//        s.str("");
//        //s << "Key=" << key << " has " << lineCount << " lines:";


//        for( TagLib::uint j = 0; j < lineCount; j++ )
//        {
//            s << audio::taglib::toString( lines[j] );
//            if ( j < lineCount-1 ) s << "|";
//        }

//        dst[key] = s.str();
//    }
//}


} // end namespace audio

#endif // USE_TAGLIB
