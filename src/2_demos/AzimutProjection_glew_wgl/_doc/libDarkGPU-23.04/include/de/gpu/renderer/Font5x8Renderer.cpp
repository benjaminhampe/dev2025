#include <de/gpu/renderer/Font5x8Renderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

void
Font5x8Renderer::add5x8ColorQuadsToMeshBuffer(
   SMeshBuffer & o, int x, int y,
   std::string const & msg, uint32_t color, Align align, Font5x8 font )
{
   auto face = getFont5x8( font );

   //cacheString( msg );

   o.setPrimitiveType( PrimitiveType::Triangles );
   o.setLighting( 0 );
   //o.setBlend( Blend::disabled() );
   //o.setCulling( false );
   o.moreVertices( 4*msg.size() );
   o.moreIndices( 6*msg.size() );

   int cx = font.quadWidth + font.quadSpacingX;
   int cy = font.quadHeight + font.quadSpacingY;
   int gw = 5 * cx + font.glyphSpacingX;
   int gh = 8 * cy + font.glyphSpacingY;

   // Align: default is TopLeft
   glm::ivec2 aligned_pos(x,y);

   if ( align != Align::TopLeft )
   {
      auto ts = face->getTextSize( msg ); // Optimize here for single lines, default: multiline but slower.
      float tw = ts.x;
      float th = ts.y;

           if ( align & Align::Center ){ aligned_pos.x -= tw/2; }
      else if ( align & Align::Right ) { aligned_pos.x -= tw;   }
      else {}
           if ( align & Align::Middle ){ aligned_pos.y -= th/2; }
      else if ( align & Align::Bottom ){ aligned_pos.y -= th;   }
      else {}
   }

   auto g = aligned_pos;

   for ( size_t u = 0; u < msg.size(); ++u )
   {
      uint32_t ch = msg[ u ];

      if ( ch == '\r' )  // Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }
      if ( ch == '\n' )	// Mac or Windows line breaks.
      {
         g.x = aligned_pos.x;
         g.y += gh;
         continue;
      }

      //char ch = char( std::clamp( unicode, uint32_t(30), uint32_t(127) ) );
      Font5x8Glyph & glyph = face->getGlyph( ch );

      // Add dot matrix 5mal8 as max 49 quads
      for ( int j = 0; j < 8; ++j ) {
      for ( int i = 0; i < 5; ++i ) {

         int x1 = g.x + i * cx;
         int y1 = g.y + j * cy;

         int x2 = x1 + font.quadWidth;
         int y2 = y1 + font.quadHeight;

         if ( glyph.get( i,j ) )
         {
            S3DVertex A( x1, y1, -1, 0,0,-1, color, 0,1 );
            S3DVertex B( x1, y2, -1, 0,0,-1, color, 0,0 );
            S3DVertex C( x2, y2, -1, 0,0,-1, color, 1,0 );
            S3DVertex D( x2, y1, -1, 0,0,-1, color, 1,1 );
            o.addQuad( A,B,C,D, true );
         }
      }
      }

      g.x += gw;
   }
}

// ===========================================================================


Font5x8Renderer::Font5x8Renderer( VideoDriver* driver )
   : m_Driver( driver )
{}

Font5x8Renderer::~Font5x8Renderer()
{}

glm::ivec2
Font5x8Renderer::getTextSize( std::string const & msg, Font5x8 const & font ) const
{
   return Font5x8::getTextSize( msg, font );
}

void
Font5x8Renderer::add2DText(
   SMeshBuffer & o,
   int x,
   int y,
   std::string const & msg,
   uint32_t color,
   Align align,
   Font5x8 const & font )
{
   add5x8ColorQuadsToMeshBuffer( o,x,y,msg,color,align,font );
}

void
Font5x8Renderer::draw2DText(
   int x,
   int y,
   std::string const & msg,
   uint32_t color,
   Align align,
   Font5x8 const & font )
{
   if ( !m_Driver )
   {
      DE_ERROR("No driver")
      return;
   }

   SMeshBuffer o;
   add5x8ColorQuadsToMeshBuffer( o, x, y, msg, color, align, font );
   m_Driver->draw2D( o );
}



} // end namespace gpu.
} // end namespace de.

