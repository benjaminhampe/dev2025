#include <irrExt/gui/FontManager.h>
#include <irrExt/gui/CGUITTFont.h>
// #include <irrExt/core/Logger.h>

namespace irrExt {

/*
TextSize
ImagePainter::getTextSize( std::wstring const & msg, Font font )
{
   auto face = getFontFace( font );
   if ( !face )
   {
      DE_RUNTIME_ERROR(font.toString(),"No font face")
      return {};
   }
   return face->getTextSize( msg );
}

void
ImagePainter::drawText( Image & img, int32_t x, int32_t y, std::wstring const & msg,
                        uint32_t color, Font font, Align align, bool blend )
{
   auto face = getFontFace( font );
   if ( !face )
   {
      DE_RUNTIME_ERROR(font.toString(),"No font face")
      return;
   }

   face->drawText( img, x,y, msg, color, align );
}
*/

// ===========================================================================
// FontManager
// ===========================================================================

void FontManager::setDevice( irr::IrrlichtDevice* device )
{
   m_device = device;
}

FontManager::FontManager()
   : m_device( nullptr )
{
   //dbDebug("Constructor")
   //   FT_Error e = FT_Init_FreeType( &m_ftLib );
   //   if ( e != 0 )
   //   {
   //      dbError("FT_Init_FreeType() failed.")
   //      m_ftLib = nullptr;
   //   }

//   uint32_t w = 96;
//   uint32_t h = 48;
//   DarkGDK::Image img( w,h );
//   for ( size_t j = 0; j < h; ++j )
//   {
//      uint32_t color = DarkGDK::RainbowColor::computeColor32( float(j)/float(h-1) );
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
   //dbDebug("Destructor")
   //saveFonts();

   for ( size_t i = 0; i < m_fonts.size(); ++i )
   {
      auto impl = m_fonts[ i ].impl;
      if ( impl ) delete impl;
   }
   m_fonts.clear();
}

// static
std::shared_ptr< FontManager >
FontManager::getInstance()
{
   static std::shared_ptr< FontManager > s_FontManager( new FontManager() );
   return s_FontManager;
}

int32_t
FontManager::findFamily( std::string const & family ) const
{
   auto const lowercaseFamily = Font::lowerCase( family );

   for ( size_t i = 0; i < m_families.size(); ++i )
   {
      if ( m_families[ i ].family == lowercaseFamily )
      {
         return int32_t( i );
      }
   }
   return -1;
}

int32_t
FontManager::findFont( Font const & font ) const
{
   for ( size_t i = 0; i < m_fonts.size(); ++i )
   {
      if ( m_fonts[ i ].font == font )
      {
         return int32_t( i );
      }
   }
   return -1;
}

bool
FontManager::addFamily( std::string const & uri, Font const & font )
{
   int32_t const fontIndex = findFont( font );
   if ( fontIndex > -1 )
   {
      //dbDebug("<font-family> already cached(",fontIndex,"), family(",font.family(),")")
      return true; // Already added
   }

   int32_t const familyIndex = findFamily( font.family() );
   if ( familyIndex < 0 )
   {
      //dbDebug("New <font-family>(", font.family(),"), uri(",uri,"), font(",font.toString(),")" )
      FontFamily fontFamily;
      fontFamily.family = font.family();
      fontFamily.uri = uri;
      m_families.push_back( fontFamily );
   }
   return true;
}


irr::gui::IGUIFont*
FontManager::getFont( Font const & font )
{
   int32_t const fontIndex = findFont( font );
   if ( fontIndex > -1 )
   {
      return m_fonts[ fontIndex ].impl; // Cache hit
   }

   int32_t const familyIndex = findFamily( font.family() );
   if ( familyIndex < 0 )
   {
      //dbError("No such font family ", font.family() )
      //dbError(dumpStrFamilies() )
      return nullptr;
   }

   if ( !m_device )
   {
      //dbError("No irrlicht device, call setDevice before using this class.")
      //dbError(dumpStrFamilies() )
      return nullptr;
   }

   auto const & uri = m_families[ familyIndex ].uri;

   // Create Atlas
   auto face = irr::gui::CGUITTFont::create( m_device,
                     uri.c_str(),
                     font.pixelSize(),
                     font.isAntiAlias(),
                     font.isTransparent() );
   if ( !face )
   {
      //dbError("Font impl not created(", font.toString(), ")")
      return nullptr;
   }

   // dbDebug("Added font (", font.toString(), "), uri(",uri,")")
   FontPair fontPair;
   fontPair.font = font;
   fontPair.impl = face;
   m_fonts.emplace_back( std::move( fontPair ) );
   return face;
}


std::string
FontManager::dumpStrFamilies() const
{
   std::ostringstream s;
   s << "FamilyCount = " << m_families.size() << "\n";
   for ( size_t i =0 ; i < m_families.size(); ++i )
   {
      s << "Family[" << i << "] " << m_families[ i ].toString() << "\n";
   }
   s << "FontCount = " << m_fonts.size() << "\n";
   for ( size_t i =0 ; i < m_fonts.size(); ++i )
   {
      s << "Font[" << i << "] " << m_fonts[ i ].font.toString() << "\n";
   }
   return s.str();
}

void
FontManager::saveFonts()
{
   //dbDebug("Save Fonts..." )
   //dbDebug(dumpStrFamilies())

/*
   // Find in Cache
   for ( size_t i = 0; i < m_fonts.size(); ++i )
   {
      std::shared_ptr< IFontAtlas > cached = m_fonts[ i ];
      if ( cached )
      {
         //dbDebug("CachedFont[",i,"] ", cached->getFont().toFontString() )
         cached->saveAtlas();
      }
   }
*/
}

} // end namespace irrExt.
