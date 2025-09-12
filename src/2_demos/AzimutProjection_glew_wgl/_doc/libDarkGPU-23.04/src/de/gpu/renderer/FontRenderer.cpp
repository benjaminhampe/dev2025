#include <de/gpu/renderer/FontRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

// ===========================================================================
FontRenderer::FontRenderer( VideoDriver* driver )
// ===========================================================================
   : m_Driver( driver )
   , m_Shader( nullptr )
{
//   addFontFamily( "../../media/fonts/arial.ttf", Font("arial", 32 ) );
//   addFontFamily( "../../media/fonts/fontawesome463.ttf", Font("awesome", 32 ) );
//   addFontFamily( "../../media/fonts/la-brands-400.ttf", Font("brands", 64 ) );
//   addFontFamily( "../../media/fonts/la-regular-400.ttf", Font("regular", 16 ) );
//   //addFontFamily( "../../media/fonts/la-brands-400.ttf", Font("brands", 64 ) );
}

FontRenderer::~FontRenderer()
{
   clear();
}

void
FontRenderer::clear()
{
}

TextSize
FontRenderer::getTextSize( std::wstring const & msg, Font const & font )
{
   return dbGetTextSize( msg, font );
}

void
FontRenderer::draw2DText( int x, int y, std::wstring const & msg, uint32_t color,
                            Align align, Font const & font )
{
   if (msg.empty()) return;
   if (!m_Driver) return;
   int w = m_Driver->getScreenWidth();
   int h = m_Driver->getScreenHeight();
   if ( w < 2 || h < 2 ) return;

   auto face = getFontFace( font );
   if ( !face )
   {
      DE_RUNTIME_ERROR(font.toString(),"No font face")
      return;
   }

   //Image dbg = face->createTextImage( msg, 0xFFFFFFFF, 0x00000000 );
   //dbSaveImage( dbg, dbStr(msg) + ".png" );

   face->cacheString( msg );

   TextSize ts = face->getTextSize( msg );

   de::Image const * atlasImage = face->getAtlasImage();
   if ( !atlasImage ) { DE_ERROR("No atlas image") return; }

   std::string uri = atlasImage->getUri();
   //DE_DEBUG("Font: ", font.toString(), " has atlasURI(", uri, "), msg(",dbStr(msg),")")

   SO const so(
         SO::MagLinear,
         SO::MinLinear,
         SO::ClampToEdge,
         SO::ClampToEdge );

   if ( !m_Driver->hasTexture( uri ) )
   {
      m_Driver->createTexture( uri, *atlasImage, so );
      face->setDirty( false );
      DE_DEBUG("Created font texture ", uri)
   }
   else
   {
      if ( face->isDirty() )
      {
         TexRef ref = m_Driver->getTexture( uri );
         m_Driver->uploadTexture( ref.m_tex, *atlasImage );
         face->setDirty( false );
         DE_DEBUG("Uploaded font texture ", uri)
      }
   }

   TexRef ref = m_Driver->getTexture( uri );
   if ( !ref.m_tex )
   {
      DE_ERROR("No atlas tex(", uri, ")")
      return;
   }

   int tex_w = ref.m_tex->getWidth();
   int tex_h = ref.m_tex->getHeight();

   // Align the text_size(w,h) to the text_pos(x,y) only.
   int32_t tw = ts.width;
   int32_t th = ts.height;
//   if ( font.lcdFit )
//   {
//      tw /= 3;
//   }

   // horizontal align to entire screen
   glm::ivec2 aligned_pos;
        if ( align & Align::Left )  { aligned_pos.x = x;       }
   else if ( align & Align::Center ){ aligned_pos.x = x - tw/2; }
   else if ( align & Align::Right ) { aligned_pos.x = x - tw;   }
   else { DE_ERROR("Invalid horizontal text align") return; }
   // vertical align to entire screen
        if ( align & Align::Top )   { aligned_pos.y = y;       }
   else if ( align & Align::Middle ){ aligned_pos.y = y - th/2; }
   else if ( align & Align::Bottom ){ aligned_pos.y = y - th;   }
   else { DE_ERROR("Invalid vertical text align") return; }

   int px = aligned_pos.x;
   int py = aligned_pos.y;

   SMeshBuffer quads( PrimitiveType::Triangles );

   // Loop over glyphs we got from cacheString() and
   // render them now...
   uint32_t last_unicode = 0;

   for ( size_t i = 0; i < msg.size(); ++i )
   {
      uint32_t unicode = msg[ i ];
      if ( unicode == '\n' || unicode == '\r' )
      {
         px = aligned_pos.x;
         py += ts.lineHeight;
         continue;
      }
      // Kerning
      glm::ivec2 kerning(0,0);
      if ( i > 0 )
      {
         kerning = face->getKerning( last_unicode, unicode );
      }

      last_unicode = unicode;

      // Draw
      Glyph & glyph = face->getGlyph( i );

      if ( glyph.ref.w() > 0 && glyph.ref.h() > 0 )
      {
         int x1 = glyph.bmp.x() + px + kerning.x; // + glyph.bmp.getX();
         int y1 = py + ts.lineHeight - ts.baseline - glyph.bmp.y(); // + ts.baseline; // + glyph.ref.getY();

         float gw = float( glyph.ref.w() );
         float gh = float( glyph.ref.h() );
         if ( font.lcdFit ) gw /= 2;
         int x2 = x1 + gw-1;
         int y2 = y1 + gh-1;

         auto const & r = glyph.ref.rect();
         float u1 = float( r.x1() ) / float( tex_w );
         float v1 = float( r.y1() ) / float( tex_h );
         float u2 = u1 + ( float( r.w() ) / float( tex_w ) );
         float v2 = v1 + ( float( r.h() ) / float( tex_h ) );

         S3DVertex A( x1, y2, -1.f, 0,0,-1, color, u1, v2 ); // color is set as uniform now
         S3DVertex B( x1, y1, -1.f, 0,0,-1, color, u1, v1 ); // not as vertex attrib anymore
         S3DVertex C( x2, y1, -1.f, 0,0,-1, color, u2, v1 );
         S3DVertex D( x2, y2, -1.f, 0,0,-1, color, u2, v2 );

         quads.addVertex( A );
         quads.addVertex( B );
         quads.addVertex( C );
         quads.addVertex( A );
         quads.addVertex( C );
         quads.addVertex( D );

         //DE_DEBUG( "Glyph[",i,"] bmp_rect(", glyph.bmp.toString(), ")" )
         //DE_DEBUG( "Glyph[",i,"] atlas_rect(", glyph.atlas.rect.toString(), ")" )
         //DE_DEBUG("Draw Glyph[",i,"] u1(",u1,"), v1(",v1,"), u2(",u2,"), v2(",v2,")" )
      }

      if ( font.lcdFit )
      {
         px += glyph.advance / 2;
      }
      else
      {
         px += glyph.advance;
      }

      //if ( font.lcdFit )
      //{
         //px += glyph.advance / 3;
      //}

   }

   //m_Driver->setDebug( false );
#if 0
   SMeshBuffer lines = SMeshBufferTool::createWireframe( quads, 1.0f, 0xFFFFFFFF );
   m_Driver->draw2D( lines );
#endif

#if 0
   SMeshBuffer colorQuads = quads;
   SMeshBufferTool::colorVertices( colorQuads, 0x19888888);
   //colorQuads.setDepth( Depth::disabled() );
   colorQuads.setBlend( Blend::alphaBlend() );
   colorQuads.setTexture( 0, nullptr );
   m_Driver->draw2D( colorQuads );
#endif

   //quads.setDepth( Depth::disabled() );
   quads.setBlend( Blend::alphaBlend() );
   quads.setTexture( 0, ref );
   quads.getMaterial().CloudTransparent = true;
   m_Driver->draw2D( quads );


   //draw2DTextDebug( x,y,msg,color,align,font);
}

