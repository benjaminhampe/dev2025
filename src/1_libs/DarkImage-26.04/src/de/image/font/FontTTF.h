#pragma once
#include <de/image/font/Font.h>

#ifdef HAVE_FREETYPE2

#include <de_freetype2.hpp>

namespace de {

// ===========================================================================
// FontAtlasPage
// ===========================================================================
struct FontTTF : public IFontAtlas
{
   FontTTF( Font font );
   ~FontTTF() override;

   // Public Section:
// =======================
// Font interface
// =======================
   Font const & getFont() const override { return m_Font; }
   Font & getFont() override { return m_Font; }

// =======================
// Atlas interface
// =======================
   void setDirty( bool dirty ) override { m_isDirty = dirty; }
   bool isDirty() const override { return m_isDirty; }
   Image const* getAtlasImage() const override { return &m_AtlasPage0; }
   Image* getAtlasImage() override { return &m_AtlasPage0; }
   void saveAtlas() override;

// =======================
// Glyph interface
// =======================
   bool cacheString( std::wstring const & msg ) override;
   glm::ivec2 getKerning( uint32_t prev_unicode, uint32_t curr_unicode ) const override;
   Glyph createGlyph( uint32_t unicode ) override;
   bool hasGlyph( uint32_t unicode ) const override;
   Glyph & getGlyph( uint32_t unicode ) override;
   TextSize getGlyphSize( uint32_t unicode ) override;

// =======================
// Text interface
// =======================
   // x = lineWidth
   // y = lineHeight
   // z = baselineOffsetY
   TextSize getTextSize( std::wstring const & txt ) override;
   // [Text] Draw
   void drawText( Image & img, int x, int y, std::wstring const & txt,
      uint32_t color = 0xFF000000, Align align = Align::Default ) override;
   // [Text] to Image
   Image createTextImage( std::wstring const & txt, uint32_t txtColor = 0xFF000000,
      uint32_t fillColor = 0xFFFFFFFF ) override;

   // Only for impl Section:
   virtual std::string const & getAtlasImageName() const { return m_AtlasPage0.uri(); }
   virtual std::string const & getUri() const { return m_Uri; }
   virtual void setUri( std::string const & uri ) { m_Uri = uri; }
   virtual void setLib( FT_Library lib ) { m_ftLib = lib; }
   virtual void open( std::string uri );
   virtual void openMemory( const uint8_t* pBytes, size_t nBytes );
   virtual void close();
   virtual bool is_open() const;
   virtual bool setPixelSize( int pixelSize );

protected:
   // [Glyph] Draw.
   void drawGlyph( Image & img, int x, int y,
                   uint32_t unicode, uint32_t color = 0xFF000000 ); // override;
   // [Glyph] Image.
   Image createGlyphImage( uint32_t unicode, uint32_t txtColor = 0xFF000000,
                           uint32_t fillColor = 0xFFFFFFFF ); // override;
public:
   Font m_Font;
   FT_Library m_ftLib;
   FT_Face m_ftFace;
   std::string m_Uri;
   bool m_IsOpen;
   bool m_isDirty;
   int m_AtlasX;
   int m_AtlasY;
   int m_AtlasCLH; // current line height
   int m_AtlasMLH; // current line height
   Image m_AtlasPage0;

   std::unordered_map< uint32_t, Glyph > m_glyphCache;
};


// ===========================================================================
// FontTTF_Utils
// ===========================================================================
struct FontTTF_Utils
{
   DE_CREATE_LOGGER("de.FontTTF.Utils")

   static void
   drawFtBitmap( Image & dst, int x, int y, FT_Bitmap const & bmp );

/*
   static void
   createLibInstance( FT_Library & lib );

   static void
   destroyLibInstance( FT_Library & lib );

   static bool
   isFace( FT_Face face );

   static void
   closeFace( FT_Face & face );

   static FT_Face
   openFace( FT_Library lib, std::string const & uri );
*/

   static bool
   hasOutline( FT_Face face );

   static bool
   flipOutline( FT_Face face );

   static Recti
   computeOutlineBBox( FT_Face face );

   static bool
   setPixelSize( FT_Face face, int pixelSize );

   static uint32_t
   getGlyphIndex( FT_Face face, uint32_t unicode );

   static glm::ivec2
   getKerning( FT_Face face, uint32_t prevLetter, uint32_t thisLetter );
};



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


} // end namespace de.

#endif
