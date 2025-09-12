#include <de/image/font/FontManager.h>

// #ifdef HAVE_FREETYPE2

#include <de/image/font/FontTTF.h>
//#include <DarkImage.h>

void dbPrepareFont(de::Font font, std::string uri )
{
    de::FontManager::getInstance()->addFamily(uri, font);
}

void dbPrepareFont(de::Font font, const uint8_t* dataPtr, uint64_t dataSize )
{
    de::FontManager::getInstance()->addFamily(font, dataPtr, dataSize);
}

namespace de {

bool
FontManager::addFamily( Font font, const uint8_t* dataPtr, uint64_t dataSize )
{
    int32_t found = findFont( font );
    if ( found > -1 )
    {
        //FontFamily family = m_Families[ found ];
        DE_DEBUG("Font already cached(",found,"), family(",font.family(),")")
        return true; // Already added
    }

    int32_t foundFamily = findFamily( font.family() );
    if ( foundFamily < 0 )
    {
        DE_DEBUG(font.family(),", font(",font.toString(),")" )
        FontFamily fontFamily;
        fontFamily.family = font.family();
        //fontFamily.uri = uri;
        fontFamily.dataPtr = dataPtr;
        fontFamily.dataSize = dataSize;
        foundFamily = int32_t( m_Families.size() );
        m_Families.push_back( fontFamily );
    }

    if ( foundFamily < 0 )
    {
        DE_DEBUG("No new <font-family>(", font.family(),"), font(",font.toString(),")" )
        return false;
    }

    return true;
}

// ===========================================================================
// implementation of the VIP font atlas getter defined in <de/Font.hpp>
// ===========================================================================

void
saveFonts()
{
   FontManager::getInstance()->saveFonts();
}

std::shared_ptr< IFontAtlas >
getFontFace( Font font )
{
   return FontManager::getInstance()->getFont( font );
}

// ===========================================================================
// FontManager
// ===========================================================================

FontManager::FontManager()
   // : m_ftLib( nullptr )
{
   //DE_DEBUG("Constructor")
   //   FT_Error e = FT_Init_FreeType( &m_ftLib );
   //   if ( e != 0 )
   //   {
   //      DE_ERROR("FT_Init_FreeType() failed.")
   //      m_ftLib = nullptr;
   //   }

//   uint32_t w = 96;
//   uint32_t h = 48;
//   de::Image img( w,h );
//   for ( size_t j = 0; j < h; ++j )
//   {
//      uint32_t color = de::RainbowColor::computeColor32( float(j)/float(h-1) );
//      for ( size_t i = 0; i < w; ++i )
//      {
//         img.setPixel( i,j,color );
//      }
//   }

//   std::cout << dumpANSIConsoleImage( img ) << std::endl;

//   dbSaveImage( img, "_FontManager_test_001.png");
//   dbSaveImage( img, "_FontManager_test_001.jpg");
//   dbSaveImage( img, "_FontManager_test_001.tga");
//   dbSaveImage( img, "_FontManager_test_001.xpm");
//   dbSaveImage( img, "_FontManager_test_001.tif");
//   dbSaveImage( img, "_FontManager_test_001.gif");
//   dbSaveImage( img, "_FontManager_test_001.ico");
//   dbSaveImage( img, "_FontManager_test_001.dds");
//   dbSaveImage( img, "_FontManager_test_001.webp");
}

FontManager::~FontManager()
{
   //DE_DEBUG("Destructor")
   //   if ( m_ftLib )
   //   {
   //      FT_Error e = FT_Done_FreeType( m_ftLib );
   //      if ( e != 0 )
   //      {
   //         // DE_ERROR("FT_Done_FreeType() failed.")
   //      }
   //      m_ftLib = nullptr;
   //   }
   //saveFonts();
}

// static
std::shared_ptr< FontManager >
FontManager::getInstance()
{
   static std::shared_ptr< FontManager > s_FontManager( new FontManager() );
   return s_FontManager;
}

int32_t
FontManager::findFamily( std::string family ) const
{
   StringUtil::lowerCase( family );

   for ( size_t i = 0; i < m_Families.size(); ++i )
   {
      if ( m_Families[ i ].family == family )
      {
         return int32_t( i );
      }
   }
   return -1;
}

FontFamily
FontManager::getFamily( std::string family ) const
{
    StringUtil::lowerCase( family );

    for ( size_t i = 0; i < m_Families.size(); ++i )
    {
       if ( m_Families[ i ].family == family )
       {
          return m_Families[ i ];
       }
    }

    std::ostringstream o;
    o << "No FontFamily " << family;
    throw std::runtime_error(o.str());
    // DE_ERROR(o.str())
    // return FontFamily();
}

int32_t
FontManager::findFont( Font const & font ) const
{
   for ( size_t i = 0; i < m_Fonts.size(); ++i )
   {
      if ( m_Fonts[ i ] && m_Fonts[ i ]->getFont() == font )
      {
         return int32_t( i );
      }
   }
   return -1;
}



bool
FontManager::addFamily( std::string uri, Font font )
{
   int32_t found = findFont( font );
   if ( found > -1 )
   {
      //FontFamily family = m_Families[ found ];
       DE_DEBUG("<font-family> already cached(",found,"), family(",font.family(),")")
      return true; // Already added
   }

   int32_t foundFamily = findFamily( font.family() );
   if ( foundFamily < 0 )
   {
      DE_DEBUG(font.family(),", uri(",uri,"), font(",font.toString(),")" )
      FontFamily fontFamily;
      fontFamily.family = font.family();
      fontFamily.uri = uri;
      foundFamily = int32_t( m_Families.size() );
      m_Families.push_back( fontFamily );
   }

   if ( foundFamily < 0 )
   {
      DE_DEBUG("No new <font-family>(", font.family(),"), uri(",uri,"), font(",font.toString(),")" )
      return false;
   }


   return true;
}


std::shared_ptr< IFontAtlas >
FontManager::getFont( Font font )
{
    int32_t found = findFont( font );
    if ( found > -1 )
    {
        //DE_DEBUG("Cache hit ", found)
        return m_Fonts[ found ];
    }

    int32_t foundFamily = findFamily( font.family() );
    if ( foundFamily < 0 )
    {
        DE_ERROR("No such font family ", font.family() )
        DE_ERROR(dumpStrFamilies() )
        return nullptr;
    }

    // Create Atlas
    std::shared_ptr< FontTTF > face = std::make_shared< FontTTF >( font );

    auto const family = m_Families[ foundFamily ];

    // Load from memory...
    if (family.dataPtr && family.dataSize)
    {
        face->openMemory( family.dataPtr, family.dataSize );
    }
    // Load from file...
    else if (!family.uri.empty())
    {
        face->open( family.uri );
    }
    // Error...
    else
    {
        DE_ERROR("No data or uri for font ", font.toString())
    }

    if ( !face->is_open() )
    {
        DE_ERROR("FontFace not opened(", font.toString(), ")")
        return nullptr;
    }

    DE_DEBUG("Added font(", font.toString(), "), family(", family.toString(), ")")
    m_Fonts.push_back( face );
    return m_Fonts.back();
}

void
FontManager::saveFonts()
{
   //DE_DEBUG("Save Fonts..." )
   //DE_DEBUG(dumpStrFamilies())

   // Find in Cache
   for ( size_t i = 0; i < m_Fonts.size(); ++i )
   {
      std::shared_ptr< IFontAtlas > cached = m_Fonts[ i ];
      if ( cached )
      {
         //DE_DEBUG("CachedFont[",i,"] ", cached->getFont().toFontString() )
         cached->saveAtlas();
      }
   }
}

std::string
FontManager::dumpStrFamilies() const
{
   std::ostringstream s;
   s << "FamilyCount = " << m_Families.size() << "\n";
   for ( size_t i =0 ; i < m_Families.size(); ++i )
   {
      s << "Family[" << i << "] " << m_Families[ i ].toString() << "\n";
   }
   return s.str();
}

} // end namespace de.

// #endif