void
FontRenderer::draw2DTextDebug( int x, int y, std::wstring const & msg, uint32_t color,
                            Align align, Font const & font )
{
   if (msg.empty()) return;
   if (!m_Driver) return;
   int w = m_Driver->getScreenWidth();
   int h = m_Driver->getScreenHeight();
   if ( w < 2 || h < 2 ) return;

   //DE_DEBUG("FONT ", w," x ",h)

   //m_Driver->setDebug( true );

   auto face = getFontFace( font );
   if ( !face )
   {
      DE_RUNTIME_ERROR(font.toString(),"No font impl")
      //DE_ERROR("No font impl") return;
   }

   face->cacheString( msg );

   de::Image const * atlasImage = face->getAtlasImage();
   if ( !atlasImage ) { DE_ERROR("No atlas image") return; }

   // Align the text_size(w,h) to the text_pos(x,y) only.
   TextSize ts = face->getTextSize( msg );
   int32_t tw = ts.width;
   int32_t th = ts.height;
   if ( font.lcdFit )
   {
      tw /= 3;
   }

   // horizontal align to entire screen
   glm::ivec2 aligned_pos;
        if ( align & Align::Left )  { aligned_pos.x = x;       }
   else if ( align & Align::Center ){ aligned_pos.x = x - tw/2; }
   else if ( align & Align::Right ) { aligned_pos.x = x - tw;   }
   else { DE_ERROR("Invalid horizontal text align") return; }
   // vertical align to entire screen
        if ( align & Align::Top )   { aligned_pos.y = y;       }
   else if ( align & Align::Middle ){ aligned_pos.y = y - th/2; }
   else if ( align & Align::Bottom ){ aligned_pos.y = y - th;   }
   else { DE_ERROR("Invalid vertical text align") return; }

   float px = aligned_pos.x;
   float py = aligned_pos.y + ts.baseline;
   float pz = -1.0f; // At -1.0 it disappears, not inside frustum.

   SMeshBuffer lines( PrimitiveType::Lines );

   // Loop over glyphs we got from cacheString() and
   // render them now...
   uint32_t last_unicode = 0;

   for ( size_t i = 0; i < msg.size(); ++i )
   {
      uint32_t unicode = msg[ i ];
      if ( unicode == '\n' || unicode == '\r' )
      {
         px = aligned_pos.x;
         py += ts.lineHeight;
         continue;
      }

      // Kerning
      glm::ivec2 kerning(0,0);
      if ( i > 0 )
      {
         kerning = face->getKerning( last_unicode, unicode );
      }

      last_unicode = unicode;

      // Draw
      Glyph const & glyph = face->getGlyph( unicode );
      if ( glyph.ref.getWidth() > 0 && glyph.ref.getHeight() > 0 )
      {
         float x1 = px + kerning.x; // + glyph.bmp.getX();
         float y1 = py - glyph.ref.getY();

         float gw = float( glyph.ref.getWidth() );
         float gh = float( glyph.ref.getHeight() );
         if ( font.lcdFit ) gh /= 3;
         float x2 = x1 + gw;
         float y2 = y1 + gh;
         float z1 = pz;

         Rectf const & texCoords = glyph.ref.getTexCoords();
         float u1 = texCoords.u1();
         float v1 = texCoords.v1();
         float u2 = texCoords.u2();
         float v2 = texCoords.v2();
         S3DVertex A( x1, y2, z1, 0,0,0, 0x4F0000FF, 0, 1 );
         S3DVertex B( x1, y1, z1, 0,0,0, 0x4F00FF00, 0, 0 );
         S3DVertex C( x2, y1, z1, 0,0,0, 0x4FFF0000, 1, 0 );
         S3DVertex D( x2, y2, z1, 0,0,0, 0x4F00FFFF, 1, 1 );

//         S3DVertex A( x1, y2, z1, 0,0,0, 0x4F0000FF, 0, 1 );
//         S3DVertex B( x1, y1, z1, 0,0,0, 0x4F00FF00, 0, 0 );
//         S3DVertex C( x2, y1, z1, 0,0,0, 0x4FFF0000, 1, 0 );
//         S3DVertex D( x2, y2, z1, 0,0,0, 0x4F00FFFF, 1, 1 );

         lines.addVertex( A ); lines.addVertex( B );
         lines.addVertex( B ); lines.addVertex( C );
         lines.addVertex( C ); lines.addVertex( D );
         lines.addVertex( D ); lines.addVertex( A );
      }

      if ( font.lcdFit )
      {
         px += glyph.advance / 3;
      }
      else
      {
         px += glyph.advance;
      }
   }

   lines.getMaterial().setDepth( Depth::disabled() );
   lines.getMaterial().setCulling( false );
   lines.getMaterial().setBlend( Blend::alphaBlend() );
   m_Driver->draw2D( lines );

}


} // end namespace gpu.
} // end namespace de.


#if 0

void
FontRenderer::test()
{
   int w = 800;
   int h = 600;
   if ( m_Driver )
   {
      w = m_Driver->getScreenWidth();
      h = m_Driver->getScreenHeight();
   }

   Text text1;
   text1.x = 10;
   text1.y = 10;
   text1.align = TextAlign::TopLeft;
   text1.font = Font( "../Arial.ttf", 52, false, false );
   text1.brush = Brush( RGBA( 255,255,255,255 ), nullptr );
   text1.pen = Pen( RGBA( 255,0,0,255 ), 2.5f );
   text1.msg = L"Hello World";
   addText( text1 );

   Text text2;
   text2.x = w / 2;
   text2.y = 10;
   text2.align = TextAlign::TopCenter;
   text2.font = Font( "../Arial.ttf", 52, false, false );
   text2.brush = Brush( RGBA( 255,255,255,255 ), nullptr );
   text2.pen = Pen( RGBA( 255,0,0,255 ), 2.5f );
   text2.msg = L"Hello TopCenter World";
   addText( text2 );
}

#endif
