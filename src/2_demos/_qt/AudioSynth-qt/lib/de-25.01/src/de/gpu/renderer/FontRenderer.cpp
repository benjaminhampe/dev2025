#include <de/gpu/renderer/FontRenderer.h>
#include <de/gpu/VideoDriver.h>
#include <de/gpu/renderer/ScreenRenderer.h>

namespace de {
namespace gpu {

// ===========================================================================
FontRenderer::FontRenderer()
// ===========================================================================
    : m_driver( nullptr )
    , m_screenRenderer( nullptr )
{
//   addFontFamily( "../../media/fonts/arial.ttf", Font("arial", 32 ) );
//   addFontFamily( "../../media/fonts/fontawesome463.ttf", Font("awesome", 32 ) );
//   addFontFamily( "../../media/fonts/la-brands-400.ttf", Font("brands", 64 ) );
//   addFontFamily( "../../media/fonts/la-regular-400.ttf", Font("regular", 16 ) );
//   addFontFamily( "../../media/fonts/la-brands-400.ttf", Font("brands", 64 ) );
}

void
FontRenderer::init( VideoDriver* driver )
{
    m_driver = driver;
    m_screenRenderer = m_driver->getScreenRenderer();
}

TextSize
FontRenderer::getTextSize( std::wstring const & msg, Font const & font )
{
    auto fontImpl = getFontFace( font );
    if (!fontImpl) return TextSize();
    return fontImpl->getTextSize( msg );
}

void
FontRenderer::draw2DText(int x, int y, std::string const & msg,
                        uint32_t color, Align align, Font const & font,
                        uint32_t bgColor, int padding )
{
    draw2DText(x,y,StringUtil::to_wstr(msg),color, align, font, bgColor, padding);
}

void
FontRenderer::draw2DText(int x, int y, std::wstring const & msg,
                        uint32_t color, Align align, Font const & font,
                        uint32_t bgColor, int padding )
{
    if (!m_driver) { DE_WARN("No driver") return; }
    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    if ( w < 2 || h < 2 ) { return; }
    if (msg.empty()) { return; }

    const auto face = getFontFace( font );
    if (!face)
    {
        DE_ERROR("No face for font ", font.toString())
        return;
    }

    face->cacheString( msg );

    const TextSize ts = face->getTextSize( msg );
    //DE_WARN("TextSize(",ts.toString(),"), ",StringUtil::to_str(msg))

    const Image* atlasImage = face->getAtlasImage();
    if (!atlasImage) { DE_ERROR("No atlas image") return; }

    const std::string uri = atlasImage->uri();

    //DE_DEBUG("Font: ", font.toString(), " has atlasURI(", uri, "), msg(",dbStr(msg),")")
    // if ( face->isDirty() )
    // {
    //     dbSaveImage(*atlasImage, uri);
    // }

    SamplerOptions const so( 1.0f,
        SamplerOptions::Minify::Linear,
        SamplerOptions::Magnify::Linear,
        SamplerOptions::Wrap::ClampToEdge,
        SamplerOptions::Wrap::ClampToEdge );

    auto tex = m_driver->getTexture( uri );
    if (!tex)
    {
        tex = m_driver->createTexture2D( uri, *atlasImage, so );
        if (!tex)
        {
            DE_ERROR("No atlas tex(", uri, ") created")
            return;
        }
        face->setDirty( false );
        //DE_DEBUG("Created font texture ", uri)
    }
    else
    {
        if ( face->isDirty() )
        {
            m_driver->uploadTexture2D( tex, *atlasImage, so );
            face->setDirty( false );
            //DE_DEBUG("Uploaded font texture ", uri)
        }
    }

    const int tex_w = tex->w();
    const int tex_h = tex->h();

    // Align the text_size(w,h) to the text_pos(x,y) only.
    const int32_t tw = ts.width;
    const int32_t th = ts.height;
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

    int32_t bbox_x1 = std::numeric_limits<int32_t>::max();
    int32_t bbox_y1 = std::numeric_limits<int32_t>::max();
    int32_t bbox_x2 = std::numeric_limits<int32_t>::lowest();
    int32_t bbox_y2 = std::numeric_limits<int32_t>::lowest();

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
        const Glyph & glyph = face->getGlyph( unicode );

        if ( glyph.ref.w() > 0 && glyph.ref.h() > 0 )
        {
            int x1 = px + glyph.bmp.x + kerning.x;
            int y1 = py + ts.baseline - glyph.bmp.y; // ts.lineHeight + ts.baseline; // + glyph.ref.getY();

            int gw = glyph.ref.w();
            int gh = glyph.ref.h();
            if ( font.lcdFit ) gw /= 2;
            int x2 = x1 + gw-1;
            int y2 = y1 + gh-1;

            bbox_x1 = std::min( bbox_x1, x1 );
            bbox_y1 = std::min( bbox_y1, y1 );
            bbox_x2 = std::max( bbox_x2, x2 );
            bbox_y2 = std::max( bbox_y2, y2 );

            auto const & r = glyph.ref.rect();
            float u1 = float( r.x ) / float( tex_w );
            float v1 = float( r.y ) / float( tex_h );
            float u2 = u1 + ( float( r.w ) / float( tex_w ) );
            float v2 = v1 + ( float( r.h ) / float( tex_h ) );

            S3DVertex A( x1, y2, 0, 0,0,1, color, u1, v2 ); // color is set as uniform now
            S3DVertex B( x1, y1, 0, 0,0,1, color, u1, v1 ); // not as vertex attrib anymore
            S3DVertex C( x2, y1, 0, 0,0,1, color, u2, v1 );
            S3DVertex D( x2, y2, 0, 0,0,1, color, u2, v2 );

            quads.addVertex( A );
            quads.addVertex( C );
            quads.addVertex( B );
            quads.addVertex( A );
            quads.addVertex( D );
            quads.addVertex( C );

            //DE_DEBUG( "Glyph[",i,"] x1(",x1,"), y1(",y1,"), x2(",x2,"), y2(",y2,")")
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

    quads.upload();

    quads.setDepth( Depth::disabled() );
    quads.setBlend( Blend::alphaBlend() );
    quads.setCulling( Culling::disabled() );
    quads.setTexture( 0, tex );
    //quads.getMaterial().CloudTransparent = true;

    // Draw background rect
    if (dbRGBA_A(bgColor) > 0)
    {
        bbox_x1 -= padding;
        bbox_y1 -= padding;
        bbox_x2 += padding;
        bbox_y2 += padding;
        auto bbox_r = Recti(bbox_x1, bbox_y1, bbox_x2 - bbox_x1, bbox_y2 - bbox_y1);
        m_screenRenderer->draw2DRect( bbox_r, bgColor );
    }

    m_screenRenderer->setMaterial( quads.material );
    quads.draw();
    quads.destroy();

    //m_Driver->draw2D( quads );
    //draw2DTextDebug( x,y,msg,color,align,font);
}

void
FontRenderer::draw2DDebug(Font const & font)
{
    if (!m_driver) return;
    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    if ( w < 2 || h < 2 ) return;

    const auto face = getFontFace( font );
    if (!face)
    {
        DE_RUNTIME_ERROR(font.toString(),"No font face")
        return;
    }

    const Image* atlasImage = face->getAtlasImage();
    if (!atlasImage) { DE_ERROR("No atlas image") return; }

    std::string uri = atlasImage->uri();
    //DE_DEBUG("Font: ", font.toString(), " has atlasURI(", uri, "), msg(",dbStr(msg),")")

    SamplerOptions const so( 1.0f,
                            SamplerOptions::Minify::Linear,
                            SamplerOptions::Magnify::Linear,
                            SamplerOptions::Wrap::ClampToEdge,
                            SamplerOptions::Wrap::ClampToEdge );

    auto tex = m_driver->getTexture( uri );
    if (!tex)
    {
        tex = m_driver->createTexture2D( uri, *atlasImage, so );
        DE_ERROR("Upload atlas tex(", uri, ")")
        if (!tex)
        {
            DE_ERROR("No atlas tex(", uri, ") created")
            return;
        }
    }

    SMeshBuffer quad( PrimitiveType::Triangles );
    S3DVertex A( 0, 0, 0, 0,0,1, 0xFFFFFFFF, 0, 0 );
    S3DVertex B( w, 0, 0, 0,0,1, 0xFFFFFFFF, 1, 0 );
    S3DVertex C( w, h, 0, 0,0,1, 0xFFFFFFFF, 1, 1 );
    S3DVertex D( 0, h, 0, 0,0,1, 0xFFFFFFFF, 0, 1 );

    quad.addVertex( A );
    quad.addVertex( B );
    quad.addVertex( C );
    quad.addVertex( A );
    quad.addVertex( C );
    quad.addVertex( D );
    quad.setCulling( Culling::disabled() );
    quad.setBlend( Blend::alphaBlend() );
    quad.setTexture( 0, tex );
    quad.getMaterial().CloudTransparent = true;

    m_screenRenderer->setMaterial( quad.material );
    quad.draw();
    quad.destroy();
}

void
FontRenderer::draw2DTextDebug( int x, int y, std::wstring const & msg, uint32_t color,
                            Align align, Font const & font )
{
   if (msg.empty()) return;
   if (!m_driver) return;
   int w = m_driver->getScreenWidth();
   int h = m_driver->getScreenHeight();
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
      if ( glyph.ref.w() > 0 && glyph.ref.h() > 0 )
      {
         float x1 = px + kerning.x; // + glyph.bmp.getX();
         float y1 = py - glyph.ref.y();

         float gw = float( glyph.ref.w() );
         float gh = float( glyph.ref.h() );
         if ( font.lcdFit ) gh /= 3;
         float x2 = x1 + gw;
         float y2 = y1 + gh;
         float z1 = pz;

         Rectf const & texCoords = glyph.ref.texCoords();
         float u1 = texCoords.u1();
         float v1 = texCoords.v1();
         float u2 = texCoords.u2();
         float v2 = texCoords.v2();
         S3DVertex A( x1, y2, z1, 0,0,0, 0x4F0000FF, u1, v2 );
         S3DVertex B( x1, y1, z1, 0,0,0, 0x4F00FF00, u1, v1 );
         S3DVertex C( x2, y1, z1, 0,0,0, 0x4FFF0000, u2, v1 );
         S3DVertex D( x2, y2, z1, 0,0,0, 0x4F00FFFF, u2, v2 );

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

   m_screenRenderer->setMaterial(lines.material);
   lines.draw();
   lines.destroy();
   //m_Driver->draw2D( lines );

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
