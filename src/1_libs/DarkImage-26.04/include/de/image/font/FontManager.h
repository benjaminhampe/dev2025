#pragma once
#include <de/image/font/Font.h>

// #ifdef HAVE_FREETYPE2

namespace de {

// ===========================================================================
class FontManager
// ===========================================================================
{
public:
   FontManager();
   ~FontManager();

   static std::shared_ptr< FontManager >
   getInstance();

   bool
   addFamily( const std::string& uri, const Font& font );

   bool
   addFamily( const Font& font, const uint8_t* pBytes, const uint64_t nBytes );

   std::shared_ptr< IFontAtlas >
   getFont( const Font& font );

   int32_t
   findFont( const Font& font ) const;

   int32_t
   findFamily( const std::string& family ) const;

   FontFamily
   getFamily( const std::string& family ) const;


   void
   saveFonts();

   std::string
   dumpStrFamilies() const;

private:
   std::vector< FontFamily > m_Families;
   std::vector< std::shared_ptr< IFontAtlas > > m_Fonts;
};

} // end namespace de.


// #endif
