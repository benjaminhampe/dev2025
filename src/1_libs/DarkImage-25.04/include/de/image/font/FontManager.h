#pragma once
#include <de/image/font/Font.h>

// #ifdef HAVE_FREETYPE2

void dbPrepareFont(de::Font font, std::string uri );
void dbPrepareFont(de::Font font, const uint8_t* pBytes, uint64_t nBytes );

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
   addFamily( std::string uri, Font font );

   bool
   addFamily( Font font, const uint8_t* pBytes, uint64_t nBytes );

   std::shared_ptr< IFontAtlas >
   getFont( Font font );

   int32_t
   findFont( Font const & font ) const;

   int32_t
   findFamily( std::string family ) const;

   FontFamily
   getFamily( std::string family ) const;


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
