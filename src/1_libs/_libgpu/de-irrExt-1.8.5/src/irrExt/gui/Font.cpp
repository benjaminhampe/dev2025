#include <irrExt/gui/Font.h>

namespace irrExt {

// =======================================================================
Font::Font()
// =======================================================================
   : m_id( 0 )
   , m_pixelSize( 42 )
   , m_baseLine( 0 )
   , m_isBold( false )
   , m_isItalic( false )
   , m_isAntiAlias( true )
   , m_isTransparent( true )
   , m_isLCDFit( false )
   , m_fontPtr( nullptr )
   , m_family( "arial" )
{}

Font::Font( std::string const & family, uint32_t pixelSize,
      bool bold, bool italic, bool aa, bool transparent, bool lcdAdjusted )
   : m_id( 0 )
   , m_pixelSize( pixelSize )
   , m_baseLine( 0 )
   , m_isBold( bold )
   , m_isItalic( italic )
   , m_isAntiAlias( aa )
   , m_isTransparent( transparent )
   , m_isLCDFit( lcdAdjusted )
   , m_fontPtr( nullptr )
   , m_family( "arial" )
{
   setFamily( family );
}

void
Font::setFamily( std::string const & family )
{
    std::string tmp = family;

   if ( family.empty() )
   {
      tmp = "arial";
   }
   else
   {
      tmp = lowerCase( family );
   }
   m_family = tmp;
}

std::string
Font::toString() const
{
   std::stringstream s;
   s << m_family << "|" << m_pixelSize << "px";
   if ( m_isBold ) s << "|Bold";
   if ( m_isItalic ) s << "|Italic";
   if ( m_isAntiAlias ) s << "|AA";
   if ( m_isTransparent ) s << "|Transparent";
   if ( m_isLCDFit ) s << "|LCD";
   return s.str();
}

std::string
Font::toFontString() const
{
   // " << (void*)fontPtr << "
   std::ostringstream s;
   s << "FONT_" << m_family << "_" << m_pixelSize;
   if ( m_isBold ) s << "_B";
   if ( m_isItalic ) s << "_I";
   if ( m_isAntiAlias ) s << "_AA";
   if ( m_isTransparent ) s << "_T";
   if ( m_isLCDFit ) s << "_LCD";
   return s.str();
}

std::string
Font::toGlyphString( uint32_t glyph_unicode ) const
{
   std::ostringstream s;
   s << "G_" << glyph_unicode << "_" << toFontString();
   return s.str();
}

bool
Font::operator==( Font const & other ) const
{
   if ( other.m_family != m_family ) return false;
   if ( other.m_pixelSize != m_pixelSize ) return false;
   if ( other.m_isBold != m_isBold ) return false;
   if ( other.m_isItalic != m_isItalic ) return false;
   if ( other.m_isAntiAlias != m_isAntiAlias ) return false;
   if ( other.m_isTransparent != m_isTransparent ) return false;
   if ( other.m_isLCDFit != m_isLCDFit ) return false;
   return true;
}

bool
Font::operator!=( Font const & other ) const
{
   return !( *this == other );
}

} // end namespace irrExt.
