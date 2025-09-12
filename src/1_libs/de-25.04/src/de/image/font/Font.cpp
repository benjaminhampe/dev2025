#include <de/image/font/Font.h>

// #ifdef HAVE_FREETYPE2
//#include <DarkImage.h>

namespace de {

// =======================================================================
Font::Font()
// =======================================================================
   : id( 0 )
   , pixelSize( 42 )
   , baseLine( 0 )
   , bold( false )
   , italic( false )
   , antiAlias( true )
   , transparent( true )
   , lcdFit( false )
   , fontPtr( nullptr )
   , m_family( "arial" )
{}

Font::Font( std::string const & family, uint32_t pixelSize,
      bool bold, bool italic, bool aa, bool transparent, bool lcdAdjusted )
   : id( 0 )
   , pixelSize( pixelSize )
   , baseLine( 0 )
   , bold( bold )
   , italic( italic )
   , antiAlias( aa )
   , transparent( transparent )
   , lcdFit( lcdAdjusted )
   , fontPtr( nullptr )
   , m_family( "arial" )
{
   setFamily( family );
}

void
Font::setFamily( std::string family )
{
   if ( family.empty() )
   {
      family = "arial";
   }
   else
   {
      StringUtil::lowerCase(family);
   }
   m_family = family;
}

std::string const &
Font::family() const
{
   return m_family;
}

std::string
Font::toString() const
{
   std::stringstream s;
   s << m_family << "|" << pixelSize << "px";
   if ( bold ) s << "|Bold";
   if ( italic ) s << "|Italic";
   if ( antiAlias ) s << "|AA";
   if ( transparent ) s << "|Transparent";
   if ( lcdFit ) s << "|LCD";
   return s.str();
}

std::string
Font::toFontString() const
{
   // " << (void*)fontPtr << "
   std::stringstream s;
   s << "FONT_" << m_family << "_" << pixelSize;
   if ( bold ) s << "_B";
   if ( italic ) s << "_I";
   if ( antiAlias ) s << "_AA";
   if ( transparent ) s << "_T";
   if ( lcdFit ) s << "_LCD";
   return s.str();
}

std::string
Font::toGlyphString( uint32_t glyph_unicode ) const
{
   std::stringstream s;
   s << "G_" << glyph_unicode << "_" << toFontString();
   return s.str();
}

bool
Font::operator==( const Font & other ) const
{
    const std::string fam1 = StringUtil::makeLower(other.m_family);
    const std::string fam2 = StringUtil::makeLower(m_family);
    if ( fam1 != fam2 ) return false;
    if ( other.pixelSize != pixelSize ) return false;
    if ( other.bold != bold ) return false;
    if ( other.italic != italic ) return false;
    if ( other.antiAlias != antiAlias ) return false;
    if ( other.transparent != transparent ) return false;
    if ( other.lcdFit != lcdFit ) return false;
    return true;
}

bool
Font::operator!=( const Font & other ) const
{
   return !( *this == other );
}


// =======================================================================
TextSize::TextSize()
// =======================================================================
   : width( 0 )
   , height( 0 )
   , baseline( 0 )
   , lineHeight(0)
   , lineCount(0)
{}

TextSize::TextSize( int32_t w, int32_t h, int32_t baselineMax, int32_t _lineHeight, int32_t _lineCount )
   : width( w )
   , height( h )
   , baseline( baselineMax )
   , lineHeight( _lineHeight )
   , lineCount( _lineCount )
{}

std::string
TextSize::toString() const
{
   std::stringstream s;
   s << width << "," << height << "," << baseline << "," << lineHeight << "," << lineCount;
   return s.str();
}


// =======================================================================
Text::Text( int x, int y, std::wstring msg, Align align,
            uint32_t fillColor, uint32_t penColor )
// =======================================================================
   : x( x )
   , y( y )
   , align( align )
   , msg( msg )
   , pen( penColor)
   , brush( fillColor )
{}

// =======================================================================
Glyph::Glyph()
// =======================================================================
   : unicode( 0 ), advance( 0 ), ref(), bmp(), glyph_index( 0 )
{

}

Glyph::~Glyph()
{
   //dbSaveImage(debugImg,getGlyphString()+".tga");
}


std::string
Glyph::getGlyphString() const
{
   //char c = char( unicode & 0xFF );
   //wchar_t wc = wchar_t( unicode & 0xFFFF );
   std::ostringstream s;
   //s << dbStr( wc );
   s << "_U" << unicode;
   s << "_G" << glyph_index;
   s << "_A" << advance;
   s << "_B" << getBaselineOffset();
   s << "_R" << getAtlasRect().str();
   return s.str();
}

int32_t
Glyph::w() const { return bmp.w; }
int32_t
Glyph::h() const { return bmp.h; }
glm::ivec2
Glyph::size() const { return { bmp.w, bmp.h }; }

Image const *
Glyph::getAtlasImage() const { return ref.image(); }
Image *
Glyph::getAtlasImage() { return ref.image(); }
Recti const &
Glyph::getAtlasRect() const { return ref.rect(); }

int32_t
Glyph::x() const { return bmp.x; }
int32_t
Glyph::y() const { return bmp.y; }
int32_t
Glyph::x1() const { return ref.x1(); }
int32_t
Glyph::y1() const { return ref.y1(); }
int32_t
Glyph::x2() const { return ref.x2(); }
int32_t
Glyph::y2() const { return ref.y2(); }
Rectf
Glyph::texCoords() const { return ref.texCoords( false ); }

//   float Glyph::getU1() const { return part.getU1( false ); }
//   float Glyph::getV1() const { return part.getV1( false ); }
//   float Glyph::getU2() const { return part.getU2( false ); }
//   float Glyph::getV2() const { return part.getV2( false ); }

// offset < 0: above baseline
// offset = 0: on baseline ( standing from above baseline, touching it )
// offset > 0: below baseline ( a 'g' has positive baseline offset )
int32_t
Glyph::getBaselineOffset() const { return bmp.h - bmp.y; }

uint32_t
Glyph::getPixel( int32_t i, int32_t j ) const { return ref.pixel( i, j ); }

Image
Glyph::copyImage() const { return ref.copyImage(); }


std::string
Glyph::toString() const { return getGlyphString(); }

// ==============================================
PreparedGlyphText::PreparedGlyphText()
// ==============================================
   : isValid( false )
   , needUploadAtlas( false )
{

}

} // end namespace de.

// #endif
