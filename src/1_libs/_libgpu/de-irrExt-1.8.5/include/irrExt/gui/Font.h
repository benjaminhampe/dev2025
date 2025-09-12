#pragma once
#include <cstdint>
#include <sstream>
//#include <irrExt/core/StringUtil.h>
// #include <vector>
// #include <memory> // SharedPtr
// #include <map>    // GlyphCache
// #include <de_glm.hpp>
// //#include <de_fontawesome.hpp>
// #include <tinyxml2/tinyxml2.h>
// //#include <de_core/de_RuntimeError.h>
// #include <de_freetype2.hpp>

namespace irrExt {

// =======================================================================
struct Font // ===   TrueTypeFonts using lib freetype2
// =======================================================================
{
    static
    std::string
    lowerCase( std::string const & src, std::locale const & loc = std::locale() )
    {
        std::string tmp = src;
       for ( size_t i = 0; i < tmp.size(); ++i )
       {
          #ifdef _MSC_VER
          tmp[ i ] = static_cast< char >( ::tolower( out[ i ] ) );
          #else
          tmp[ i ] = static_cast< char >( std::tolower< char >( tmp[ i ], loc ) );
          #endif
       }
       return tmp;
    }

   Font();
   Font( std::string const & family, uint32_t pixelSize,
         bool bold = false, bool italic = false,
         bool aa = true, bool transparent = true, bool lcdAdjusted = false );

   std::string const & family() const { return m_family; }
   uint32_t pixelSize() const { return m_pixelSize; }
   bool isBold() const { return m_isBold; }
   bool isItalic() const { return m_isItalic; }
   bool isAntiAlias() const { return m_isAntiAlias; }
   bool isTransparent() const { return m_isTransparent; }
   bool isLCDFit() const { return m_isLCDFit; }

   std::string
   toString() const;

   std::string
   toFontString() const;

   std::string
   toGlyphString( uint32_t glyph_unicode ) const;

   bool
   operator==( Font const & other ) const;

   bool
   operator!=( Font const & other ) const;


   void
   setFamily( std::string const & family );


   uint32_t m_id;
   uint32_t m_pixelSize;
   int32_t m_baseLine;
   bool m_isBold;
   bool m_isItalic;
   bool m_isAntiAlias;
   bool m_isTransparent;
   bool m_isLCDFit;
   void* m_fontPtr;
protected:
   std::string m_family;

};

// ===========================================================================
struct FontFamily
// ===========================================================================
{
   std::string family;
   std::string uri;

   std::string
   toString() const
   {
      std::ostringstream s;
      s << family << "|" << uri;
      return s.str();
   }

};

} // end namespace irrExt.


/*
struct FaceUtil
{
   static Glyph
   createGlyph( uint32_t unicode )
   {
      Glyph glyph;
      glyph.unicode = unicode;
      //glyph.ref.m_img = &m_AtlasPage0;

      uint32_t index = FontTTF_Utils::getGlyphIndex( m_ftFace, unicode );
      if ( index == 0 )
      {
         // its not a bug for white spaces and unprintable characters like margins, paddings
         // it would spam the logs printing errors for white spaces.
         // DE_ERROR("Invalid index. unicode(",unicode,")")
         return glyph;
      }

      // This is because we cache faces and the face may have been set to a different size.
      FontTTF_Utils::setPixelSize( m_ftFace, m_Font.pixelSize );

      uint32_t loadFlags = FT_LOAD_RENDER;
      loadFlags |= FT_LOAD_COMPUTE_METRICS;
      loadFlags |= FT_LOAD_FORCE_AUTOHINT;

      if ( m_Font.lcdFit )
      {
         loadFlags |= FT_LOAD_TARGET_LCD;
      }

      if ( !m_Font.antiAlias )
      {
         loadFlags |= FT_LOAD_MONOCHROME;
      }

      // FT_LOAD_NO_SCALE | FT_LOAD_NO_BITMAP

      FT_Error e = FT_Load_Glyph( m_ftFace, index, loadFlags );
      if ( e ) { DE_ERROR("Failed FT_Load_Glyph(",unicode,")") return glyph; }

      FT_GlyphSlot ftGlyph = m_ftFace->glyph;
      if ( !ftGlyph ) { DE_ERROR("No glyph to access unicode(",unicode,")") return glyph; }

      //FT_Glyph_Metrics const & ftGlyphMetrics = ftGlyph->metrics;

      glyph.glyph_index = index;
      glyph.advance = ftGlyph->advance.x;

      if ( m_Font.lcdFit )
      {
         glyph.advance = 3 * ftGlyph->advance.x;
      }

      //glyph.advance_y = ftGlyph->advance.y;
      if ( FT_IS_SCALABLE( m_ftFace ) )
      {
         glyph.advance /= 64;
         //glyph.advance_y /= 64;
   //    glyph.hBearing /= 64;
   //    glyph.vBearing /= 64;
   //    glyph.bbox /= 64;
      }

   //    glyph.hBearing.x = ftGlyphMetrics.horiBearingX;
   //    glyph.hBearing.y = ftGlyphMetrics.horiBearingY;
   //    glyph.vBearing.x = ftGlyphMetrics.vertBearingX;
   //    glyph.vBearing.y = ftGlyphMetrics.vertBearingY;
   //    glyph.bbox = computeOutlineBBox();

      int32_t bmp_x = ftGlyph->bitmap_left;
      int32_t bmp_y = ftGlyph->bitmap_top;
      int32_t bmp_w = ftGlyph->bitmap.width;
      int32_t bmp_h = ftGlyph->bitmap.rows;
      // Now raster glyph to image and convert image to the dark side.
      glyph.bmp = Recti( bmp_x, bmp_y, bmp_w, bmp_h );

      // int w_total = m_AtlasPage0.getWidth();
      // int h_total = m_AtlasPage0.getHeight();

      if ( m_AtlasX + glyph.bmp.getWidth() + 4 >= m_AtlasPage0.getWidth() )
      {
         m_AtlasY += m_AtlasCLH;
         m_AtlasX = 1;
         m_AtlasCLH = 0;
      }

      // drawBorder
      //Recti r_border( m_AtlasX - 1, m_AtlasY - 1, bmp_w + 2, bmp_h + 2 );
      //ImagePainter::drawOutlineRect( m_AtlasPage0, r_border, 0xFF0000FF );

      // drawGlyph
      glyph.ref.m_rect = Recti( m_AtlasX, m_AtlasY, bmp_w, bmp_h );
      FontTTF_Utils::drawFtBitmap( m_AtlasPage0, m_AtlasX, m_AtlasY, ftGlyph->bitmap );

      // advance atlas cursor
      m_AtlasX += bmp_w + 2;
      m_AtlasCLH = std::max( m_AtlasCLH, bmp_h + 2 );

      // debugImage
   //   auto dbg = glyph.ref.m_rect;
   //   glyph.debugImg = Image( dbg.w()+2, dbg.h()+2 );
   //   glyph.debugImg.fill(0);
   //   FontTTF_Utils::drawFtBitmap( glyph.debugImg, 0, 0, ftGlyph->bitmap );

      // DE_DEBUG( "Glyph[", char( glyph.unicode & 0xFF ), "] ", glyph.toString() )
      return glyph;
   }
};
*/
