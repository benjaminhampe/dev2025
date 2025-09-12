#pragma once
#include <irrExt/gui/Font.h>
#include <irrlicht.h>
#include <memory> // SharedPtr
#include <vector>
// #include <memory> // SharedPtr
// #include <map>    // GlyphCache
// #include <de_glm.hpp>
// //#include <de_fontawesome.hpp>
// #include <tinyxml2/tinyxml2.h>
// //#include <de_core/de_RuntimeError.h>
// #include <de_freetype2.hpp>

namespace irrExt {

// ===========================================================================
struct FontPair
// ===========================================================================
{
   Font font;
   irr::gui::IGUIFont* impl = nullptr;
};

// ===========================================================================
class FontManager
// ===========================================================================
{
public:
   static std::shared_ptr< FontManager >   getInstance();

   FontManager();
   ~FontManager();

   void   setDevice( irr::IrrlichtDevice* device );

   bool   addFamily( std::string const & uri, Font const & font );

   irr::gui::IGUIFont*   getFont( Font const & font );

   int32_t   findFont( Font const & font ) const;

   int32_t   findFamily( std::string const & family ) const;

   std::string   dumpStrFamilies() const;

   void saveFonts();

private:
   //DE_CREATE_LOGGER("de.FontManager")
   irr::IrrlichtDevice* m_device;
   std::vector< FontFamily > m_families;
   //std::vector< irr::gui::IGUIFont* > m_fonts;
   std::vector< FontPair > m_fonts;
   //std::unordered_map< Font, irr::gui::IGUIFont* > m_fontLut;
};

/*
// =======================
// FontAtlasManager interface
// =======================
std::shared_ptr< IFontAtlas >
getFontFace( Font font );

//DarkGDK::addFontFamily( mediaDir + "fonts/garton.ttf", DarkGDK::Font("garton", 32 ) );
//DarkGDK::addFontFamily( mediaDir + "fonts/carib.otf", DarkGDK::Font("carib", 32 ) );
//DarkGDK::addFontFamily( mediaDir + "fonts/arial.ttf", DarkGDK::Font("Arial", 32 ) );
//DarkGDK::addFontFamily( mediaDir + "fonts/awesome.ttf", DarkGDK::Font("awesome", 32 ) );
bool
addFontFamily( std::string uri, Font font );

void
saveFonts();
*/

inline bool
addFontFamily( std::string const & uri, Font const & font )
{
   return FontManager::getInstance()->addFamily( uri, font );
}

inline irr::gui::IGUIFont*
getFontFace( Font font )
{
   return FontManager::getInstance()->getFont( font );
}


} // end namespace irrExt.


